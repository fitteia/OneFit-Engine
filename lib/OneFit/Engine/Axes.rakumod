unit module OneFit::Engine::Axes;

class Axis is export {
    has %!axis = %(
	auto      => True,
	precision => 0
    );
    
    method read ($str, Bool :h(:hash($h))) {
	my @arr = $str.split: /\s+/;
	my $i=1;
	for <tag type min max step ticks format precision> {
	    %!axis{$_}=@arr[$i++];
	}
	%!axis<label>=@arr[9 ..^ @arr.elems].join: " ";
	%!axis<grid>="off";
	%!axis<gridlinetype> =1;
	%!axis<gridlinewidth>=2;
	($h.Bool) ?? %!axis !! self;
    }
    multi method auto () { %!axis<auto> }
    multi method auto (Bool $a) { %!axis<auto>=$a; self }
    multi method precision () { %!axis<precision> }
    multi method precision ($val) { %!axis<precision>=$val; self }
    multi method type () { %!axis<type>}
    multi method type ($type) { %!axis<type> = $type; self }
    method get () { %!axis }
    method h () { %!axis }
    method hash () { %!axis }
    method grid (Str :$set) {
	if $set.defined {
	    %!axis<grid>=$set;
	    %!axis<gridlinetype> =2;
	    
	}
	self
    }
    method scale (:$min,:$max,:$nt, Bool :$auto) {
	# algorithm from chatGPT
	if %!axis<type> eq "Normal" {
	    %!axis<step> = ($max-$min)/$nt; # unrouded step 
	    my $mag  = 10**(floor(log10(%!axis<step>)));
	    %!axis<step> = (2,5,10).grep( { $_*$mag >= %!axis<step> } )[0] * $mag; # rounded step
	    %!axis<min max>=<$min $max>;
	    if $auto.Bool {
		%!axis<min>  = floor(($min-%!axis<step>/4)/%!axis<step>)*%!axis<step>;
		%!axis<max>  = ceiling(($max+%!axis<step>/4)/%!axis<step>)*%!axis<step>;
	    }
	    %!axis<precision>=abs(log10(%!axis<step>).floor) if log10(%!axis<step>).floor < 0;
	}
	else {
	    %!axis<step> = 10;
	    %!axis<ticks> = 9;
	    %!axis<min max>=<$min $max>;
	    if $auto.Bool {
		%!axis<min>  = 0.9*$min;
		%!axis<max>  = 1.1*$max;
	    }
	    %!axis<precision> = 0;
	    %!axis<format>    = "power";
	}
	say %!axis<min max>;
	self
    }
    method auto-scale (:$min,:$max,:$nt) {
	# algorithm from chatGPT
	if %!axis<type> eq "Normal" {
	    %!axis<step> = ($max-$min)/$nt; # unrouded step 
	    my $mag  = 10**(floor(log10(%!axis<step>)));
	    %!axis<step> = (2,5,10).grep( { $_*$mag >= %!axis<step> } )[0] * $mag; # rounded step
	    %!axis<min>  = floor(($min-%!axis<step>/4)/%!axis<step>)*%!axis<step>;
	    %!axis<max>  = ceiling(($max+%!axis<step>/4)/%!axis<step>)*%!axis<step>;
	    %!axis<precision>=abs(log10(%!axis<step>).floor) if log10(%!axis<step>).floor < 0;
	}
	else {
	    %!axis<step> = 10;
	    %!axis<ticks> = 9;
	    %!axis<min>  = 0.9*$min;
	    %!axis<max>  = 1.1*$max;
	    %!axis<precision> = 0;
	    %!axis<format>    = "power";

	}
	self
    }
}
 
