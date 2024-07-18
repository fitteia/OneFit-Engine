unit module OneFit;

use OneFit::Engine::Parameters;
use OneFit::Engine::Axes;
use OneFit::Engine::Titles;
use OneFit::Engine::Blocks;
use OneFit::Engine::Functions;
use OneFit::Engine::CodeC;
use OneFit::Engine::Stpfiles;

class Engine is export {
    has %!engine;
    has @!blocks;
    has @!par-tables;
    has @!Functions;
    has $!path = '.';
    
    use JSON::Fast;

    method resources () { %?RESOURCES }
    
    method path ($folder) { $!path = $folder; self }
    
    method read ($file, Bool :$h) {
	die "OFE input file reading error: $file doesn't exist" unless "$file".IO.e;
	if $file.contains('json') {
	    %!engine = from-json($file.IO.slurp) ;
	}
	else {
	    use Inline::Perl5;
	    use CGI:from<Perl5>;

	    my $sav = CGI.new( $file.IO.open );
	    for $sav.param { %!engine{$_} = $sav.param($_) }	 
	}
	%!engine<FitType> = "Global" unless %!engine<FitType>;
	($h.Bool) ?? %!engine !! self;
    }
    method save ($file) {
	my %e=%!engine;
	%e<par-tables> = @!par-tables>>.table;
	if $file.contains('json') {
	    spurt( $file, to-json(%e, :sorted-keys) );
	}
	else {
	    use Inline::Perl5;
	    use CGI:from<Perl5>;

	    my $sav = CGI.new;
	    for %!engine.keys -> $key { $sav.param($key,%e{$key})  }	 

	    unless my $fh = open $file.subst("json","sav"), :w { say "dammed!"; };
	    $sav.save($fh);
	    $fh.close;
	}
	self
    }

    multi method h () { %!engine }
    multi method h (%e) { %!engine=%e; self }
    multi method hash () { %!engine }
    multi method hash (%e) { %!engine=%e; self }
    multi method get () { %!engine }
    multi method set (%e) { %!engine=%e; self }
    multi method Num ($npts) { %!engine<Num>=$npts }
    
    method initialize () {
	self.blocks(:r);
	self.functions(:r);
	self.code(:w);
	self.code(:c);
	self.parameters(:r);
#	self!write-ndata-h();
	self
    }
    
    multi method data () { %!engine<Dados> }
    multi method data ($dados) { %!engine<Dados>=$dados; self }
    
    method blocks (Str :f(:from($data)),
		   Bool :q(:$quiet),
		   Bool :a(:arr($array)),
		   Bool :r(:read($r)),
		   Bool  :e(:export($export)),
		   Bool :$fit, Bool :$plot,
		   Bool :$All,
		   Bool :v(:verbose($verbose)),
		   Bool :$logx = False,
		   Bool :$logy = False,
		   Bool :$autox = False,
		   Bool :$autoy = False,
		  ) {
	if $r.Bool {
	    my @arr = ($data.Bool) ??  $data.split( /'#' <ws> DATA <ws>/) !! %!engine<Dados>.split( /'#' <ws> DATA <ws>/);
	    @!blocks = gather {
		my $i=0;
		for @arr[1 ..^ @arr.elems].hyper {
		    $_ ~~ /TAG <ws> \= <ws> $<tag>=(<-[\n]>+)\n/;
		    if $All {
			take Block.new.read('# DATA ' ~ $_,:quiet($quiet)).No($i++).path($!path);
		    }
		    else {
			if $<tag>.Str eq (%!engine<SelectAll> or any %!engine<Tags>.Slip) {
			    if $fit.defined {
				take Block.new.No($i++).read('# DATA ' ~ $_, :fit, :quiet($quiet)).path($!path);
			    }
			    if $plot.defined {
				take Block.new.No($i++).read('# DATA ' ~ $_, :plot, :quiet($quiet)).path($!path);
			    }
			    if none($fit,$plot) {
				take Block.new.No($i++).read('# DATA ' ~ $_, :quiet($quiet)).path($!path);
			    }
			}
			else {
			    if $verbose.Bool {
				say $<tag>.Str, " not selected in list ", %!engine<Tags>.Slip.join(" ")
			    }
			}
		    }
		}
	    }
	    for <Xmin Xmax Ymin Ymax> {
		%!engine{$_} = { my @a= $^a.split(',');
			   @a.push( {@a.tail xx @!blocks.elems-@a.elems}().Slip ).join('\,')
			 }(%!engine{$_})
	    }
	    for @!blocks {
		.Graph.path: $!path;
		.Graph.Xaxis.type("Logarithmic") if $logx;
		.Graph.Yaxis.type("Logarithmic") if $logy;
		.Graph.Xaxis.scale( min => %!engine<Xmin>.split(/<[\\]>+ <[,]>/)[.No], max => %!engine<Xmax>.split(/<[\\]>+ <[,]>/)[.No], nt => 5 ) if all(%!engine<Xmin Xmax>)>>.defined and .Graph.Xaxis.auto;
		.Graph.Yaxis.scale( :min(%!engine<Ymin>.split(/<[\\]>+ <[,]>/)[.No]), :max(%!engine<Ymax>.split(/<[\\]>+ <[,]>/)[.No]), :nt(5) ) if all(%!engine<Ymin Ymax>)>>.defined and .Graph.Yaxis.auto;
		.Graph.Xaxis.scale( min => .X.min, max => .X.max, nt => 5 , auto => True) if $autox;
		.Graph.Yaxis.scale( :min(.Y.min), :max(.Y.max), :nt(5), auto => True) if $autoy;
	    }
	} 
	if $export.Bool { @!blocks.race.map( { .export(path => $!path, :fit($fit),:plot($plot)) }) }
	if $array.Bool  { return @!blocks }
	else { return self }
	
    }
    method a () { @!blocks }
    method arr () { @!blocks }
    
    method functions (Bool :read(:$r), Str :e(:$equation), Bool :and-rtna(:$rtna)){
	%!engine<Function>=$equation if $equation;
	if $r.Bool {
	    my $formula = %!engine<Function>;
	    my $Y = %!engine<Y>;
	    $formula ~~ s/$Y\s*'='\s*//;
	    my @fstrings=$formula.split('\n').head.split(';').head.split('\+');
	    my @colors=<0-white 1-black 2-red 3-green 4-blue 5-yellow 6-brown 7-grey 8-violet 9-cyan>;
	    my @traces=['0-none','1-solid','2-dot','3-dashed','4-long dashes','5-dot dashed','6-dot long dashes','7-dot dot dashed','8-dot dashed dashed'];
	    for (0 ..^ @fstrings.elems) -> $i {
		%!engine{'Cor' ~ $i}   = @colors[( $i mod 8 ) + 2];
		%!engine{'Traco' ~ $i} = @traces[( ($i+1) mod 7 ) + 2];
		my $fillarr = {
		    my @a= $^a.split('\,');
		    @a.push( {@a.tail xx @!blocks.elems-@a.elems}().Slip ).join('\,')
		};
		{<Func Funcx Funcy> <<~>> $i}().map: {
		    if %!engine{$_}.defined { %!engine{$_} = $fillarr(%!engine{$_}) }
		    else { %!engine{$_} = $fillarr( ($_ ~~ /Func\d+/) ?? "" !! "auto") }
		};
#		{<Cor Traco> <<~>> $i}().map: { %!engine{$_} = $fillarr(%!engine{$_}) };

		my $function = @fstrings[$i];
		@!Functions[$i] = Function.new.No(1+$i);
		@!Functions[$i].decode($function,%!engine);
		for @!blocks {
		    .Graph.gph = "fit-curves-{.No+1}";
		    .Graph.Curves.push: %(label      => @!Functions[$i].label.Str.split('\,')[.No],
					  posx       => @!Functions[$i].posx.Str.split('\,')[.No],
					  posy       => @!Functions[$i].posy.Str.split('\,')[.No],
					  color      => @!Functions[$i].color.Str.subst(/\D/,'',:g),
					  linetype   => @!Functions[$i].line.Str.subst(/\D/,'',:g),
					  symbolsize => 1,
 					  errorbars  => "off",
					  No         => $i+1;
					 );
		}
	    }
	    if $rtna { return @!Functions }
	    else { return self }
	}
	else { return @!Functions }
    }

    method code (Bool :w(:write($w)), Bool :c(:compile($c)), Bool :$quiet) {
	my $code = Code.new.path($!path);
	$code.cwrite(%!engine,@!blocks,@!Functions) if $w;
	$code.compile(:quiet($quiet)) if $c;
	%!engine<compile-log> = "compile.log".IO.slurp;
	self
    }
    
    
    method par-selected () {
	my @a;
	if %!engine<FitType> ~~ /Individual/ {
	    @a = gather {for (0 ..^@!blocks.elems) { take @!par-tables[$_] if @!blocks[$_].Tag.contains(%!engine<SelectedDataSet>) }}
	}
	else {
	    @a = @!par-tables;
	}
	@a.head;
     }
     
     method parameters (Bool :r(:read($r)),
			Bool :$from-output,
			Bool :$from-log,
			Bool :$to-json,
			Bool :$fix-all
		       ){
#	say "Greatings from update parameters form engine" if $from-engine;
#	say "Greatings from update parameters form output" if $from-output;
	#	say "Greatings from update parameters form log" if $from-log;
	if %!engine<FitType> ~~ /Individual/ {
	    for (1 .. @!blocks.elems).race -> $i {
		my $parameters;
		if @!blocks[$i-1].parameters.defined { $parameters = @!blocks[$i-1].parameters }
		else { $parameters = Parameters::Parameters.new.path($!path) }

		$parameters.from-engine(self) if none ($from-output.Bool,$from-log.Bool);
		$parameters.from-output(file=>"fit$i.out") if $from-output.Bool;
		$parameters.from-log(file=>"fit$i.log") if $from-log.Bool;
		@!par-tables[$i-1]= $parameters;
		@!blocks[$i-1].parameters=$parameters;
		if $fix-all.Bool { $parameters.parfile.write($parameters.a, No => $i, :fix-all) }
		else {$parameters.parfile.write($parameters.a, No => $i) }
		self!to-engine($parameters) if (any($from-output.Bool,$from-log.Bool) and @!blocks[$i-1].Tag.contains(%!engine<SelectedDataSet>));
		@!blocks[$i-1].chi2=$parameters.output{"chi2\[1\]"} if $parameters.output{"chi2\[1\]"};
	    }
	}
	else {
	    my $parameters;
	    if @!blocks.head.parameters.defined { $parameters = @!blocks.head.parameters }
	    else { $parameters = Parameters::Parameters.new.path($!path) }

	    $parameters.from-engine(self) if none ($from-output.Bool,$from-log.Bool);
	    $parameters.from-output(path => $!path) if $from-output.Bool;
	    $parameters.from-log(path => $!path) if $from-log.Bool;
	    @!par-tables[0]= $parameters;
	    for @!blocks {
		.parameters = $parameters;
		.chi2 = $parameters.output{'chi2['~ .No+1 ~']'} if $parameters.output{'chi2[' ~ .No+1 ~ ']'};
	    }
	    if $fix-all.Bool { $parameters.parfile.write($parameters.a, path => $!path, :fix-all) }
	    else { $parameters.parfile.write($parameters.a, path => $!path) }
	    self!to-engine($parameters) if any($from-output.Bool,$from-log.Bool);
	}
	%!engine<par-tables>=@!par-tables>>.table;
	if $to-json { to-json(@!par-tables>>.table, :sorted-keys) } 
	else { self }
     }

     method stp () {
	 dir($!path, :test(/\.stp/)).race.map({ $_.unlink if $_.IO.f });
	 Stpfile.new.update(self, path => $!path);
	 self
     }

     method agr () {
	dir($!path, :test(/\.agr\-par/)).race.map({ $_.unlink if $_.IO.f });
	#	@!blocks.race>>.map({ .write-agr });
	@!blocks.race>>.write-agr(path => $!path);
	self
     }

     method fit(Bool :$no-plot,
		Bool :$autox,
		Bool :$autoy,
		Bool :$logx,
		Bool :$logy,
		Bool :$quiet=False
	       ) {
	 dir($!path, :test(/par|\.c|out|agr|agr\-par|log|res|fit/)).race.map({ $_.unlink if $_.IO.f });
	 say "read blocks" unless $quiet;
	 self.blocks(:read,
		     :fit,:export,
		     :autox($autox.Bool),
		     :autoy($autoy.Bool),
		     :logx($logx),
		     :logy($logy),
		     :quiet($quiet)
		    );
	 say "read pars" unless $quiet;
	 self.parameters(:read);
	 say "read functions" unless $quiet;
	 self.functions(:read);
	 say "write stp" unless $quiet;
	 self.stp;
	 say "write code" unless $quiet;
	 self.code(:write,:compile, :quiet($quiet));
	 if %!engine<FitType> ~~ /Individual/ {
	     for (1 .. @!blocks.elems).race {
		 shell "cd $!path; ./onefit-user -@fitenv$_.stp -f -pg data$_.dat <fit$_.par >fit$_.log 2>&1; cp fit-residues-1.res fit-residues-$_.res-tmp";
	     }
	     for (1 .. @!blocks.elems).race {
		 shell "cd $!path; mv fit-residues-$_.res-tmp fit-residues-$_.res" ;
	     }
	     @!blocks.race.map( { .export(:plot) });
	     self.parameters(:read, :from-output, :from-log);
	     do {
		 self.agr;
		 for (1 .. @!blocks.elems).race {
		     shell "cd $!path; ./onefit-user -@fitenv$_.stp -nf -pg -ofit$_.out --grbatch=PDF data$_.dat <fit$_.par >plot$_.log 2>&1";
		 }
	     } unless $no-plot.Bool;
	 }
	 else {
	     my $datafiles = (1 ..@!blocks.elems).map({'data' ~ $_ ~ '.dat'}).join: ' ';
	     shell  "cd $!path; ./onefit-user -@fitenv.stp -f -pg $datafiles <fit.par >fit.log 2>&1";
	     @!blocks.race.map( { .export(:plot) });
	     self.parameters(:read, :from-output, :from-log);
	     do {
		 self.agr;
		 shell "cd $!path; ./onefit-user -@fitenv.stp -nf -pg -ofit.out --grbatch=PDF $datafiles <fit.par >plot.log 2>&1";
	     }  unless $no-plot;
	 }
	 my $TXT = self!results();
	 %!engine<fit-results> = $TXT;
	 %!engine<SimulFitOutput> = self!results(fmt => " ");
	 my @fit-curves;
	 for (1 .. @!blocks.elems) {
	     @fit-curves.push: "$!path/fit-curves-$_".IO.slurp
	 }
	 %!engine<fit-curves> = @fit-curves;
	 my @fit-residues;
	 for (1 .. @!blocks.elems) {
	     @fit-residues.push: "$!path/fit-residues-$_.res".IO.slurp
	 }
	 %!engine<fit-residues> = @fit-residues;
	 say $TXT unless $quiet;
	 self
     }

     method plot(Bool :$autox,
		 Bool :$autoy,
		 Bool :$logx,
		 Bool :$logy
		) {
	 dir($!path, :test(/par|\.c|out|agr|agr\-par|log|res|fit/)).race.map({ $_.unlink if $_.IO.f });
	 self.blocks(:read, :fit, :export, :autox($autox.Bool), :autoy($autoy.Bool), :logx($logx), :logy($logy));
	 self.parameters(:read);
	 self.functions(:read);
	 self.stp;
	 self.code(:write,:compile);
	 if %!engine<FitType> ~~ /Individual/ {
	     for (1 .. @!blocks.elems).race {
		 shell "cd $!path; ./onefit-user -@fitenv$_.stp -f -pg data$_.dat <fit$_.par >fit$_.log 2>&1; cp fit-residues-1.res fit-residues-$_.res-tmp";
	     }
	     for (1 .. @!blocks.elems).race {
		 shell "cd $!path; mv fit-residues-$_.res-tmp fit-residues-$_.res" ;
	     }
	     @!blocks.race.map( { .export(:plot) });
	     self.parameters(:read, :from-output, :from-log);
	     self.agr;
	     for (1 .. @!blocks.elems).race {
		 shell "cd $!path; ./onefit-user -@fitenv$_.stp -nf -pg -ofit$_.out --grbatch=PDF data$_.dat <fit$_.par >plot$_.log 2>&1";
	     }
	 }
	 else {
	     my $datafiles = (1 ..@!blocks.elems).map({'data' ~ $_ ~ '.dat'}).join: ' ';
	     shell  "cd $!path; ./onefit-user -@fitenv.stp -f -pg $datafiles <fit.par >fit.log 2>&1";
	     @!blocks.race.map( { .export(:plot) });
	     	     self.parameters(:read, :from-output);
	     self.agr;
	     shell "cd $!path; ./onefit-user -@fitenv.stp -nf -pg -ofit.out --grbatch=PDF $datafiles <fit.par >plot.log 2>&1";

	 }
	 self
     }

     method !to-engine($parameters) {
	for (0 ..^ $parameters.a) { %!engine{"Pval$_"} = $parameters.a[$_]<value>   }
	self
    }

     method !results (:$fmt = ', ') {
#	 my int $NPTS=0;
#	 my $parameters-tmp = Parameters::Parameters.new.path($!path);
	 my Bool $MIXED=False;
	 $MIXED = @!par-tables[0].a.tail<name>.contains("MIXED") and @!par-tables[0].a.tail<value> > 0.0;
	 my @fields = ("# TAG");
	 @fields.push: "Npts";
	 @fields.push: "chi2";
	 my @a = ("%!engine<T>_" <<~<< ( (0 ..^ @!blocks[0].T.words.elems) >>+>> 1 ) );
	 @fields.push: @a.Slip;
	 for @!par-tables.head.a { @fields.push: ( .<name>, "\x0B1" ~ "err" ).Slip }
	 my $TXT = @fields.join($fmt) ~ "\n";
	 if $MIXED {
	     my @line-fields;
	     @line-fields.push: "global";
	     @line-fields.push: @!blocks.map({ .X.elems }).sum;
	     @line-fields.push: @!blocks[0].chi2;
	     @line-fields.push: @!blocks[0].T.words.join($fmt);
	     for @!par-tables[0].a {
		 .<err>="-" unless .<err>.defined;
		 if so .<err> ~~ /fixed|constant/ { @line-fields.push: (.<value>, "{ .<err> }").Slip }
		 else { @line-fields.push: .<value err>.Slip  }
	     }
	     $TXT ~= @line-fields.join($fmt) ~ "\n";
	 }
	 for @!blocks {
#	     $NPTS += .X.elems;
	     my @line-fields;
	     my $i = %!engine<FitType> ~~ /Individual/ ?? .No !! 0;
	     @line-fields.push: .Tag;
	     @line-fields.push: .X.elems;
	     @line-fields.push: .chi2;
	     @line-fields.push: .T.words.join($fmt);
#	     my $par-table = @!par-tables[$i];
	     if "$!path/fit{.No+1}.log".IO.e and $MIXED {
		 .parameters.from-output(file => "fit{ .No+1 }.out");
		 .parameters.from-log(file => "fit{ .No+1 }.log");
		 $par-tables[$i] = .parameters;
#		 $parameters-tmp.from-engine(self);
#		 $parameters-tmp.from-output(file => "fit{ .No+1 }.out");
#		 $parameters-tmp.from-log(file => "fit{ .No+1 }.log");
#		 $par-table=$parameters-tmp;
	     }

	     for @!par-tables[$i].a {
#	     for $par-table.a {
		 .<err>="-" unless .<err>.defined;
		 if so .<err> ~~ /fixed|constant/ { @line-fields.push: (.<value>, "{ .<err> }").Slip }
		 else { @line-fields.push: .<value err>.Slip  }
	     }
	     $TXT ~= @line-fields.join($fmt) ~ "\n";
	 }
	 
	 return $TXT;
     }
}

