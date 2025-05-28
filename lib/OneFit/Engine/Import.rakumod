unit module OneFit::Engine::Import;


class Import is export {
	has @!Input-files;
	has $!filename;
	has $!path = '.';
	has %!options = 'stelar-hdf5' 	 	=> False,
					'stelar-hdf5-Re' 	=> False,
					'stelar-hdf5-Im' 	=> False,
					'stelar-hdf5-R1' 	=> False,
					'stelar-hdf5-R1-err'=> "",
					'stelar-sdf-Mz'	 	=> "",
					'stelar-sdf-Re'	 	=> False,
					'stelar-sdf-Im'	 	=> False,
					'stelar-sef-Mz'	 	=> False,
					'stelar-sef-R1'	 	=> "",
					'stelar-sef-R1-err'	=> "",
					'ist-ffc'			=> False,
					'ist-ffc-R1'		=> False,
					'ist-ffc-R1-err'	=> "";

     multi method path ($folder) { 
		$!path = $folder;
		$folder.IO.mkdir unless $folder.IO.e;
		self 
	}

    multi method path () { $!path }

	multi method input-files () { @!Input-files }

	multi method input-files ( @files ) { @!Input-files= @files; self }

    multi method filename () { @!Input-files[0] }
    
	method filter-with (%options) {
		my @files=();
		%!options = %!options, %options;
		given %!options.values.grep(*.so).elems  {
			when 0 { @files = self.import() }
		    when 1 {	
				for %!options.kv -> $k,$v {
					if $v.so { 
						if $k.contains(/err/) { @files = self.import($k, :err($v)) }	
						elsif $k.contains(/sdf/) and $v.so { @files = self.import($k, :rfpoptions($v)) }
						elsif $k.contains(/hdf5/) and $v.so { @files = self.import($k, :fpoptions($v)) }
						else  { @files = self.import($k) }
					}
				}
			}
			when 2 {
				if %!options<stelar-sef-Mz> and %!options<stelar-sef-R1>.so { @files = merge(self.path,%!options<stelar-sef-R1>,self.import()) }
			   	else { say "too many stelar options selected!" }	
			}
			default { say "too many stelar options selected!" }
		}
		return @files;
	}
	multi method import (:@infiles) {
		my @input-files = @infiles.so ?? @infiles !! @!Input-files;
		my @files=();
		for @input-files -> $file {
			given is-type($file) {
				say "file $file is type: ",$_;
				when 'zip' {
					my @files-in-zip = gather for shell("unzip -Z1 $file",:out).out(:close).lines { take $_.IO.basename if $_.split("/").tail.so and $_.IO.basename !eq %!options<stelar-sef-R1> }
					shell "unzip -jo $file";
					@files.push: self.import( infiles => @files-in-zip ).Slip
	    		}	
				when 'fitteia-blocks' 	{ @files.push: self!fitteia-blocks($file).Slip }
				when 'stelar-hdf5' 		{ @files.push: self.import('stelar-hdf5', file => $file).Slip }
				when 'stelar-sdf'  		{ @files.push: self.import('stelar-sdf', file => $file).Slip }
				when 'stelar-sef-Mz'  	{ @files.push: self.import('stelar-sef-Mz', file => $file).Slip }
				when 'stelar-sef-R1'  	{ @files.push: self.import('stelar-sef-R1', file => $file).Slip }
				when 'ist-ffc'			{ @files.push: self.import('ist-ffc', file => $file).Slip }
				default {
		   			@files.push: $file;
		   			$file.IO.copy("{self.path}/$file");
				}
			}
		}
		return @files; 
	}


	multi method import ('stelar-hdf5', :$file, :$fpoptions ) { self!stelar-hdf5-Mz( file => $file, fpoptions => $fpoptions ) }
	multi method import ('stelar-hdf5-Re', :$file, :$fpoptions ) { self!stelar-hdf5-Mz( Re => True, file => $file, fpoptions => $fpoptions ) }
	multi method import ('stelar-hdf5-Im', :$file, :$fpoptions ) { self!stelar-hdf5-Mz( Im => True, file => $file, fpoptions => $fpoptions ) }
	multi method import ('stelar-hdf5-R1', :$file, :$fpoptions ) { self!stelar-hdf5-R1( file => $file, ftpoptions => $fpoptions ) }
	multi method import ('stelar-hdf5-R1-err', :$file, :$err ) { self!stelar-hdf5-R1( err => $err, file => $file ) }

	multi method import ('stelar-sdf', :$file, :$rfpoptions) { self!stelar-sdf-Mz( file => $file, rftpoptions => $rfpoptions ) }
	multi method import ('stelar-sdf-Mz', :$file, :$rfpoptions) { self!stelar-sdf-Mz( file => $file, rfpoptions => $rfpoptions ) }
	multi method import ('stelar-sdf-Re', :$file, :$rfpoptions) { self!stelar-sdf-Mz( Re => True, file => $file, rfpoptions => $rfpoptions ) }
	multi method import ('stelar-sdf-Im', :$file, :$rfpoptions) { self!stelar-sdf-Mz( Im => True, file => $file, rfpoptions => $rfpoptions ) }
	multi method import ('stelar-sef-Mz', :$file) { self!stelar-sef-Mz( file => $file ) }
	multi method import ('stelar-sef-R1', :$file) { self!stelar-sef-R1( file => $file ) }
	multi method import ('stelar-sef-R1-err', :$file, :$err) { self!stelar-sef-R1( file => $file, err => $err ) }

	multi method import ('ist-ffc', :$file) { self!ist-ffc( file => $file ) }
	multi method import ('ist-ffc-R1', :$file) { self!ist-ffc-R1( file => $file ) }
	multi method import ('ist-ffc-R1-err', :$file, :$err) { self!ist-ffc-R1( err => $err, file => $file  ) }

	method !fitteia-blocks ($file) {
		my @files;
		my @blocks = $file.IO.slurp.split(/'#' <ws> DATA <ws>/);
		for (1 ..^ @blocks.elems) -> $i {
			my $file-name="{$file.IO.extension('').Str}-block{ sprintf('%03d',$i.Int) }.dat";
			$file-name = $file unless @blocks.elems > 2;
			@files.push: $file-name; 
			"{self.path}/$file-name".IO.spurt: "# DATA { @blocks[$i] }";
		}
		return @files;	
	}

	method !stelar-hdf5-Mz (:$file, Bool :$Re, Bool :$Im, :$fpoptions) {
		my %options;
		if $fpoptions.so {
			my %aux = $fpoptions.split(':');
		   	for %aux.kv -> $k,$v { 
				%options<fit-if> =$v if $k.contains(/^f/);
				%options<plot-if>=$v if $k.contains(/^p/);
			}
		}	
		my $stelar-hdf5 = self.filename();
		$stelar-hdf5 = $file if $file.so;
		my $path = self.path();
		$stelar-hdf5.IO.copy: "$path/$stelar-hdf5";
		my @zones = gather for shell("cd $path && h5dump -n $stelar-hdf5",:out).out.slurp(:close).lines { take $_.words.tail if $_.contains(/t1_fit/) }
		my @data-files;
		for ( 1 .. @zones.elems ).race {
	    	my @x;
	    	my @Re_;
	    	my @Im_;
	    	my $buf = shell("cd $path && h5dump -d /zone{$_}/t1_fit $stelar-hdf5",:out).out.slurp(:close);  
	    	for $buf.lines {
				my @c = $_.words;
				@x.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(0\,\d+\)/);
				@Re_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(1\,\d+\)/);
				@Im_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(2\,\d+\)/);
	    	}
			my $BR = ($buf.split("ATTRIBUTE")[1].split('(0):')[1].words.head.Rat * 1e6).round(0.0001);
	    	my $datafile = "{$stelar-hdf5.IO.extension('').Str}-{ sprintf('%09d',$BR.Int) }-z{ sprintf('%03d',$_.Int) }.dat";
	    	my 	$header = "# DATA dum = $BR\n# TAG = zone{$_}\n# R1 = "
					~
					$buf.split("ATTRIBUTE")[4].split('(0):')[1].words.head;
				$header ~= "\n# fit if " ~ %options<fit-if> ~ "\n" if %options<fit-if>.so;
				$header ~= "# plot if " ~ %options<plot-if> ~ "\n" if %options<plot-if>.so;

	    	my $sqr =  { $^a.map({ $_ ** 2 }) };
	    	my @module = ($sqr(@Re_) Z+ $sqr(@Im_))>>.sqrt;
	    
	    	my @y = @module.map({ $_ / @module.max }) if !$Re and !$Im;
	    	@y = @Re_.map({ $_ / @Re_.max }) if $Re;
	    	@y = @Im_.map({ $_ / @Im_.max }) if $Im;
	    	my @err = (1 .. @x.elems).map({1});

	    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
	    	@data-files.push: $datafile;
		}
		"$path/$stelar-hdf5".IO.unlink;
		return @data-files.sort.reverse;
    }

    method !stelar-hdf5-R1 (:$file, Rat :$err) {
		my $stelar-hdf5 = self.filename();
		$stelar-hdf5 = $file if $file.so;
		my $path = self.path;
		$stelar-hdf5.IO.copy: "$path/$stelar-hdf5";
		my @zones = gather for shell("cd $path && h5dump -n $stelar-hdf5",:out).out.slurp(:close).lines { take $_.words.tail if $_.contains(/t1_fit/) }
		my @BR;
		my @R1;
		for @zones.hyper {
	    	for shell("cd $path && h5dump -d $_ $stelar-hdf5",:out).out.slurp(:close) {
				my @c = $_.split: "ATTRIBUTE";
				@c = gather for @c { take $_  if $_.contains(/'"BR"'|'"R1"'/) }
				@BR.push: @c[0].split('(0):')[1].words.head;
				@R1.push: @c[1].split('(0):')[1].words.head;
	    	}
		}
		"$path/$stelar-hdf5".IO.extension('dat').spurt:  (@BR.map({ ($_.Num * 1e6).round(0.0001) }) Z @R1 Z @R1.map({ $_.Num * (($err.Bool) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
		return $stelar-hdf5.IO.extension('dat').Str;
    }

	method !stelar-sdf-Mz (:$file, Bool :$Re, Bool :$Im, :$rfpoptions) {
		my %aux = $rfpoptions.so ?? $rfpoptions.split(':') !! ("range","0..end");
		my %options;
	   	for %aux.kv -> $k,$v { 
			%options<range>  =$v if $k.contains(/^r/);
			%options<fit-if> =$v if $k.contains(/^f/);
			%options<plot-if>=$v if $k.contains(/^p/);
			%options<gfilt>=$v if $k.contains(/^g/);
		}
		my @window-range = %options<range>.so ?? %options<range>.split(/\D+/) !! <0 end>;
		my $stelar-sdf = self.filename();
		$stelar-sdf = $file if $file.so;
		my $path = self.path();
		$stelar-sdf.IO.copy: "$path/$stelar-sdf";
	    my $buf = $stelar-sdf.IO.slurp(:close);
		my @tau-zones = $buf.split(/SUMMARY/);
		my @data-files;
		for @tau-zones[1..*] {
			my $buf = $_;
 			my @zones = $buf.split(/ZONE/);
			my $BS = @zones[0].split(/BS <ws> '=' <ws>/)[1].words.head.Rat;
			my @aux = @zones[0].split(/TAU <ws> '=' <ws>/)[1].words.head.trans([ "[", "]" ] => "").split(':');
			my $type = @aux.head;
			my @range = @aux[1,2].map({  $_.subst('*T1MAX','').Rat });
			my $tauf = @range.max; 
			my $taui = @range.min;
			my $ntaus = @aux.tail;

			my $i = @window-range[0].Int;
			my $f = @window-range[1].subst("end",$BS-1).Int; 
			$i = 0 if $i < 0;
			$f = $BS - 1 if $f >= $BS;
			my $N = $f - $i + 1;

			for ( 1 ..^ @zones.elems ).race {
				my $buf=@zones[$_];
				my $BR = ($buf.split(/BR <ws> '=' <ws>/)[1].words.head.Rat * 1e6).round(0.0001);
				my $index=$buf.words.head.split('.').map({ sprintf('%03d',$_.Int) }).join('_');
				my $datafile = "{$stelar-sdf.IO.extension('').Str}-{ sprintf('%09d',$BR.Int) }-z{ $index }.dat";
				my $T1MAX =	$buf.split(/T1MAX <ws> '=' <ws>/)[1].words.head.Rat * 1e-6;
		    	my 	$header = "# DATA dum = $BR\n# TAG = zone{ $index }\n";
					$header ~= "# fit if " ~ %options<fit-if> ~ "\n" if %options<fit-if>.so;
					$header ~= "# plot if " ~ %options<plot-if> ~ "\n" if %options<plot-if>.so;
				my @x;
				my @y;
				my @m;
				if $type eq "log" { @x = (0 ..^ $ntaus).map({ $taui * $T1MAX * ($tauf/$taui) ** ($_/($ntaus-1)) }) }
				else { @x = (0 ..^ $ntaus).map({ ($taui + ($tauf - $taui) * $_/($ntaus - 1) ) * $T1MAX }) }
				@x = @x.reverse if @range[1] < @range[0];
				my @Re;
				my @Im;
				@m = gather for $buf.lines {
					if %options<gfilt>.so {
						@Re.push: $_.words[0] if $_.contains(/^'-'?\d+/);
						@Im.push: $_.words[1] if $_.contains(/^'-'?\d+/);
					}
					else { take $_.words[ $Re ?? 0 !! $Im ?? 1 !! 2 ] if $_.contains(/^'-'?\d+/) }
				};
				
				for (1 .. $ntaus).race { 
					if %options<gfilt>.so {
						#						my $re =  @Re.splice(0,$BS.Int)[$i .. $f].sum/$N; 
						#						my $im =  @Im.splice(0,$BS.Int)[$i .. $f].sum/$N; 
			
						#						"/tmp/lixo{$_}.dat".IO.spurt: @Re.splice(0,$BS.Int)[$i .. $f].join("\n"); 
						#						my @re =  shell("gfilt $N { %options<gfilt> }  /tmp/lixo{$_}.dat",:out).out.lines(:close) ; 
						#						"/tmp/lixo1{$_}.dat".IO.spurt: @Im.splice(0,$BS.Int)[$i .. $f].join("\n"); 
						#						my @im =  shell("gfilt $N { %options<gfilt> }  /tmp/lixo1{$_}.dat",:out).out.lines(:close) ; 
						#						shell("rm /tmp/lixo*.dat");
			
						my @re = gfilt-shell(@Re.splice(0,$BS.Int)[$i .. $f],%options<gfilt>); 
						my @im = gfilt-shell(@Im.splice(0,$BS.Int)[$i .. $f],%options<gfilt>); 
						my $sqr =  { $^a.map({ $_ ** 2 }) };
	    				my @module = ($sqr(@re) Z+ $sqr(@im))>>.sqrt;
						
						@y.push: @module.sum/$N;
					}
					else { @y.push: @m.splice(0,$BS.Int)[$i .. $f].sum/$N; }
				}

		    	@y = @y.map({ $_ / @y.max });
		    	my @err = (1 .. @x.elems).map({1});

		    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
		    	@data-files.push: $datafile;
			}
		}
		"$path/$stelar-sdf".IO.unlink;
		return @data-files.sort.reverse;
    }

    method !stelar-sef-R1 (:$file, Rat :$err) {
		my $stelar-sdf = self.filename();
		$stelar-sdf = $file if $file.so;
		my $path = self.path();
		$stelar-sdf.IO.copy: "$path/$stelar-sdf";
		my @R1 = gather for "$path/$stelar-sdf".IO.lines(:close) { take $_.words[0,2] if $_.contains(/^\s*\d+/) } .map({ [ ($_[0] * 1e6).round(0.0001) ,$_[1]] }).Array;
		"$path/$stelar-sdf".IO.extension('dat').spurt:  (@R1 Z @R1.map({ $_[1].Rat * (($err.Bool) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
		return $stelar-sdf.IO.extension('dat').Str
    }

    method !stelar-sef-Mz (:$file) {
		my $stelar-sef = self.filename();
		$stelar-sef = $file if $file.so;
		my $path = self.path();
		$stelar-sef.IO.copy: "$path/$stelar-sef";
		my @zones = gather for $stelar-sef.IO.split(/MAGNITUDES\n/)[1..*] -> $zone { take gather for $zone.lines { take $_ if $_.contains(/^\s\d+/) } .join("\n"); }

		my @files;
		for 0 ..^ @zones.elems { 
			my (@tau,@Mz);
			for @zones[$_].lines {
				@tau.push: $_.words[0].Num;
				@Mz.push: $_.words[1].Num;
			}	
			my $max = @Mz.max;
			@zones[$_] = (@tau Z @Mz.map({ $_/$max}))>>.join(" ").join("\n");
			"{self.path}/{$stelar-sef.IO.extension('').Str}-z{sprintf('%03d',$_+1)}.dat".IO.spurt: "# DATA dum = {$_+1} \n# TAG = zone{$_+1}\n" ~ @zones[$_].join("\n");
			@files.push: "{$stelar-sef.IO.extension('').Str}-z{sprintf('%03d',$_+1)}.dat"  
		}
		return  @files;
    }

	method !ist-ffc (:$file) {
		my $ffc = self.filename();
		$ffc = $file if $file.so;
		my $path = self.path();
		$ffc.IO.copy: "$path/$ffc";
		my @files;
		my @aux = "$path/$ffc".IO.slurp(:close).split(/endtau\n|shiFdt/)[0,1];	
		my @freqs = gather for @aux[1].lines { take $_.split(',')[2] }
		my @modes = gather for @aux[1].lines { take $_.split(',')[4] }
		my @ntaus  = gather for @aux[1].lines { take $_.split(',')[5] }
		my @lines;	
	   	for @aux[0].lines {
			my @a = $_.split(',')[2,3];
			@lines.push: @a[0]*1e-6 ~ ' ' ~ @a[1]; 
		}
		for (1 .. @ntaus.elems) {
			my @zone = @lines.splice(0,@ntaus[$_-1].Int);
			my $datafile = "{ $ffc.IO.extension('').Str }-{ sprintf('%09d',(@freqs[$_-1]*1e3).Int) }-z{ sprintf('%03d',$_) }.dat";
			my $header = "# DATA dum = @modes[$_-1] @freqs[$_-1]\n# TAG = zone{ sprintf('%03d',$_) }";
			"$path/$datafile".IO.spurt: "$header\n" ~ @zone.join("\n") ~ "\n\n";
			@files.push: $datafile;
		}
		return @files.sort.reverse
	}
    method !ist-ffc-R1 (:$file, Rat :$err) {
		my $ist-ffc = self.filename();
		$ist-ffc = $file if $file.so;
		my $path = self.path;
		my @f;
		my @R1;
		my @err;
		$ist-ffc.IO.copy: "$path/$ist-ffc";
		for "$ist-ffc".IO.lines {
			my @a = $_.split(',')[1,2,3];
			@f.push: @a[0]*1e3;
			@R1.push: 1e6/@a[1];
			@err.push: $err.Bool ?? 1e6/@a[1]*$err !! @a[2]/@a[1]*1e6/@a[1];
		}
			
		"$path/$ist-ffc".IO.extension('dat').spurt:  (@f  Z @R1 Z @err).join("\n") ~ "\n\n";
		return $ist-ffc.IO.extension('dat').Str;
    }


	sub is-type ($file)  {
	   	return is-hdf5($file) ?? 'stelar-hdf5' !! 
			is-zip($file) ?? 'zip' !! 
			is-block($file) ?? 'fitteia-blocks' !! 
			is-sdf($file) ?? "stelar-sdf" !! 
			is-ffc($file) ?? 'ist-ffc' !! 
			is-sef($file) ?? "stelar-sef-Mz" !! 
			is-sef-R1($file) ?? "stelar-sef-R1" !! "";	
	}

	sub is-hdf5 ($file)  { return $file.IO.open(:bin).read(8,:close) eq Buf[uint8].new(0x89, 0x48, 0x44, 0x46, 0x0D, 0x0A, 0x1A, 0x0A) }
	sub is-zip($file)    { return $file.IO.open(:bin).read(4,:close) eq Buf[uint8].new(0x50, 0x4B, 0x03, 0x04) }
	sub is-block ($file) { return $file.IO.slurp(:close).contains(/'#' <ws> DATA <ws>/) }
	sub is-sdf ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/T1MAX/) }
	sub is-sef-R1 ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/_BRLX__/) }
	sub is-sef ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/MAGNITUDES\n/) }
	sub is-ffc ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/endtau/) }


	sub merge ($path, $R1-file, @files is copy) {
		my @BR = gather for $R1-file.IO.lines(:close) { take $_.words.head if $_.contains(/^\s*\d/) }
		for 0 ..^ @files.elems {
		   	my $file = @files[$_];	
			$file = $file.subst(/z\d+/,sprintf("%09d-z%03d",(@BR[$_]*1e6).Int,$_+1));
			shell("sed -E -i -e 's/dum = [0-9]+/BR = { @BR[$_]*1e6 }/' $path/@files[$_]");
		   	"$path/@files[$_]".IO.rename: "$path/$file";
			@files[$_]= $file
		}
		return @files.sort.reverse;
	}
		
	sub gfilt-shell(@a,$npts)  { 
		"/tmp/lixo.dat".IO.spurt: @a.join("\n"); 
		my @z =  shell("gfilt { @a.elems }  $npts  /tmp/lixo.dat",:out).out.lines(:close) ; 
		shell "rm /tmp/lixo.dat";
		return @z
	}	

	sub gfilt-raku(@a,$npts)  { 
		my @z;
		my $n1 = @a.elems-1;
		for (0 .. $n1) -> $i {
			my $sum=0;
			my $start= [0,$i-5*$npts].max;
			my $end =  [$i+5*$npts,$n1].min; 
			@z[$i]=0;
			for ($start .. $end) -> $j {
				my $exp=exp( -( ($i-$j)/(2*$npts) )**2 );
				@z[$i] += @a[$j]*$exp;
				$sum += $exp;
			}
		}	
		return @z
	}	
		
}



