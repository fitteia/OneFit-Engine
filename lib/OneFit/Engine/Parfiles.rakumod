unit module OneFit::Engine::Parfiles;

# A .par number in at most 9 characters, with as many significant digits as
# fit, up to 6 (what fit.out gives back for a fitted value). MINUIT reads a
# .par parameter line in fixed 10-column fields (mnpars.F: F10.0, A10,
# 4F10.0) and the C engine's ParFreeF (loadpar.c) counts the fields after
# the value by the blanks between them, so every number needs a blank after
# it. The old "%9.2e" kept only 3 digits below 1e-3 and above 1e6 - a
# resumed fit started visibly off its own minimum - and "%-9g" could run
# past the field (-0.00123456).
#
# Not sprintf: Rakudo's sprintf rounds the printed decimal half-up and does
# not renormalise ("10.0000e-05"). Like onefite-native's parfiles.rs
# format_par9 - byte-identical by construction - this takes the value's
# shortest round-trip decimal (Num.Str), rounds those digits half-up as a
# string and picks the spelling: plain decimal (with its leading "0" when
# that still fits) unless scientific with a compact exponent ("1.2855e-9")
# is shorter by more than 2.
our sub par9($x --> Str) {
    my $v = $x.Num;
    return ~$v if $v.isNaN || $v == Inf || $v == -Inf;
    return "0" if $v == 0;
    my $sign = $v < 0 ?? "-" !! "";
    # shortest decimal of |v| as digits d0 d1 ... and the exponent of d0
    my ($m, $x10) = $v.abs.Str.lc.split("e");
    $x10 = $x10.defined ?? +$x10 !! 0;
    my $point = $m.index(".") // $m.chars;
    my $all = $m.subst(".", "");
    my $lead = $all.chars - $all.subst(/^0+/, "").chars;
    my @digits = $all.substr($lead).comb;
    my $exp = $point - $lead - 1 + $x10;
    for 6...1 -> $p {
        my @d = @digits.head($p);
        my $e = $exp;
        if @digits > $p && @digits[$p] >= 5 {
            my $i = @d.elems;
            loop {
                if $i == 0 { @d.unshift(1); @d.pop; $e++; last }
                $i--;
                if @d[$i] == 9 { @d[$i] = 0 } else { @d[$i]++; last }
            }
        }
        @d.pop while @d > 1 && @d[*-1] == 0;
        my $ds = @d.join;
        my $s = $sign ~ @d[0] ~ (@d > 1 ?? "." ~ @d[1..*].join !! "") ~ "e" ~ $e;
        my $plain = $e < 0
            ?? "." ~ ("0" x (-$e - 1)) ~ $ds
            !! ($ds.chars <= $e + 1 ?? $ds ~ ("0" x ($e + 1 - $ds.chars)) !! $ds.substr(0, $e + 1) ~ "." ~ $ds.substr($e + 1));
        my $with-zero = $plain.starts-with(".") ?? "{$sign}0$plain" !! "$sign$plain";
        my $pl = $with-zero.chars <= 9 ?? $with-zero !! "$sign$plain";
        my $pick = ($pl.chars <= 9 && $pl.chars <= $s.chars + 2) ?? $pl !! $s;
        return $pick if $pick.chars <= 9;
    }
    die "par9: one significant digit always fits in 9 characters";
}

class Parfile is export {
    has $!table;
    has @!fit-methods = <simp scan min minos exit>;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    method write (@parameters, Bool :$fix-all, Bool :$fix-none, Bool :$s, :$No="", :$path, Str :$fit-methods) {
	$!path = $path if $path.defined;
	my $table = "0123456789/123456789/123456789/123456789/123456789/123456789\n";;
	$table ~= sprintf("%-10.1f%-10s%-2.1f\n",1.0,"n.tot.par",1+@parameters.elems);
	for (0 ..^@parameters.elems) {
	    my ($v,$m,$M)=@parameters[$_]<value min max>;
		#	    my $s = $v.so ?? abs(0.1*$v) !! 0.1;
		my $s = ($v.defined && +$v != 0) ?? abs(0.1*$v) !! 0.1;
	    my $f = {
		$^a ??
#		sprintf( { (abs($^b) > 1e6 or abs($^b) < 1e-2) ?? "%10.3e" !! "%10.3e" }($^a), $^a)
		par9($^a)
		!! $^a
		}; 
	    $table ~= sprintf("%-10.1f%-10s%-10s%-10s%-10s%-10s\n",$_+2,@parameters[$_]<name>,$f($v),$f($s),$f($m),$f($M));
	}
	$table ~= (0 ..^ @parameters.elems).hyper.map({ sprintf("\n%-10s%-2.1f","fix",2+$_) if any($fix-all.Bool,!@parameters[$_]<free>.Bool) and $fix-none.not});
	$table ~=  "\nset       err       1.0\n";
#	@!fit-methods = gather for @!fit-methods { take $_ unless .contains("minos") } if @parameters[@parameters.elems - 1]<name>  eq "MIXED" and @parameters[@parameters.elems - 1]<value> == 1;
	if $fit-methods.Bool {
	    @!fit-methods = gather for $fit-methods.words { take $_ unless .contains("exit") };
	    @!fit-methods.push("exit");
	}
	$table ~=  @!fit-methods.join: "\n";
	"$!path/fit$No.par".IO.spurt: $table;
	$!table=$table;
	($s.Bool) ?? $!table !! self;
    }
    method get () { $!table }
}
