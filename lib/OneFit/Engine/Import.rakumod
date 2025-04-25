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
					'stelar-sdf'	 	=> False,
					'stelar-sdf-Re'	 	=> False,
					'stelar-sdf-Im'	 	=> False,
					'stelar-sef-R1'	 	=> False,
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

	multi method input-files ( @files ) { @!Input-files= @files; 
	
	#		say (is-hdf5(@files[0]),is-zip(@files[0]),is-sdf(@files[0]),is-block(@files[0]),is-ffc(@files[0]));
	
	self }

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
						else  { @files = self.import($k) }
					}
				}
			}
			default { say "too many stelar options selected!" }
		}
		return @files;
	}
	
	multi method import ('stelar-hdf5') { self!stelar-hdf5-Mz() }
	multi method import ('stelar-hdf5-Re') { self!stelar-hdf5-Mz( Re => True ) }
	multi method import ('stelar-hdf5-Im') { self!stelar-hdf5-Mz( Im => True ) }
	multi method import ('stelar-hdf5-R1') { self!stelar-hdf5-R1() }
	multi method import ('stelar-hdf5-R1-err', :$err) { self!stelar-hdf5-R1( err => $err ) }

	multi method import ('stelar-sdf') { self!stelar-sdf-Mz() }
	multi method import ('stelar-sdf-Re') { self!stelar-sdf-Mz( Re => True ) }
	multi method import ('stelar-sdf-Im') { self!stelar-sdf-Mz( Im => True ) }
	multi method import ('stelar-sef-R1') { self!stelar-sef-R1() }
	multi method import ('stelar-sef-R1-err', :$err) { self!stelar-sef-R1( err => $err ) }

	multi method import ('ist-ffc') { self!ist-ffc() }
	#	multi method import ('ist-ffc1-R1') { self!ist-ffc1-R1() }
	#	multi method import ('ist-ffc1-R1-err	') { self!ist-ffc1-R1( err => $err ) }

	multi method import () {
		my @files=();
		for @!Input-files {
			say is-type($_); 
	    	if $_.IO.extension.Str ~~ /zip/ {
				shell "unzip $_ -d {self.path}";
				@files.push: self.path.IO.dir>>.Str.map({ $_.subst("{self.path}/",'')  }).sort.Slip;
	    	}	
	    	else {
				if $_.IO.slurp.contains(/'#' <ws> DATA <ws>/) {
					my @blocks = $_.IO.slurp.split(/'#' <ws> DATA <ws>/);
					for (1 ..^ @blocks.elems) -> $i {
						my $file-name="{$_.IO.extension('').Str}-block{ sprintf('%03d',$i.Int) }.dat";
						$file-name = $_ unless @blocks.elems > 2;
						@files.push: $file-name; 
						"{self.path}/$file-name".IO.spurt: "# DATA { @blocks[$i] }";
					}		 
				}	
				else {	
		   			@files.push: $_;
		   			$_.IO.copy("{self.path}/$_")
				}
	    	}
		}
		return @files; 
	}


	method !stelar-hdf5-Mz (Bool :$Re, Bool :$Im) {
		my $stelar-hdf5 = self.filename();
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
	    	my $datafile = "zone{ sprintf('%03d',$_.Int) }.dat";
	    	my $header = "# DATA dum = " ~
				($buf.split("ATTRIBUTE")[1].split('(0):')[1].words.head.Rat * 1e6).round(0.0001)
				~
				"\n# TAG = zone{$_}\n# R1 = "
				~
				$buf.split("ATTRIBUTE")[4].split('(0):')[1].words.head;

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
		return @data-files;
    }

    method !stelar-hdf5-R1 (Rat :$err) {
		my $stelar-hdf5 = self.filename();
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

	method !stelar-sdf-Mz (Bool :$Re, Bool :$Im) {
		my $stelar-sdf = self.filename();
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
			for ( 1 ..^ @zones.elems ).race {
				my $buf=@zones[$_];
				my $index=$buf.words.head.split('.').map({ sprintf('%03d',$_.Int) }).join('_');
				my $datafile = "zone{$index}.dat";
				my $T1MAX =	$buf.split(/T1MAX <ws> '=' <ws>/)[1].words.head.Rat * 1e-6;
		    	my $header = "# DATA dum = " ~
					($buf.split(/BR <ws> '=' <ws>/)[1].words.head.Rat * 1e6).round(0.0001)
					~
					"\n# TAG = zone{$index}";
	
				my @x;
				my @y;
				my @m;
				if $type eq "log" { @x = (0 ..^ $ntaus).map({ $taui * $T1MAX * ($tauf/$taui) ** ($_/($ntaus-1)) }) }
				else { @x = (0 ..^ $ntaus).map({ ($taui + ($tauf - $taui) * $_/($ntaus - 1) ) * $T1MAX }) }
				@x = @x.reverse if @range[1] < @range[0];

				@m = gather for $buf.lines { take $_.words[ $Re ?? 0 !! $Im ?? 1 !! 2 ] if $_.contains(/^'-'?\d+/) };
	
		    	for (1 .. $ntaus) { 
					@y.push: @m.splice(0,$BS.Int).sum/$BS;
			   	}
		    	@y = @y.map({ $_ / @y.max });
		    	my @err = (1 .. @x.elems).map({1});

		    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
		    	@data-files.push: $datafile;
			}
		}
		"$path/$stelar-sdf".IO.unlink;
		return @data-files;
    }

    method !stelar-sef-R1 (Rat :$err) {
		my $stelar-sdf = self.filename();
		my $path = self.path();
		$stelar-sdf.IO.copy: "$path/$stelar-sdf";
		my @R1 = gather for "$path/$stelar-sdf".IO.lines(:close) { take $_.words[0,2] if $_.contains(/^\d+/) } .map({ [ ($_[0] * 1e6).round(0.0001) ,$_[1]] }).Array;
		"$path/$stelar-sdf".IO.extension('dat').spurt:  (@R1 Z @R1.map({ $_[1].Rat * (($err.Bool) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
		return $stelar-sdf.IO.extension('dat').Str
    }

	method !ist-ffc () {
		my $ffc = self.filename();
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
			my $datafile = "zone{ sprintf('%03d',$_) }.dat";
			my $header = "# DATA dum = @modes[$_-1] @freqs[$_-1]\n# TAG = zone{ sprintf('%03d',$_) }";
			"$path/$datafile".IO.spurt: "$header\n" ~ @zone.join("\n") ~ "\n\n";
			@files.push: $datafile;
		}
		return @files
	}

	sub is-type ($file)  {
	   	return is-hdf5($file) ?? 'hdf5' !! is-zip($file) ?? 'zip' !! is-sdf($file) ?? "sdf" !! is-block($file) ?? 'blocks' !! is-ffc($file) ?? 'ffc' !! "";	
	}

	sub is-hdf5 ($file)  { return $file.IO.open(:bin).read(8,:close) eq Buf[uint8].new(0x89, 0x48, 0x44, 0x46, 0x0D, 0x0A, 0x1A, 0x0A) }
	sub is-zip($file)    { return $file.IO.open(:bin).read(4,:close) eq Buf[uint8].new(0x50, 0x4B, 0x03, 0x04) }
	sub is-block ($file) { return $file.IO.slurp(:close).contains(/'#' <ws> DATA <ws>/) }
	sub is-sdf ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/TAU/) }
	sub is-ffc ($file) 	 { return $file.IO.slurp(:enc('utf8'),:close).contains(/endtau/) }

}



