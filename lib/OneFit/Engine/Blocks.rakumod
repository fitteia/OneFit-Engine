unit module OneFit::Engine::Blocks;

use OneFit::Engine::Graphs;
use OneFit::Engine::Titles;
use OneFit::Engine::Axes;
use OneFit::Engine::Agrs;


class Block is export {
    has $!No=0;
    has $!T;
    has $!Tag;
    has @!Data;
    has $!Graph;
    has @!Export-data;
    has $.parameters is rw;
    has @!X;
    has @!Y;
    has @!E;
    has $!pstep=1;
    has $!pcond=1; # must return .Bool = True by default
    has $!fstep=1;
    has $!fcond=1; # must return .Bool = True by default
    has $!path = '.';
    has $.chi2 is rw;
    
    method path ($folder) { $!path = $folder; self }
    
    method read (Str $txt,
		 Bool :$fit,
		 Bool :$plot,
		 Bool :q(:$quiet),
		 :xg(:$xgrid),
		 :yg(:$ygrid),
		 :eb(:$errorbars),
		 :ssz(:$symbolsize)
		){
	$!Graph = Graph.new.path($!path);
	$!Graph.Curves.push: %( label      => "",
				posx       => 0,
				posy       => 0,
				color      => 1,
				linetype   => 0,
				symbolsize => ($symbolsize.defined) ?? $symbolsize !! 1,
				errorbars  => ($errorbars.defined) ?? $errorbars !! "off",
				No         => 0
			     );

	for $txt.lines -> $line {
	    if $line.contains("DATA")  {
		$!T = $line.split("=")[1].subst(/^\s*/,"");
	    }
	    elsif $line.contains("TAG") {
		$!Tag = $line.split("=")[1].subst(/^\s*/,"");
	    }
	    elsif $line.contains("TITLE") {
		$!Graph.Title.read($line);
		<title posx posy>.map({ $!Graph.Curves[0]{$_}=$!Graph.Title.h{$_} });

	    }
	    elsif $line.contains("XAXIS") {
		$!Graph.Xaxis.read($line).auto(False);
		$!Graph.Xaxis.grid: set => ($xgrid.defined) ?? "on" !! "off" 
	    }
	    elsif $line.contains("YAXIS") {
		$!Graph.Yaxis.read($line).auto(False);
		$!Graph.Xaxis.grid: set => ($ygrid.defined) ?? "on" !! "off" 
	    }
	    elsif $line.contains(/fit <ws> if/) {
		$line ~~ /'#' <ws> fit <ws> if $<c>=(<-[,]>+) <ws> ',' <ws> $<s>=(\d+)/;
		$!fstep = $<s>;
		$!fcond = $<c>;
		$!fcond ~~ s:g/c(\d+)/ \(\$c->\{$0\}\) /;
	    }
	    elsif $line.contains(/plot <ws> if/) {
		$line ~~ /'#' <ws> plot <ws> if $<c>=(<-[,]>+) <ws> ',' <ws> $<s>=(\d+)/;
		$!pstep = $<s>;
		$!pcond = $<c>;
		$!pcond ~~ s:g/c(\d+)/ \(\$c->\{$0\}\) /;
	    }
	    else {
		if $line {
		    if $line ~~ /^'#'/ or $line.words.elems < 2 {
			say "ignoring non-conform line: $line" unless $quiet
		    }
		    else { @!Data.push: $line }

		}
	    }
	}
	@!Data = @!Data.map({ .words } ).sort({ +$^a[0] <=> +$^b[0] }).map({ .join(" ") });
#	@!Data>>.say;
	self.select(:fit($fit),:plot($plot));
	self.XYE;
	self
    }
    multi method No () { $!No }
    multi method No ($no) { $!No=$no; self }
    method Tag () { $!Tag }
    method Data () { @!Data }
    method Export-data () { @!Export-data }
    method T () { $!T }
    method export (:$path, Bool :$fit, Bool :$plot) {
	$!path = $path if $path.defined;
	self.select(:fit($fit), :plot($plot));
	{ "$!path/data" ~ $!No+1 ~ ".dat" }().IO.spurt: {($!T.words.elems>1) ?? $!No+1 !! $!T.words[0]}() ~ "\n" ~ @!Export-data.join("\n") ~ "\n";
#	{ "$!path/data" ~ $!No+1 ~ ".dat" }().IO.spurt: $!No+1 ~ "\n" ~ @!Export-data.join("\n") ~ "\n";
	self
    }
    method Graph () { $!Graph }

    method select (Bool :$fit, Bool :$plot) {
	use Inline::Perl5;
	my $p5 = Inline::Perl5.new;
	$p5.run(q:to/EOP/);
	sub myeval {
	    my ($c,$out)=@_;
	    eval($out)
	}	    
	EOP

    	if ($fit and $!fcond.Bool) or ($plot and $!pcond.Bool) {
	    my @selection = gather {
		my $i=0;
		my %c;
#		say self.No;
		repeat while $i < @!Data.elems {
		    %c<1 2 3> = @!Data[$i].words;
		    my $condition = $fit ??  $!fcond !! $!pcond;
		    take $i if  $p5.call('myeval',%c,$condition).Bool;
		    $i += ($fit and $!fcond.Bool) ?? $!fstep !! $!pstep;
		}
	    }
	    @!Export-data = @!Data[@selection];
	}
	else { @!Export-data = @!Data }
	self
    }
    
    method XYE () {
	# assuming:
	# x y ey ex
	# or
	# x y ey
	@!X=();
	@!Y=();
	@!E=();
	my $ex=0;
	my @a;
	for @!Data.hyper {
	    @a.push: .words
	}
#	say @a.join("\n");
#	@a= @a.sort: { +$^a[2] <=> +$^b[2] };
#	say @a.join("\n");
#	@a=@a.sort: { +$^a[1] <=> +$^b[1] };
#	say @a.join("\n");
	for @!Data.hyper {
	    my @arr = $_.words;
	    @!X.push: +@arr[0];   # the + is to make sure X will get numnbers
	    @!Y.push: +@arr[1];   # the + is to make sure Y will get numnbers
	    if @arr[3].defined { $ex = @arr[3]; @!E.push: @arr[2..3].join(" ") }
	    else {
		@arr[2].defined ?? @!E.push: "@arr[2] 0.0" !! @!E.push: "1.0 0.0"
	    }
	}
	self.E(:ex-to-ey) if ($ex>=0 or any(@!E>>.contains('%')));
	self
    }
    method X () { @!X }
    method Y () { @!Y }
    
    method E (:$ex, :$ey, Bool :$ex-to-ey) {
	if none($ex,$ey,$ex-to-ey) { return @!E }
	my @Ex;
	for (0..^ @!E.elems).hyper {
	    my @a = @!E[$_].split(" ");
	    @a.push(0.0) unless @a[1].defined; 
	    @Ex.push: { $ex.defined ?? +$ex !! +@a[1]}() ;
	    @!E[$_]= +@a[0];
	}	
	if $ey.defined {
	    if $ey.contains("%") {
		for (0..^ @!E.elems) { @!E[$_] = +$ey.subst("%","")*@!Y[$_]/100 }
	    }
	    else {
		@!E = (@!E <<*>> 0) <<+>> $ey
	    }
	}
	if (@!X.elems > 4 and any(@Ex) > 0) {
	    my @Ee;
	    my @h;
	    for 0 ..^ @!X.elems -> $i {
		if $i < 2 {
		    @h = (@!X[$i+3]-@!X[$i+2],@!X[$i+2]-@!X[$i+1],@!X[$i+1]-@!X[$i]);
		    @Ee[$i] = abs(2.0/6.0*(@!Y[$i+3]-@!Y[$i+2])/@h[0]
				  -7.0/6.0*(@!Y[$i+2]-@!Y[$i+1])/@h[1]
				  +11.0/6.0*(@!Y[$i+1]-@!Y[$i])/@h[2]);
		}
		elsif $i > @!X.elems-3 {
		    @h = (@!X[$i-2]-@!X[$i-3],@!X[$i-1]-@!X[$i-2],@!X[$i]-@!X[$i-1]);
		    @Ee[$i] = abs(  -2.0/6.0*(@!Y[$i-3]-@!Y[$i-2])/@h[0]
				    +7.0/6.0*(@!Y[$i-2]-@!Y[$i-1])/@h[1]
				    -11.0/6.0*(@!Y[$i-1]-@!Y[$i])/@h[2]);
		}
		else {
		    @h = (@!X[$i+2]-@!X[$i-2],@!X[$i+1]-@!X[$i-1]);
		    @Ee[$i] = abs(  -1.0/3.0*(@!Y[$i+2]-@!Y[$i-2])/@h[0]
				    +4.0/3.0*(@!Y[$i+1]-@!Y[$i-1])/@h[1]);
		}
	    }
	    @!E = @!E <<+>> (@Ee <<*>> @Ex); 
	}
	for (0..^@!Data.elems).hyper -> $b {
#	    for (0 ..^ @!X.elems) -> $i { @!Data[$b] = "@!X[$i] @!Y[$i] @!E[$i]" }
	    @!Data[$b]=([Z] @!X,@!Y,@!E)[$b;*].join: " "
	}
#	@!Data>>.say;
	self;
    }
    method write-agr (:$path) {
	$!path = $path if $path.defined;
	my $agr = OneFit::Engine::Agrs::template.new;
	$!Graph.rminf.autoposxy($agr);
	my $TXT = $agr.head(
	    $!Graph.Title.h<title>,
	    $!Graph.Title.h<posx>,
	    $!Graph.Title.h<posy>,
	    $!Tag,
	    $!Graph.Curves[0]<symbolsize>
	);

	for $!Graph.Curves -> %label {
	    $TXT ~= $agr.string(%label<label>,%label<color>,%label<posx>,%label<posy>);
	}
	$TXT ~= $agr.setgraph($!Graph.Xaxis,$!Graph.Yaxis);
	$TXT ~= $agr.dataset(0,
			     :type<xydy>,
			     :symbol,
			     symbolsize => $!Graph.Curves[0]<symbolsize>,
			     errorbars => $!Graph.Curves[0]<errorbars>
			    );
	for (1 ..^ $!Graph.Curves.elems)  {
		    $TXT ~= $agr.dataset($_,
					 :l,
					 lt => $!Graph.Curves[$_]<linetype>,
					 c  => $!Graph.Curves[$_]<color>
					);
	}
	$TXT ~= $agr.dataset($!Graph.Curves.elems,:l);
	    
	{ "$!path/fit" ~ $!No+1 ~ ".agr-par" }().IO.spurt: $TXT;
	self
    }
}
