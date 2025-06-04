unit module OneFit::Engine::Agrs;

class template is export {
    has $.view_xmin is rw = 0.183247;
    has $.view_xmax is rw = 1.038397; 
    has $.view_ymin is rw = 0.149994; 
    has $.view_ymax is rw = 0.849964;
    has $.errorbars is rw = "off";
    has $.symbolsize is rw = 1;
    has $.linetype is rw = 1;
    has $.color is rw =1;
    has $.type is rw = "xy";
    
    method head ($title,$posx,$posy,$tagtoprint,$symbolsize) {
	qq:to/EOT/;
# Grace project file
#
version 50114
page size 773, 600
page scroll 5%
page inout 5%
link page off
map font 0 to \"Times-Roman\", \"Times-Roman\"
map font 2 to \"Times-Italic\", \"Times-Italic\"
map font 1 to \"Times-Bold\", \"Times-Bold\"
map font 3 to \"Times-BoldItalic\", \"Times-BoldItalic\"
map font 4 to \"Helvetica\", \"Helvetica\"
map font 6 to \"Helvetica-Oblique\", \"Helvetica-Oblique\"
map font 5 to \"Helvetica-Bold\", \"Helvetica-Bold\"
map font 7 to \"Helvetica-BoldOblique\", \"Helvetica-BoldOblique\"
map font 10 to \"Courier-Bold\", \"Courier-Bold\"
map font 11 to \"Courier-BoldOblique\", \"Courier-BoldOblique\"
map font 8 to \"Symbol\", \"Symbol\"
map font 9 to \"ZapfDingbats\", \"ZapfDingbats\"
map color 0 to (255, 255, 255), \"white\"
map color 1 to (0, 0, 0), \"black\"
map color 2 to (255, 0, 0), \"red\"
map color 3 to (0, 255, 0), \"green\"
map color 4 to (0, 0, 255), \"blue\"
map color 5 to (255, 255, 0), \"yellow\"
map color 6 to (188, 143, 143), \"brown\"
map color 7 to (220, 220, 220), \"grey\"
map color 8 to (148, 0, 211), \"violet\"
map color 9 to (0, 255, 255), \"cyan\"
map color 10 to (255, 0, 255), \"magenta\"
map color 11 to (255, 165, 0), \"orange\"
map color 12 to (114, 33, 188), \"indigo\"
map color 13 to (103, 7, 72), \"maroon\"
map color 14 to (64, 224, 208), \"turquoise\"
map color 15 to (0, 139, 0), \"green4\"
reference date 0
date wrap on
date wrap year 1900
default linewidth 1.0
default linestyle 1
default color 1
default pattern 1
default font 4
default char size 1.000000
default symbol size {$symbolsize}
default sformat \"%16.8g\"
background color 0
page background fill on
timestamp off
timestamp 0.03, 0.03
timestamp color 1
timestamp rot 0
timestamp font 4
timestamp char size 1.000000
timestamp def \"\"
with string
    string on
    string loctype view
    string 0.05, 0.05
    string color 4
    string rot 0
    string font 4
    string just 0
    string char size 1.000000
    string def \"\[{$tagtoprint}\]\"
with string
    string on
    string loctype world
    string g0
    string {$posx}, {$posy}
    string color 1
    string rot 0
    string font 0
    string just 0
    string char size 1.500000
    string def \"{$title}\"
EOT
	}
	
	method string ($str,$color,$posx,$posy) {
	       qq:to/EOTT/;
with string
    string on
    string loctype world
    string g0
    string {$posx}, {$posy}
    string color {$color}
    string rot 0
    string font 0
    string just 0
    string char size 1.500000
    string def \"{$str}\"
EOTT
        }

	method setgraph ($X,$Y) {
		say $X.h;
	qq:to/EOTEXT/;
r0 off
link r0 to g0
r0 type above
r0 linestyle 1
r0 linewidth 1.0
r0 color 1
r0 line 0, 0, 0, 0
r1 off
link r1 to g0
r1 type above
r1 linestyle 1
r1 linewidth 1.0
r1 color 1
r1 line 0, 0, 0, 0
r2 off
link r2 to g0
r2 type above
r2 linestyle 1
r2 linewidth 1.0
r2 color 1
r2 line 0, 0, 0, 0
r3 off
link r3 to g0
r3 type above
r3 linestyle 1
r3 linewidth 1.0
r3 color 1
r3 line 0, 0, 0, 0
r4 off
link r4 to g0
r4 type above
r4 linestyle 1
r4 linewidth 1.0
r4 color 1
r4 line 0, 0, 0, 0
g0 on
g0 hidden false
g0 type XY
g0 stacked false
g0 bar hgap 0.000000
g0 fixedpoint off
g0 fixedpoint type 0
g0 fixedpoint xy 0.000000, 0.000000
g0 fixedpoint format general general
g0 fixedpoint prec 6, 6
with g0
    world xmin {$X.h<min>}
    world xmax {$X.h<max>}
    world ymin {$Y.h<min>}
    world ymax {$Y.h<max>}
    stack world 0, 0, 0, 0
    znorm 1
    view xmin {$.view_xmin}
    view xmax {$.view_xmax}
    view ymin {$.view_ymin}
    view ymax {$.view_ymax}
    title \"\"
    title font 4
    title size 1.500000
    title color 1
    subtitle \"\"
    subtitle font 4
    subtitle size 1.000000
    subtitle color 1
    xaxes scale {$X.h<type>}
    yaxes scale {$Y.h<type>}
    xaxes invert off
    yaxes invert off
    xaxis  on
    xaxis  type zero false
    xaxis  offset 0.000000 , 0.000000
    xaxis  bar off
    xaxis  bar color 1
    xaxis  bar linestyle 1
    xaxis  bar linewidth 2.0
    xaxis  label \"{$X.h<label>}\"
    xaxis  label layout para
    xaxis  label place auto
    xaxis  label char size 1.500000
    xaxis  label font 4
    xaxis  label color 1
    xaxis  label place normal
    xaxis  tick on
    xaxis  tick major {$X.h<step>}
    xaxis  tick minor ticks {$X.h<ticks>}
    xaxis  tick default 6
    xaxis  tick place rounded true
    xaxis  tick in
    xaxis  tick major size 1.500000
    xaxis  tick major color 1
    xaxis  tick major linewidth {$X.h<gridlinewidth>}
    xaxis  tick major linestyle 1
    xaxis  tick major grid {$X.h<grid>}
    xaxis  tick minor color 1
    xaxis  tick minor linewidth {$X.h<gridlinewidth>}
    xaxis  tick minor linestyle 1
    xaxis  tick minor grid {$X.h<grid>}
    xaxis  tick minor size 1.000000
    xaxis  ticklabel on
    xaxis  ticklabel format {$X.h<format>}
    xaxis  ticklabel prec {$X.h<precision>}
    xaxis  ticklabel formula \"\"
    xaxis  ticklabel append \"\"
    xaxis  ticklabel prepend \"\"
    xaxis  ticklabel angle 0
    xaxis  ticklabel skip 0
    xaxis  ticklabel stagger 0
    xaxis  ticklabel place normal
    xaxis  ticklabel offset auto
    xaxis  ticklabel offset 0.000000 , 0.010000
    xaxis  ticklabel start type auto
    xaxis  ticklabel start 0.000000
    xaxis  ticklabel stop type auto
    xaxis  ticklabel stop 0.000000
    xaxis  ticklabel char size 1.500000
    xaxis  ticklabel font 4
    xaxis  ticklabel color 1
    xaxis  tick place both
    xaxis  tick spec type none
    yaxis  on
    yaxis  type zero false
    yaxis  offset 0.000000 , 0.000000
    yaxis  bar off
    yaxis  bar color 1
    yaxis  bar linestyle 1
    yaxis  bar linewidth 1.0
    yaxis  label \"{$Y.h<label>}\"
    yaxis  label layout para
    yaxis  label place auto
    yaxis  label char size 1.500000
    yaxis  label font 4
    yaxis  label color 1
    yaxis  label place normal
    yaxis  tick on
    yaxis  tick major {$Y.h<step>}
    yaxis  tick minor ticks {$Y.h<ticks>}
    yaxis  tick default 6
    yaxis  tick place rounded true
    yaxis  tick in
    yaxis  tick major size 1.500000
    yaxis  tick major color 1
    yaxis  tick major linewidth {$Y.h<gridlinewidth>}
    yaxis  tick major linestyle 1
    yaxis  tick major grid {$Y.h<grid>}
    yaxis  tick minor color 1
    yaxis  tick minor linewidth {$Y.h<gridlinewidth>}
    yaxis  tick minor linestyle 1
    yaxis  tick minor grid {$Y.h<grid>}
    yaxis  tick minor size 1.000000
    yaxis  ticklabel on
    yaxis  ticklabel format {$Y.h<format>}
    yaxis  ticklabel prec {$Y.h<precision>}
    yaxis  ticklabel formula \"\"
    yaxis  ticklabel append \"\"
    yaxis  ticklabel prepend \"\"
    yaxis  ticklabel angle 0
    yaxis  ticklabel skip 0
    yaxis  ticklabel stagger 0
    yaxis  ticklabel place normal
    yaxis  ticklabel offset auto
    yaxis  ticklabel offset 0.000000 , 0.010000
    yaxis  ticklabel start type auto
    yaxis  ticklabel start 0.000000
    yaxis  ticklabel stop type auto
    yaxis  ticklabel stop 0.000000
    yaxis  ticklabel char size 1.500000
    yaxis  ticklabel font 4
    yaxis  ticklabel color 1
    yaxis  tick place both
    yaxis  tick spec type none
    altxaxis  off
    altyaxis  off
    legend on
    legend loctype view
    legend 0.977315227809, 0.799966426
    legend box color 1
    legend box pattern 0
    legend box linewidth 1.0
    legend box linestyle 1
    legend box fill color 0
    legend box fill pattern 1
    legend font 4
    legend char size 1.500000
    legend color 1
    legend length 4
    legend vgap 1
    legend hgap 1
    legend invert false
    frame type 0
    frame linestyle 1
    frame linewidth 2.0
    frame color 1
    frame pattern 1
    frame background color 0
    frame background pattern 0
EOTEXT
	}	

	method dataset ($k,:t(:$type),Bool :s(:$symbol),:ss(:$symbolsize),:e(:$errorbars), Bool :l(:$line), :lt(:$linetype), :c(:$color)) {

	qq:to/EOP/;   
    s$k hidden false
    s$k type {($type.defined) ?? $type !! $.type}
    s$k symbol {($symbol) ?? 1 !! 0}
    s$k symbol size {($symbolsize.defined) ?? $symbolsize !! $.symbolsize}
    s$k symbol color 1
    s$k symbol pattern 1
    s$k symbol fill color 1
    s$k symbol fill pattern 0
    s$k symbol linewidth 2.0
    s$k symbol linestyle 1
    s$k symbol char 65
    s$k symbol char font 4
    s$k symbol skip 0
    s$k line type {($line) ?? 1 !! 0}
    s$k line linestyle {($linetype.defined) ?? $linetype.subst(/\D/,'',:g) !! $.linetype}
    s$k line linewidth 2.0
    s$k line color {($color.defined) ?? $color.subst(/\D/,'',:g) !! $.color}
    s$k line pattern 1
    s$k baseline type 0
    s$k baseline off
    s$k dropline off
    s$k fill type 0
    s$k fill rule 0
    s$k fill color 1
    s$k fill pattern 1
    s$k avalue off
    s$k avalue type 2
    s$k avalue char size 1.000000
    s$k avalue font 4
    s$k avalue color 1
    s$k avalue rot 0
    s$k avalue format general
    s$k avalue prec 3
    s$k avalue prepend \"\"
    s$k avalue append \"\"
    s$k avalue offset 0.000000 , 0.000000
    s$k errorbar {($errorbars.defined) ?? $errorbars !! $.errorbars}
    s$k errorbar place both
    s$k errorbar color 1
    s$k errorbar pattern 1
    s$k errorbar size 1.000000
    s$k errorbar linewidth 1.0
    s$k errorbar linestyle 1
    s$k errorbar riser linewidth 1.0
    s$k errorbar riser linestyle 1
    s$k errorbar riser clip off
    s$k errorbar riser clip length 0.100000
    s$k comment \"sm1-gph.agr\"
    s$k legend  \"\"
EOP


	}	
}
