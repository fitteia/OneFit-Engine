unit module OneFit::Engine::Parfiles;

class Parfile is export {
    has $!table;
    has @!fit-methods = <simp scan min minos exit>;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    method write (@parameters, Bool :$fix-all, Bool :$fix-none, Bool :$s, :$No="", :$path) {
	$!path = $path if $path.defined;
	my $table = "0123456789/123456789/123456789/123456789/123456789/123456789\n";;
	$table ~= sprintf("%-10.1f%-10s%-2.1f\n",1.0,"n.tot.par",1+@parameters.elems);
	for (0 ..^@parameters.elems) {
	    my ($v,$m,$M)=@parameters[$_]<value min max>;
	    my $s = $v ?? abs(0.1*$v) !! 0.1;
	    my $f = {
		$^a ??
#		sprintf( { (abs($^b) > 1e6 or abs($^b) < 1e-2) ?? "%10.3e" !! "%10.3e" }($^a), $^a)
		sprintf( { (abs($^b) > 1e6 or abs($^b) < 1e-3) ?? "%9.2e" !! "%-9g" }($^a), $^a) 
		!! $^a
		}; 
	    $table ~= sprintf("%-10.1f%-10s%-10s%-10s%-10s%-10s\n",$_+2,@parameters[$_]<name>,$f($v),$f($s),$f($m),$f($M));
	}
	$table ~= (0 ..^ @parameters.elems).hyper.map({ sprintf("\n%-10s%-2.1f","fix",2+$_) if any($fix-all.Bool,!@parameters[$_]<free>.Bool) and $fix-none.not});
	$table ~=  "\nset       err       1.0\n";
	@!fit-methods = gather for @!fit-methods { take $_ unless .contains("minos") } if @parameters[@parameters.elems - 1]  eq "MIXED" and @parameters[@parameters.elems - 1]<value> == 1;
	$table ~=  @!fit-methods.join: "\n";
	"$!path/fit$No.par".IO.spurt: $table;
	$!table=$table;
	($s.Bool) ?? $!table !! self;
    }
    method get () { $!table }
}
