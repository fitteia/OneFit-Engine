unit module OneFit::Engine::Stpfiles;

class Stpfile is export {
    has @!stp;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    multi method update ($eng, Bool :$a, Str :$path) {
	$!path = $path if $path.defined;
	my %e=$eng.hash;
	my ($nums) = %e<Num Xmin Xmax>>>.split('\,')>>.join(' ');
	my $NT=$eng.blocks(:a).elems;
	my $i=$nums.words.elems;
	my @a = $nums.words;
	$nums ~=  " " ~ gather { for ($i ..^ $NT) { take @a[@a.elems-1] } } .join(' ');
	my $xscales=(0 ..^$NT).map({ $eng.blocks(:a)[$_].Graph.Xaxis.h<type>  }).join(' ').subst("Normal","lin",:g).subst("Logarithmic",'log',:g);
	my $parfiles=(1 .. $NT).map({ "fit{$_}.agr-par" }).join(' ');
	my $xms=(0 ..^$NT).map({ $eng.blocks(:a)[$_].Graph.Xaxis.h<min>  }).join(' ');
	my $xMs=(0 ..^$NT).map({ $eng.blocks(:a)[$_].Graph.Xaxis.h<max>  }).join(' ');
	my $gphs=(1 .. $NT).map({ "fit-curves-{$_}" }).join(' ');
	my $function = (0 ..^ $eng.functions.elems).map({ "userfitf$_\(" ~ $eng.functions[$_].parameters.join(',') ~ "\)" }).join('+');

	@!stp=();
        if %e<FitType> ~~ /Individual/ {
            for (1 .. $NT).race {
		@!stp.push: self!cstp(
		    $nums.words[$_-1],
		    $parfiles.words[$_-1],
		    $gphs.words[$_-1],
		    $xms.words[$_-1],
		    $xMs.words[$_-1],
		    $xscales.words[$_-1],
		    $function,
		    "fit$_.par",
		    "fit$_.out"
		);
		"$!path/fitenv{$_}.stp".IO.spurt: @!stp[$_-1];	    }
        }
        else {
	    @!stp.push: self!cstp($nums,$parfiles,$gphs,$xms,$xMs,$xscales,$function,"fit.par","fit.out");
	    "$!path/fitenv.stp".IO.spurt: @!stp[0];
	}
        ($a.Bool) ??  @!stp !! self;
    }

    method get () { @!stp }
    method a () { @!stp }
    method arr () { @!stp }
    
    method !cstp ($nums,$parfiles,$gphs,$xms,$xMs,$xscales,$function,$fitpar,$fitout) {
	my $stp = qq:to/EOT/;
--num={$nums}
--xmgr_par_file={$parfiles}
--xyt
--gph={$gphs}
--xm={$xms}
--xM={$xMs}
--xscale={$xscales}
--Function={$function}
--Parameter={$fitpar}
--PERL_file=gfitn.pl
-nf
-np
-nD
-o{$fitout}
--xmgrace
EOT
    }
}
