unit module OneFit::Engine::Grammars;

class Output-Actions {
    method TOP                ($/) { make $<pair>.map({ .made }) }
    method key                ($/) { make $/.Str }
    method pair               ($/) { make $<key>.made => $<number>.made }
    method number             ($/) { make +$/ }
    method comment            ($/) { make $/.Str }
}


grammar Number {
    token number {
	<[+-]>?
	[ 0 | <[1..9]><[0..9]>* ]+
	[ '.' <[0..9]>* ]?
	[ <[eE]> <[+-]>? <[0..9]>+ ]?
    }
}

grammar Output is Number {
    token TOP { [ <comment> | <pair> | <.covar> | \n]+ }
    token comment { '#' \N* \n }
    token pair   { \h* <key> \h* '=' \h* <number>    \h* }
    token covar  { \h* <key> \h* '=' \h* \n <.matrix> \h* }
    token key    { [ \w | '_' | '-' | '(' | ')' | ',' | '[' | ']' | '.' | '+']+ }
    token matrix { [[<number> \h*]+ \n]+ }

    method parse-made(Str $input, :$rule='TOP') {
	my $m = self.parse($input, :actions(Output-Actions), :rule($rule));
	unless $m { die "input:[$input] is not valid!" }
	return $m.made
    }
}

grammar Log is Number {
     token TOP { [ <.comment> | <line> | \h* <.char> \h* ]+ }
    #    token TOP { [ \h* <number> \h* | \h* <word> \h* | \h* <char> \h* | \n ]+ }
    token comment {\h* \D \N+ \n*}
    token line      { \h* <number> \h+ <word> \h+ <number> \h+ [<word>] [ <.word> | <.char> | \h]* <.nl>* }
    token word      { [ \w | '_' | '-' | '(' | ')' | ',' | '[' | ']' | '.' | '+' ]+}
    token char      { [ '*' | ':' | '=' | '.' ]+ }
    token nl        { \n }
}

grammar Function is Number {
    token TOP           { <Y> \h* '(' <X> ','? <pars>? ')' \h* <range>? \h* '=' \h* <expression> \h* }
    token  Y            { <.ws> <par> <.ws> }
    token  X            { <.ws> <par> <.ws> }
    token pars          { [ <.ws> <par> <.ws> ','? <.ws> ]+ }
    token par           { [ <range> <.ws> <attrib>? ] | [ <name> <.ws> <attrib>? ] }
    token range         { [ <min> <le> <name> <le> <max> ] |
			  [ <max> <ge> <name> <ge> <min> ] |
			  [ <min> <le> <name> ]            |
			  [ <name> <le> <max> ]            |
			  [ <max> <ge> <name> ]            |
			  [ <name> <ge> <min> ]            |
			  [ <name>? <.ws> [ <attrib> | <column> ]? <.ws> '[' <.ws> <min> <.sep> <max> <.ws> ']' ]
			}
    token name          { \w+ }
    token min           { <number> <.ws> [ ',' <.ws> <number>]* }
    token max           { <number> <.ws> [ ',' <.ws> <number>]* }
    token attrib        { [ <column> <.ws> <initial-value>]         |
			  [ <eq> <.ws> <fixed-value> ]
			}
    token initial-value { <number> }
    token fixed-value   { <number> }
    token le            { <.ws> '<' '='? <.ws> }
    token ge            { <.ws> '>' '='? <.ws> }
    token column        { ':' }
    token sep           { <.ws> [ ';' ]+ <.ws> }
    token eq            { '=' }
    token expression    { \N+ }
    token word          { \w+ }

    method parse-me ($input) {
	my $m= self.parse($input);
	my %res;
	%res<Parameters> = $m<pars><par>.map({ $_<name> or $_{'range'}<name> }).Array>>.Str.join(',');
	%res<Y>    = { $m<Y><par><name>  or $m<Y><par><range><name> }().Str;
	if $m<Y><par><range><min>.Bool {
	    %res<Ymin> = $m<Y><par><range><min>.Str;
	    %res<Ymax> = $m<Y><par><range><max>.Str
	}
	elsif $m<range><min>.Bool {
	    %res<Ymin> = $m<range><min>.Str;
	    %res<Ymax> = $m<range><max>.Str;
	}
	else {
	    %res<Ymin> = 0;
	    %res<Ymax> = 10;
	}
	%res<X> = { $m<X><par><name>  or $m<X><par><range><name> }().Str;
	%res<Xmin> = $m<X><par><range>.Bool ??  $m<X><par><range><min>.Str !! 0;
	%res<Xmax> = $m<X><par><range>.Bool ??  $m<X><par><range><max>.Str !! 10;
	%res<Function> = $m<expression>.Str;
	my $i=0;
	for $m<pars><par> -> $par {
	    %res{"Pval" ~ $i} = 1;
	    %res{"Pmin" ~ $i} = "";
	    %res{"Pmax" ~ $i} = "";
	    %res{"F" ~ $i} = "Free";

	    %res{'Pval'~$i} = $par<attrib><range><min>.Str if $par<attrib><range><min>.Bool;
	    %res{'Pval'~$i} = $par<range><attrib><initial-value>.Str if $par<range><attrib><initial-value>.Bool;
	    if $par<attrib><eq>.Bool {
		%res{'Pval'~ $i } = $par<attrib><fixed-value>.Str;
		%res{'F' ~ $i} = "Fix";
	    }
	    if $par<range><attrib><eq>.Bool {
		%res{'Pval'~ $i } = $par<range><attrib><fixed-value>.Str;
		%res{'F' ~ $i} = "Fix";
	    }
	    %res{'Pmin'~$i} = $par<range><min>.Str if $par<range>.Bool;
	    %res{'Pmax'~$i} = $par<range><max>.Str if $par<range>.Bool;
	    $i++
	}
	return %res;
    }
}

grammar Label is Number {
    token TOP    { <.ws> <name> <.ws> <attrib>? <.ws> <values>? <.ws> [ <sep> <.ws> <values> ]* }
    token values { [ <number> <.ws> ] }
    token name   { \w+ }
    token attrib    { ':' | '=' }
    token sep    { ',' | ';' }
}

grammar Data is Number {
    token TOP { [ <data> | <tag> | <matrix> | \n]+ }
    token data   { [ \h* '#' \h* 'DATA' \h* <key> \h* '=' \h* [ <number> \h*]+ ]+ }
    token tag    { [ \h* '#' \h* 'TAG' \h* '=' \h* <key> \h* ]+ }
    token key    { [ \w | '_' | '-' | '(' | ')' | ',' | '[' | ']' | '.' | '+']+ }
    token matrix { [ \h* [<number> \h*]+ \n]+ }

    method parse-me ($input) {
	my $m= self.parse($input);
	say $m;
	my %res;
	%res<data>=$m<data><key>.Str;
	%res<tag>=$m<tag><key>.Str;
	return %res
    }
}

