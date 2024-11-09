unit module OneFit::Engine::Stelar;

class Stelar-hdf5 is export {
    has $!stelar-hdf5;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    multi method filename ($file) { $!stelar-hdf5 = $file; self }

    multi method filename () { $!stelar-hdf5 }
    
    method Mz (Bool :$Re, Bool :$Im) {
	my @zones = gather for shell("h5dump -n $!stelar-hdf5",:out).out.slurp(:close).lines { take $_.words.tail if $_.contains(/t1_fit/) }
	my @data-files;
	for @zones {
	    my @x;
	    my @Re_;
	    my @Im_;
	    for shell("h5dump -d $_ $!stelar-hdf5",:out).out.lines(:close) {
		my @c = $_.words;
		@x.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(0\,\d+\)/);
		@Re_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(1\,\d+\)/);
		@Im_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(2\,\d+\)/);
	    }
	    my $datafile = $_.subst('/','',:g).subst('t1_fit','.dat');
	    my $sqr =  { $^a.map({ $_ ** 2 }) };
	    my @module = ($sqr(@Re_) Z+ $sqr(@Im_))>>.sqrt;
	    $datafile.IO.spurt:  (@x Z @module.map({ $_ / @module.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if !$Re and !$Im;
	    $datafile.IO.spurt:  (@x Z @Re_.map({ $_ / @Re_.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if $Re;
	    $datafile.IO.spurt:  (@x Z @Im_.map({ $_ / @Im_.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if $Im;
	}
	for (1 .. @zones.elems) { @data-files.push: 'zone' ~ $_ ~ '.dat' };
	return @data-files;
    }

    method R1 (:$err) {
	
	my @zones = gather for shell("h5dump -n $!stelar-hdf5",:out).out.slurp(:close).lines { take $_.words.tail if $_.contains(/t1_fit/) }
	my @BR;
	my @R1;
	for @zones {
	    for shell("h5dump -d $_ $!stelar-hdf5",:out).out.slurp(:close) {
		my @c = $_.split: "ATTRIBUTE";
		@c = gather for @c { take $_  if $_.contains(/'"BR"'|'"R1"'/) }
		@BR.push: @c[0].split('(0):')[1].words.head;
		@R1.push: @c[1].split('(0):')[1].words.head;
	    }
	}
	say $!stelar-hdf5;
	$!stelar-hdf5.IO.extension('dat').spurt:  (@BR Z @R1 Z @BR.map({ $_ * (($err) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
	return $!stelar-hdf5.IO.extension: 'dat';
    }
}
