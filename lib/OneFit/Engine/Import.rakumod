unit module OneFit::Engine::Import;


class Import is export {
	has @!Input-files;
	has $!filename;
	has $!path = '.';
	has %!options = ();

     multi method path ($folder) { 
		$!path = $folder;
		$folder.IO.mkdir unless $folder.IO.e;
		self 
	}

    multi method path () { $!path }

	multi method input-files () { @!Input-files }

	multi method input-files ( @files ) { @!Input-files= @files; self }

    multi method filename () { @!Input-files[0] }
    
	method filter-with (%options, Bool :q(:$quiet) = False) {
		my @files=();
		%!options = %!options, %options;
		
		@files = self.import(:quiet($quiet));

		return @files;
	}
	multi method import (:@infiles, Bool :q(:$quiet) = False) {
		my @input-files = @infiles.so ?? @infiles !! @!Input-files;
		my @files=();
		for @input-files -> $file {
			given self.is-type($file) {
				note "\nFile $file is type: ", $_ unless $quiet;
				when 'zip' {
					my @files-in-zip = gather for shell("unzip -Z1 $file",:out).out(:close).lines { take $_.IO.basename if $_.split("/").tail.so and $_.IO.basename !eq %!options<sef-R1-file> }
					shell "unzip -jo $file";
					@files.push: self.import( infiles => @files-in-zip ).Slip
	    		}	
				when 'fitteia-blocks' 	{ @files.push: self!fitteia-blocks($file).Slip }
				when 'stelar-hdf5' 		{ 
					if %!options<R1> { @files.push: self.import('stelar-hdf5-R1', file => $file).Slip }
					else { @files.push: self.import('stelar-hdf5', file => $file).Slip }	
				}
				when 'stelar-sdf'  		{ 
					if %!options<R1> { @files.push: self.import('stelar-sdf-R1', file => $file).Slip }
					else { @files.push: self.import('stelar-sdf', file => $file).Slip }
				}
				when 'stelar-sef-Mz'  	{ @files.push: self.import('stelar-sef-Mz', file => $file).Slip }
				when 'stelar-sef-R1'  	{ @files.push: self.import('stelar-sef-R1', file => $file).Slip }
				when 'ist-ffc'			{ 
					if %!options<R1> { @files.push: self.import('ist-ffc-R1', file => $file).Slip }
					else { @files.push: self.import('ist-ffc', file => $file).Slip }
				}
				default {
		   			@files.push: $file;
		   			$file.IO.copy("{self.path}/$file");
				}
			}
		}
		set-errors(self.path,@files,%!options<err>) if %!options<err>.so;
		return @files; 
	}


	multi method import ('stelar-hdf5', :$file ) { self!stelar-hdf5-Mz( file => $file ) }
	multi method import ('stelar-hdf5-R1', :$file ) { self!stelar-hdf5-R1( file => $file ) }

	multi method import ('stelar-sdf', :$file ) { self!stelar-sdf-Mz( file => $file ) }
	multi method import ('stelar-sdf-R1', :$file ) { self!stelar-sdf-Mz( Im => True, file => $file ) }
	multi method import ('stelar-sef-Mz', :$file ) { self!stelar-sef-Mz( file => $file ) }
	multi method import ('stelar-sef-R1', :$file ) { self!stelar-sef-R1( file => $file ) }

	multi method import ('ist-ffc', :$file) { self!ist-ffc( file => $file ) }
	multi method import ('ist-ffc-R1', :$file) { self!ist-ffc-R1( file => $file ) }

	method !fitteia-blocks ($file) {
		my @files;
		my @blocks = $file.IO.slurp.split(/'#' <ws> DATA <ws>/);
		for (1 ..^ @blocks.elems) -> $i {
			my $file-name="{$file.IO.extension('').Str}-block{ sprintf('%03d',$i.Int) }.dat";
			$file-name = $file unless @blocks.elems > 2;
			@files.push: $file-name; 
			"{self.path}/$file-name".IO.spurt: "# DATA { @blocks[$i] }";
			LAST { note "processed blocks: { @blocks.elems - 1 }" }
		}
		return @files;	
	}

	method !stelar-hdf5-Mz (:$file) {
		my %options = %!options;
		my $stelar-hdf5 = self.filename();
		$stelar-hdf5 = $file if $file.so;
		my $path = self.path();
		my $Re = %!options<Re>;
		my $Im = %!options<Im>;
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
	    	my 	$header = "# DATA dum = $BR\n# TAG = { $datafile.IO.extension('').Str }\n# R1 = "
					~
					$buf.split("ATTRIBUTE")[4].split('(0):')[1].words.head;
				$header ~= "\n# fit if " ~ %options<fit-if> ~ "\n" if %options<fit-if>.so;
				$header ~= "# plot if " ~ %options<plot-if> ~ "\n" if %options<plot-if>.so;

	    	my $sqr =  { $^a.map({ $_ ** 2 }) };
	    	my @module = ($sqr(@Re_) Z+ $sqr(@Im_))>>.sqrt;
	    	my $mmax = @module.max;
			my $rmax = @Re_.max;
			my $imax = @Im_.max;
	    	my @y = @module.map({ $_ / $mmax }) if !$Re and !$Im;
	    	@y = @Re_.map({ $_ / $rmax }) if $Re;
	    	@y = @Im_.map({ $_ / $imax }) if $Im;
	    	my @err = (1 .. @x.elems).map({1});

	    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
	    	@data-files.push: $datafile;
			LAST { note "processed zones: { @zones.elems }" }
		}
		"$path/$stelar-hdf5".IO.unlink;
		return @data-files.sort.reverse;
    }

    method !stelar-hdf5-R1 (:$file ) {
		my $stelar-hdf5 = self.filename();
		$stelar-hdf5 = $file if $file.so;
		my $path = self.path;
		$stelar-hdf5.IO.copy: "$path/$stelar-hdf5";
		my @zones = gather for shell("cd $path && h5dump -n $stelar-hdf5",:out).out.slurp(:close).lines { take $_.words.tail if $_.contains(/t1_fit/) }
		my @BR;
		my @R1;
		my $err = %!options<err> if %!options.so;
		$err = $err.contains("%") ?? $err.subst("%","").Num /100 !! "";
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

	method !stelar-sdf-Mz (:$file) {
		my %options=%!options;
		my $Re = %options<Re>.so ?? %options<Re> !! False;
		my $Im = %options<Im>.so ?? %options<Im> !! False;
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
		    	my 	$header = "# DATA dum = $BR\n# TAG = { $datafile.IO.extension('').Str }\n";
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
						my @re = gfilt-shell(@Re.splice(0,$BS.Int)[$i .. $f],%options<gfilt>); 
						my @im = gfilt-shell(@Im.splice(0,$BS.Int)[$i .. $f],%options<gfilt>); 
						my $sqr =  { $^a.map({ $_ ** 2 }) };
	    				my @module = ($sqr(@re) Z+ $sqr(@im))>>.sqrt;
						
						@y.push: @module.sum/$N;
					}
					else { @y.push: @m.splice(0,$BS.Int)[$i .. $f].sum/$N; }
				}
				my $ymax=@y.max;
		    	@y = @y.map({ $_ / $ymax });
		    	my @err = (1 .. @x.elems).map({1});

		    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
		    	@data-files.push: $datafile;
				LAST { note "processed zones: { @zones.elems }" }
			}
		}
		"$path/$stelar-sdf".IO.unlink;
		return @data-files.sort.reverse;
    }

    method !stelar-sef-R1 (:$file) {
		my $stelar-sdf = self.filename();
		$stelar-sdf = $file if $file.so;
		my $path = self.path();
		my $err = %!options<err>;
		$err = $err.contains("%") ?? $err.subst("%","").Num /100 !! "";
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
			my $datafile = "{$stelar-sef.IO.extension('').Str}-z{sprintf('%03d',$_+1)}.dat";  
			@zones[$_] = (@tau Z @Mz.map({ $_/$max}))>>.join(" ").join("\n");
			"{self.path}/{$stelar-sef.IO.extension('').Str}-z{sprintf('%03d',$_+1)}.dat".IO.spurt: "# DATA dum = {$_+1} \n# TAG = { $datafile.IO.extension('').Str }\n" ~ @zones[$_].join("\n");
			@files.push: $datafile;
			LAST { note "processed zones: { @zones.elems }" }
		}
		if %!options<sef-R1-file> { @files = merge(self.path,%!options<sef-R1-file>,@files) }
		return  @files;
    }

	method !ist-ffc (:$file) {
		my $ffc = self.filename();
		$ffc = $file if $file.so;
		my $path = self.path();
		$ffc.IO.copy: "$path/$ffc";
		my @files;
		my @aux = "$path/$ffc".IO.slurp(:close).split(/endtau\n|dumTau|shiFdt|Parameters/)[0,1];	
		my @freqs = gather for @aux[1].lines { take $_.split(',')[2] }
		my @modes = gather for @aux[1].lines { take $_.split(',')[4] }
		my @ntaus  = gather for @aux[1].lines { take $_.split(',')[6] }
		my @lines;	
		for @aux[0].lines {
			my @a = $_.split(',')[2,3];
			@lines.push: @a[0]*1e-6 ~ ' ' ~ @a[1]; 
		}
		my $empty=@ntaus.elems;
		my $proc=0;
		for (1 .. @ntaus.elems) {
			$proc++;
			my @zone = @lines.splice(0,@ntaus[$_-1].Int);
			my $datafile = "{ $ffc.IO.extension('').Str }-{ sprintf('%09d',(@freqs[$_-1]*1e3).Int) }-z{ sprintf('%03d',$_) }.dat";
			my 	$header = "# DATA dum = @modes[$_-1] { @freqs[$_-1]*1e3 }\n# TAG = { $datafile.IO.extension('').Str }\n";
				$header ~= "# fit if " ~ %!options<fit-if> ~ "\n" if %!options<fit-if>.so;
				$header ~= "# plot if " ~ %!options<plot-if> ~ "\n" if %!options<plot-if>.so;

			my @x;
			my @y;
			for @zone {
			   	my @w = $_.words;	
				@x.push: @w[0].Num;
				@y.push: @w[1].Num;
			}
			my $max = @y.max;	
			next if $max.Num == 0;
			$empty--;
			@zone = (@x Z @y.map({ $_/$max}))>>.join(" ").join("\n");
			"$path/$datafile".IO.spurt: "$header\n" ~ @zone.join("\n") ~ "\n\n";
			@files.push: $datafile;
			last if @lines.elems <= 0;
			LAST { note "processed zones: $proc/{ @ntaus.elems }, $empty empty" }
		}
		return @files.sort.reverse
	}
    method !ist-ffc-R1 (:$file) {
		my $ist-ffc = self.filename();
		$ist-ffc = $file if $file.so;
		my $path = self.path;
		my @f;
		my @R1;
		my @err;
		my $erro = %!options<err>;
		$erro = $erro.contains("%") ?? $erro.subst("%","").Num /100 !! "";
		$ist-ffc.IO.copy: "$path/$ist-ffc";
		for "$ist-ffc".IO.lines.grep(/^<![#]>/) {
			my @a = $_.split(',')[1,2,3];
			@f.push: @a[0]*1e3;
			@R1.push: 1e6/@a[1];
			@err.push: $erro.Bool ?? 1e6/@a[1]*$erro !! @a[2]/@a[1]*1e6/@a[1];
		}
			
		"$path/$ist-ffc".IO.extension('dat').spurt:  (@f  Z @R1 Z @err).join("\n") ~ "\n\n";
		return $ist-ffc.IO.extension('dat').Str;
    }


	method is-type ($file)  {
	   	return self.is-hdf5($file) ?? 'stelar-hdf5' !! 
			self.is-zip($file) ?? 'zip' !! 
			self.is-block($file) ?? 'fitteia-blocks' !! 
			self.is-sdf($file) ?? "stelar-sdf" !! 
			self.is-ffc($file) ?? 'ist-ffc' !! 
			self.is-sef-Mz($file) ?? "stelar-sef-Mz" !! 
			self.is-sef-R1($file) ?? "stelar-sef-R1" !! "";	
	}

	method is-hdf5 ($file)  { 
		return $file.IO.open(:bin).read(8,:close) eq Buf[uint8].new(0x89, 0x48, 0x44, 0x46, 0x0D, 0x0A, 0x1A, 0x0A); 
	}
	method is-zip($file)    { return $file.IO.open(:bin).read(4,:close) eq Buf[uint8].new(0x50, 0x4B, 0x03, 0x04) }
	method is-block ($file) { return $file.IO.slurp(:close).contains(/'#' <ws> DATA <ws>/) }
	method is-sdf ($file) 	 { 
		return $file.IO.slurp(:enc('utf8'),:close).contains(/T1MAX/) 
	}
	method is-sef-R1 ($file) 	 { 
		return $file.IO.slurp(:enc('utf8'),:close).contains(/_BRLX__/) 
	}
	method is-sef-Mz ($file) 	 { 
		return $file.IO.slurp(:enc('utf8'),:close).contains(/MAGNITUDES\n/) 
	}
	method is-ffc ($file) 	 { 
		return $file.IO.slurp(:enc('utf8'),:close).contains(/:i endtau|FFC001/) 
	}


	sub merge ($path, $R1-file, @files is copy) {
		my @BR = gather for $R1-file.IO.lines(:close) { take $_.words.head if $_.contains(/^\s*\d/) }
		for 0 ..^ @files.elems {
		   	my $file = @files[$_];	
			$file = $file.subst(/z\d+/,sprintf("%09d-z%03d",(@BR[$_]*1e6).Int,$_+1));
			shell("sed -E -i -e 's/dum = [0-9]+/dum = { @BR[$_]*1e6 }/' -e 's/TAG = .*/TAG = { $file.IO.extension('').Str }/' $path/@files[$_]");
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

	sub set-err-split($filename, $err) {
		my $tmp="/tmp/{$*PID}-lixo.txt";
		$filename.IO.copy($tmp);
		my @errs=(+0,+0);
		my @Ys;
		my @means=(+0,+0);
		my @xy;
		my @Ns=(+0,+0);
		my $split-at = $err.words.tail;
		for $filename.IO.lines(:close) {
			@xy.push($_.words.join(" ")) if none($_.contains("#"),$_.words.elems < 2);
		}		
		for @xy.sort {
			if $_.words[0] <= $split-at { 
				@Ys[0].push: $_.words[1];
	   		}
			else { 
				@Ys[1].push: $_.words[1]; 
	   		}
		}

		for 0 ..^@Ys.elems {
			my $N = @Ys[$_].elems;
			my $Y2 = @Ys[$_].map({ $_ ** 2 }).sum; 
			if $err.contains(/ 'std' | 'standard' <ws> 'deviation' /) {
				my $mean = @Ys[$_].sum/$N;
				@errs[$_]	= sqrt(abs($Y2 - $N*$mean**2)/($N-1));
			}
			else {
				my $mean = @Ys[$_]>>.abs.sum/$N;
				@errs[$_] = $mean*$err.split("%").head.Num/100;
			}
		}
		$filename.IO.spurt: shell("awk -v err1={@errs[0]} -v err2={@errs[1]} -v splitAt={$split-at} ' \{ if ( !/#/ && NF>2 ) \{ \$3 = ( \$1 <= splitAt ) ?  err1 : err2; print; } else \{ print; \} \}' $tmp", :out).out.slurp;
	
   		unlink $tmp;		
	}

	sub set-err($filename,$err is copy) {
		my $tmp = "/tmp/{$*PID}-lixo.txt";	
		$filename.IO.copy($tmp);
		if $err.contains(/'std' | 'standard' <ws> 'deviation'/) {
			note "using the standard deviation of your dependent variable to calculate its uncertainty";
			my @Y;
			for $filename.IO.lines(:close) {
				my @xy = $_.words;
				if none($_.contains("#"),@xy.elems < 2) {
					@Y.push: @xy[1];
				}		
			}		
			my $N=@Y.elems;
			my $mean=@Y.sum/$N;
			my $Y2 = @Y.map({ $_ ** 2 }).sum; 
			$err = sqrt(abs($Y2 - $N*$mean**2)/($N-1));
		}
		elsif $err.contains(/'avg' | 'average'/) {
			note "using the average of the absolute value of your dependent variable to calculate its uncertainty";
			my @Y;
			for $filename.IO.lines(:close) {
				my @xy = $_.words;
				if none($_.contains("#"),@xy.elems < 2) {
					@Y.push: abs(@xy[1]);
				}		
			}
			my $N=@Y.elems;
			$err = @Y.sum/$N*$err.split("%").head.Num/100;
		}
		elsif $err.contains("%") and !$err.contains(/ 'avg' | 'average'/) {
			$err = '$2*' ~ $err.subst("%","").Num /100 ;
		}
		else { $err = $err }

		$filename.IO.spurt: shell("awk '\{ if (!/#/ && NF>2) \{ \$3=$err; print \} else \{ print \}\}' $tmp", :out).out.slurp;
	   unlink $tmp;	
	}

	sub set-errors($path,@files,$err) {
		for @files {
			if $err.contains("split") { set-err-split("$path/$_",$err); }
			else { set-err("$path/$_", $err); }
		}
	}


}



