unit module OneFit::Engine::Stelar;

role Stelar {
    has $!stelar;
    has $!path = '.';

    multi method path ($folder) { 
		$!path = $folder;
		$folder.IO.mkdir unless $folder.IO.e;
		self 
	}

    multi method path () { $!path }

    multi method filename ($file) { $!stelar = $file; self }

    multi method filename () { $!stelar }
    
}

class Stelar-hdf5 does Stelar is export {
    
	method Mz (Bool :$Re, Bool :$Im) {
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
	    	my $datafile = "zone{$_}.dat";
	    	my $header = "# DATA dum = " ~
				$buf.split("ATTRIBUTE")[1].split('(0):')[1].words.head.Rat * 1e6
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

    method R1 (Rat :$err) {
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
		"$path/$stelar-hdf5".IO.extension('dat').spurt:  (@BR.map({ $_ * 1e6 }) Z @R1 Z @R1.map({ $_ * (($err.Bool) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
		return $stelar-hdf5.IO.extension('dat').Str;
    }

}

class Stelar-sdf does Stelar is export {
	method Mz (Bool :$Re, Bool :$Im) {
		my $stelar-sdf = self.filename();
		my $path = self.path();
		$stelar-sdf.IO.copy: "$path/$stelar-sdf";
	    my $buf = $stelar-sdf.IO.slurp(:close);
		my @zones = $buf.split(/ZONE/);
		my $BS = @zones[0].split(/BS <ws> '=' <ws>/)[1].words.head.Rat;
		my @aux = @zones[0].split(/TAU <ws> '=' <ws>/)[1].words.head.trans([ "[", "]" ] => "").split(':');
		my $type = @aux.shift;
		my $tauf = @aux.shift.subst('*T1MAX','').Rat;
		my $taui = @aux.shift.subst('*T1MAX','').Rat;
		my $ntaus = @aux.tail;
		my @data-files;
		for ( 1 ..^ @zones.elems ).race {
			my $buf=@zones[$_];
			my $index=$buf.words.head.subst('.','_');
			my $datafile = "zone{$index}.dat";
			my $T1MAX =	$buf.split(/T1MAX <ws> '=' <ws>/)[1].words.head.Rat * 1e-6;
	    	my $header = "# DATA dum = " ~
				$buf.split(/BR <ws> '=' <ws>/)[1].words.head.Rat * 1e6
				~
				"\n# TAG = zone{$index}";

			my @x;
			my @y;
			my @m;
			if $type eq "log" { @x = (0 ..^ $ntaus).map({ $taui * $T1MAX * ($tauf/$taui) ** ($_/($ntaus-1)) }) }
			else { @x = (0 ..^ $ntaus).map({ ($taui + ($tauf - $taui) * $_/($ntaus - 1) ) * $T1MAX }) }

			@m = gather for $buf.lines { take $_.words[ $Re ?? 0 !! $Im ?? 1 !! 2 ] if $_.contains(/^'-'?\d+/) };

	    	for (1 .. $ntaus) { 
				@y.push: @m.splice(0,$BS.Int).sum/$BS;
		   	}
	    	@y = @y.map({ $_ / @y.max });
	    	my @err = (1 .. @x.elems).map({1});

	    	"$path/$datafile".IO.spurt:  "$header\n" ~ (@x Z @y Z @err).join("\n") ~ "\n\n";
	    	@data-files.push: $datafile;
		}
		"$path/$stelar-sdf".IO.unlink;
		return @data-files;
    }

    method R1 (Rat :$err) {
		my $stelar-sdf = self.filename();
		my $path = self.path();
		$stelar-sdf.IO.copy: "$path/$stelar-sdf";
		my @R1 = gather for "$path/$stelar-sdf".IO.lines(:close) { take $_.words[0,2] if $_.contains(/^\d+/) } .map({ [+$_[0] * 1e6,$_[1]] }).Array;
		"$path/$stelar-sdf".IO.extension('dat').spurt:  (@R1 Z @R1.map({ $_[1] * (($err.Bool) ?? $err !! 0.05)  })).join("\n") ~ "\n\n";
		return $stelar-sdf.IO.extension('dat').Str;
    }
}
