unit module OneFit::Engine::Functions;

use OneFit::Engine::CodeC;

class Function is export {
    has $!formula;
    has @!parameters=();
    has @!dif-eqs;
    has $!solve-to = 0;
    has $!dif-h;
    has $!ODEidx;
    has Bool $!IS-NODE1;
    has Str $.label is rw = "";
    has $.posx is rw = 0;
    has $.posy is rw = 0;
    has $.color is rw = 1;
    has $.line is rw = 1;
    has $.No;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    method decode ($function, %e) {
	$!formula = $function;
	$.label = %e{"Func" ~ $!No-1} if %e{"Func" ~  $!No-1}.defined;
	$.posx = %e{"Funcx" ~ $!No-1} if %e{"Funcx" ~ $!No-1}.defined;
	$.posy = %e{"Funcy" ~ $!No-1} if %e{"Funcy" ~ $!No-1}.defined;
	$.color = %e{"Cor" ~ $!No-1} if %e{"Cor" ~ $!No-1}.defined;
	$.line = %e{"Traco" ~ $!No-1} if %e{"Traco" ~ $!No-1}.defined;

	my @p = split(/\s*','\s*/,%e<Parameters>.subst(/\s/,'',:g));
	for @p {
	    @!parameters.push($_) if $!formula.contains(/ <|w> $_ <?wb> /);
	}
	say %e<T>;
	$!formula.subst(%e<T>,"{%e<T>}_1",:g);
	say $!formula;
	$!formula.subst(/\n/,"").match:
	/<-["]>+
	 [
	     <ws> ','? <ws>
	     <["]> $<captures>=(<-["]>+) <["]>
	     <ws> ','? <ws>
	 ]+
	 ','? <ws>
	 $<no>=(<-["]>+)
	 ')'$
	 /;
	@!dif-eqs=$<captures>.Array>>.Str if $<captures>.defined;
	$!solve-to = $<no>.Str if $<no>.defined;
	$!IS-NODE1 = ($!formula.contains(/^NODE1/)) ?? True !! False;
	if $!IS-NODE1 {
	    $!dif-h = $!formula.split(',')[@!dif-eqs.elems+2];
	    $!ODEidx=($!dif-h eq @!parameters[@!dif-eqs.elems+1]) ?? 
								  @!dif-eqs.elems+2 !! @!dif-eqs.elems+1;

	}
	self
    }
    method parameters () { @!parameters }
    method formula () { $!formula }
    method No ($n) { $!No = $n; self }
    
    method write-c (:$path, :$name, :$x,:$y, :$t, :$AuxDeclar) {
	$!path = $path if $path.defined;
	my $template = template.new;
	my $TXT = $template.head;
	my %Eq-names = (1 .. @!dif-eqs.elems).map({ @!parameters[$_].subst('0','') => $_ - 1 }) if $!IS-NODE1;

	if $!IS-NODE1 {
	    $TXT ~= $template.struct.subst("NODE1",@!dif-eqs.elems,:g);

	    my $arr = { (0 ..^ $^n).map( {$_ * 0 } ).join(',') }; # creates "0,0,0,..$^n
	    my $mat = "\{\n\{" ~ (1 .. @!dif-eqs.elems).map( { $arr(4) } ).join("\},\n\{") ~ "\}\n}";
	    # creates {{0,0,..$^n},{...}}
	    $TXT ~= "static double _df_[][4]=$mat;\n\n";
	    
	    $mat = "\{\n\{" ~ (1..@!dif-eqs.elems).map( { $arr(2) } ).join("\},\n\{") ~ "\}\n}";
	    $TXT ~= "static double _x_[]=\{0,0\};\n";
	    $TXT ~= "static double _y_[][2]=$mat;\n\n";
	}
	
	$TXT ~= "\n/**\n    XGFITN CONTROL START\n";
	$TXT ~= "     NAME=$name\n";
	$TXT ~= "     XY= $x $t\n";
	$TXT ~= (0 ..^ @!parameters.elems).map({ "     PAR="~ @!parameters[$_] ~ " 1 2 3 4\n" }).join('');
	$TXT ~= "     DESCRIPTION=$name\n";
	$TXT ~= "\n   XGFITN CONTROL END\n**/\n\n/** XGFITN HEADER START **/";
	$TXT ~= "\nstatic int $name" ~ "_first_time=YES;\n";
	$TXT ~= (0 ..^ @!parameters).map({ "static int $name" ~ "_arg_" ~ @!parameters[$_] ~ "=-1;\n" }).join('');
	$TXT ~= "\ndouble $name\(double $x, double \*par, double $t, int mode, char \*\*par_names, int nnames\)\n\{\n   double { ["af"].push(@!parameters.Slip).join(',')};\n";
	$TXT ~= "   double $AuxDeclar;\n" if ($AuxDeclar);
	
	if $!IS-NODE1 {
	    $TXT ~= "   int i;\n";
	    $TXT ~= "   double RuKu4thNODE1()," ~ (1..@!dif-eqs.elems).map({ "_dy" ~ $_ ~"dx__\(\)" }).join(',') ~ ";\n";
	    $TXT ~= "   ODEn oden;\n" ;
	}

	$TXT ~= "\n   if(par_names != NULL)\{\n      if(nnames<{@!parameters.elems}) gfitn_error(\"$name: \",\" needs {@!parameters.elems} parameters as arguments\");\n ";
	$TXT ~= (0..^ @!parameters.elems).map({ "     GETPAR\($name" ~ "_arg_@!parameters[$_],par_names\[$_\]\);\n"  });
	$TXT ~= "      $name" ~ "_first_time=NO;\n    \}\n ";
	$TXT ~= "   else \{\n ";
	$TXT ~= (0..^ @!parameters.elems).map({ "     $name" ~ "_arg_@!parameters[$_] = {$_+2};\n" });
	$TXT ~= "    }\n ";
	$TXT ~= (0 ..^ @!parameters.elems).map({ "   @!parameters[$_] = par\[$name" ~ "_arg_@!parameters[$_]\];\n"; }); 

	if $!IS-NODE1 {
	    $TXT ~= "    oden.t=$t;\n";
            $TXT ~= "    oden.n={@!dif-eqs.elems};\n";
	    $TXT ~= "    oden.x0=@!parameters[0];\n ";
	    $TXT ~= (0 ..^ @!dif-eqs.elems).map({ "   oden.dy\[$_\]=\&_dy{$_+1}dx__;\n    oden.y0\[$_\] = @!parameters[$_+1];\n" });
            $TXT ~= "    oden.h = $!dif-h;\n ";
	    $TXT ~= ($!ODEidx ..^ @!parameters.elems).map({ "   oden.par\[{ $_ - $!ODEidx }\].val = @!parameters[$_];\n"; });
	}
	
	$TXT ~= "/** XGFITN HEADER END **/\n    \{\n/** XGFITN MAIN CODE START **/\n";
	if $!IS-NODE1 {
	    $TXT ~= "    af=RuKu4thNODE1($x,&oden);\n";
            $TXT ~= "    i = { $!solve-to ?? "\(int\) $!solve-to" !! 1 };\n";
	    $TXT ~= "    af=\(_y_\[i-1\]\[1\]-_y_\[i-1\]\[0\]\)/\(_x_\[1\]-_x_\[0\])\*\($x - _x_\[0\]\)+_y_\[i-1\]\[0\];\n";
	}
	else {
	    $TXT ~= "    af=$!formula;\n";
	}
	$TXT ~= "    return af;\n";
	$TXT ~= "\n/** XGFITN MAIN CODE END **/\n  \}\n}\n/** XGFITN ADITIONAL CODE START **/\n";
	if $!IS-NODE1 { 
	    my @all-par-names = gather { for %Eq-names.sort(*.value).map(*.kv) -> ($k,$v) { take $k } };
	    @all-par-names.push: @!parameters.Slip;
	    for (0 ..^ @!dif-eqs.elems) {
	    	my @dif-eq-pars = gather { for @all-par-names -> $name { take $name if @!dif-eqs[$_].contains(/<|w> $name <|w>/) } };
		$TXT ~= "double _dy{$_+1}dx__\(ODEn *oden\)\n\{\n   double ";
		$TXT ~= "$t," if @!dif-eqs[$_].contains(/<|w> $t '_'? \d* <|w>/);
		$TXT ~= "@!parameters[0].subst('0','')," if @!dif-eqs[$_].contains(/<|w> $x <|w>/);
		$TXT ~= "{ @dif-eq-pars.join(',') };\n ";
		$TXT ~= "  double $AuxDeclar;\n" if ($AuxDeclar);
		$TXT ~= "{ gather { for ($!ODEidx ..^ @!parameters.elems) -> $i { 
		     my $par = @!parameters[$i]; 
		     take "\n   @!parameters[$i] = oden->par\[{ $i - $!ODEidx }\].val;" if @!dif-eqs[$_].contains(/<|w> $par <|w>/);
		     } } }";	
		my $h = $!dif-h;
		$TXT ~= "\n   $!dif-h = oden->h;" if @!dif-eqs[$_].contains(/<|w> $h <|w>/) and !$h.contains($t);
		$TXT ~= "\n   $t = oden->t;" if @!dif-eqs[$_].contains(/<|w> $t '_'? \d*/) ;
		$TXT ~= "\n   $x = oden->x;" if @!dif-eqs[$_].contains(/<|w> $x '_'? \d*/) ;
		for %Eq-names.sort(*.value).map(*.kv) -> ($k,$v) { $TXT ~= "\n   $k = oden->y\[$v\];" if @!dif-eqs[$_].contains(/<|w> $k <|w>/) };
		$TXT ~= "\n   return @!dif-eqs[$_];";
		$TXT ~= "\n\}\n\n";
	    }
	    $TXT ~= "\n" ~ $template.RungeKutta.subst("SOLVE-TO",%Eq-names{$y},:g);
	}
	$TXT ~= "\n/** XGFITN ADITIONAL CODE END **/\n";
	"$!path/$name.c".IO.spurt: $TXT;
	#say $TXT;
	self
    }

}


			
