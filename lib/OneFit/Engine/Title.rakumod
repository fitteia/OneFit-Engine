unit module OneFit::Engine::Title;

class Title is export {
    has %!title = %(
	title => "",
	posx  => 0.0,
	posy  => 0.0
    );
    
    method read ($str, Bool :h(:hash($h))) {
	my @arr = $str.split: '"';
	%!title<title> = @arr[1];
	(%!title<posx>,%!title<posy>)=@arr[2].words;
	($h.Bool) ?? %!title !! self;
    }
    method h () { %!title }
    method hash () { %!title }
}
