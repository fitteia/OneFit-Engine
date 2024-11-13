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
	    my $buf = shell("h5dump -d $_ $!stelar-hdf5",:out).out.slurp(:close);  
	    for $buf.lines {
		my @c = $_.words;
		@x.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(0\,\d+\)/);
		@Re_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(1\,\d+\)/);
		@Im_.push: @c[1 ..^ @c.elems]>>.subst(',','',:g).Slip if @c.head.contains(/\(2\,\d+\)/);
	    }
	    my $datafile = $_.subst('/','',:g).subst('t1_fit','.dat');
	    my $flarmor = '# DATA dum = ' ~ $buf.split("ATTRIBUTE")[1].split('(0):')[1].words.head.Rat * 1e6;
	    my $tag = '# TAG = ' ~ $_.split('/')[1];
	    my $R1 = '# R1 = ' ~ ~ $buf.split("ATTRIBUTE")[4].split('(0):')[1].words.head;
	    my $header = "$flarmor\n$tag\n$R1";
	    my $sqr =  { $^a.map({ $_ ** 2 }) };
	    my @module = ($sqr(@Re_) Z+ $sqr(@Im_))>>.sqrt;
	    $datafile.IO.spurt:  "$header\n" ~ (@x Z @module.map({ $_ / @module.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if !$Re and !$Im;
	    $datafile.IO.spurt:  "$header\n" ~ (@x Z @Re_.map({ $_ / @Re_.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if $Re;
	    $datafile.IO.spurt:  "$header\n" ~ (@x Z @Im_.map({ $_ / @Im_.max }) Z (1 .. @x.elems).map({1})).join("\n") ~ "\n\n" if $Im;
	}
	for (1 .. @zones.elems) { @data-files.push: 'zone' ~ $_ ~ '.dat' };
	return @data-files;
    }

    method R1 (Rat :$err) {
	
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
	$!stelar-hdf5.IO.extension('dat').spurt:  (@BR.map({ $_ * 1e6 }) Z @R1 Z @R1.map({ $_ * (($err.Bool) ?? $err !! 0.05) })).join("\n") ~ "\n\n";
	return $!stelar-hdf5.IO.extension('dat').Str;
    }
}
