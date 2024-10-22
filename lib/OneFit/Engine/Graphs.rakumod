unit module OneFit::Engine::Graphs;

use OneFit::Engine::Titles;
use OneFit::Engine::Axes;

class Graph is export {
    has $.Title is rw = Title.new.read('# TITLE "" 0.0 0.0');
    has $.Xaxis is rw = Axis.new.read("# XAXIS Normal 0 10 1 1 decimal 0 x axis (a.u)");
    has $.Yaxis is rw = Axis.new.read("# YAXIS Normal 0 10 1 1 decimal 0 y axis (a.u)");
    has $.gph is rw = "";
    has @.Curves is rw ;
    has $!path = '.';

    method path ($folder) { $!path = $folder; self }
    
    method rminf () {
	"$!path/$.gph".IO.spurt: gather { for "$!path/$.gph".IO.lines { take $_ unless $_.contains(/inf|nan/) } } .join("\n");
	self
    }
    method autoposxy($agr) {
	for (1 ..^ @.Curves.elems).race -> $i {
	    if @.Curves[$i]<posx>.Str.contains("auto") {
		my ($xi,$xf)=($.Xaxis.h<min>,$.Xaxis.h<max>);
		($xi,$xf)=(log($xi),log($xf)) if $.Xaxis.h<type>.contains(/Logarithmic/);
		@.Curves[$i]<posx> = $xi + ($i)*($xf-$xi)/(@.Curves.elems+1);
		@.Curves[$i]<posx> = exp(@.Curves[$i]<posx>) if $.Xaxis.h<type>.contains(/Logarithmic/);
	    }
	    
#	    @.Curves[$i]<posy>=($.Yaxis.h<max>-$.Yaxis.h<min>)/2;
	    my $target = @.Curves[$i]<posx>;
	    say "target", $target;
	    @.Curves[$i]<posy>=$.Yaxis.h<min>;
	    my (@posx,@posy);
	    say "$!path/$.gph".IO.slurp;
	    for "$!path/$.gph".IO(:e).slurp.lines.skip {
#		say $_;
		my @cols = .words;
#		say @cols;
		if @cols[@.Curves[$i]<No>] >= $.Yaxis.h<min> and @cols[@.Curves[$i]<No>] <= $.Yaxis.h<max> {
		    @posx.push: @cols[0];
#		    say "col[0]=",@cols[0];
		    @posy.push(@cols[@.Curves[$i]<No>]);
		}
	    }
	    if @posy.elems > 0 {
		for (0 ..^ @posy.elems) {
		    @.Curves[$i]<posx> = @posx[$_];
		    @.Curves[$i]<posy> = @posy[$_];
		    say "set", @posx[$_];
		    last if @posx[$_] >= $target;
		}
	    }
	    my ($ly,$lyi,$lyf) = (@.Curves[$i]<posy>,$.Yaxis.h<min>,$.Yaxis.h<max>);
	    ($ly,$lyi,$lyf) = ($ly,$lyi,$lyf)>>.log if $.Yaxis.h<type> ~~ /Logarithmic/;
	    $ly = $ly + 0.005*($lyf - $lyi)/($agr.view_ymax - $agr.view_ymin);
	    $ly = $ly - 0.03*($lyf-$lyi)/($agr.view_ymax-$agr.view_ymin) if $ly > 0.9*$lyf;
	    if $.Yaxis.h<type> ~~ /Logarithmic/ { @.Curves[$i]<posy> = exp($ly) }
	    else { @.Curves[$i]<posy> = $ly }
	    say "final",@.Curves[$i]<posx>;
	}
	self
    }
}
