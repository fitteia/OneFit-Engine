unit module OneFit::Engine::Parameters;

#use OneFit::Engine::Output;
use OneFit::Engine::Grammars;
use OneFit::Engine::Parfiles;

class Parameters {
    has @!p;
    has $!np;
    has $!parfile;
    has $!path = '.';
    has %.output;

    method path ($folder) { $!path = $folder; self }
    
    method from-engine ($engine, Bool :$a) {
	my %e = $engine.get;
	my @pnames = %e{'Parameters'}.split(',');
	$!np = @pnames.elems;
	for (0 ..^ @pnames.elems) {
	    my %h = 
		'name' => @pnames[$_].subst(/\s+/,'',:g),
		'value'=> (%e{"Pval" ~ $_}) ?? %e{"Pval" ~ $_}.subst(/\s+/,'',:g) !! 1.0, # defaults to 1.0
		'min'  => %e{"Pmin" ~ $_}.subst(/\s+/,'',:g),
		'max'  => %e{"Pmax" ~ $_}.subst(/\s+/,'',:g),
		'err'  => "",
		'free' => %e{"F" ~ $_} ~~ "Free" ?? True !! False				       
	    ;
	    @!p[$_]=%h;
	}
#	say " Hey read parameters from-engine";
	($a.Bool) ?? @!p !! self;
    }
    method from-output (Str :$file, Bool :$a , Str :$path) {
	$!path = $path if $path.Bool;
	my $filename = ($file.defined) ?? $file !! "fit.out";
	my $m = OneFit::Engine::Grammars::Output.parse-made("$!path/$filename".IO.slurp).hash;
	%.output=$m;
	@!p.map( {if $_<name> {$_<value> = $m{$_<name>.subst(/\s+/,'')} if $m{$_<name>}.defined  } } );
	($a.Bool) ?? @!p !! self;
    }
    method from-log ( Str :$file, Bool :$a , Str :$path) {
	$!path = $path if $path.Bool;
	my $filename = ($file.Bool) ?? $file !! "fit.log";
	my $str = "$!path/$filename".IO.slurp( enc=>"latin1" ).split("EXT PARAMETER").tail.split("EXTERNAL").head.Str;
#	$str.say;
	my $e = OneFit::Engine::Grammars::Log.parse($str);
	my @par;
	for $e<line> {
	    my %h = 'no'   => $_<number>>>.Str[0],
		    'name' => $_<word>>>.Str[0],
		    'value'=> $_<number>>>.Str[1],
		    'err'  => $_<word>>>.Str[1]
	    ;
	    @par.push: %h;
	}
	for (0 ..^ $!np) { @!p[$_]<err>=@par[$_+1]<err> }
#	say @!p;
	($a.Bool) ?? @!p !! self;
    }
    
    method table () { @!p }
    method a () { @!p }

    method parfile () { $!parfile = Parfile.new.path($!path) }
}
