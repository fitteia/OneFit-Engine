unit module OneFit::Engine::Archive;

use JSON::Fast;

class HistoryLog is export {
	has $.path is rw;
	has $.file is rw;
	has %!arch;

	submethod BUILD (:$!path = ::('OFE-PATH') ~ "/../log/arc", :$!file = "HistoryLog.json") {
		$!path.IO.mkdir unless $!path.IO.d;
		%!arch = from-json( "$!path/$!file".IO.slurp ).Hash.Slip if "$!path/$!file".IO.e;
	}

	multi method fit($s) {
		my @keys =  %!arch.keys.sort;
		my $selected = @keys.tail;
		given $s {
			when /:i last <ws> '-' <ws> \d+ / { $selected =@keys[@keys.elems - $s.split('-')[1].trim.Int] }
			when /\d+/ { $selected = @keys[$s.Int] }
		   	default { $selected }
		}	
		try { 
			say "trying to execute" ~ %!arch{$selected};
			my $cmd = %!arch{$selected}.subst('#','\#').subst(/ <ws> '--ar' \w* <ws> /,'');
			shell("unzip -o $!path/{$selected}.zip; $cmd");
	   	}
		if $! { say "couldn't execute" ~ %!arch{$selected} }
	}

	multi method list() {
		%!arch.pairs.sort(*.keys).values;
	}

	multi method print() {
		my $width = %!arch.keys.elems.chars;
		my $i=0;
		for %!arch.pairs.sort(*.keys)  -> $p {
			say $i.fmt("\%{$width}d") 
			~ ": " 
			~ "[{$p.key}] " 
			~ DateTime.new($p.key.parse-base(36)) 
			~ " " ~ "$.path/{$p.key}.zip".IO.s ~ "B "
			~ " ==> " 
			~ $p.value;
			$i++;	
		}
		self;
	}

	multi method archive(Str $cmd) {
		my $dt = DateTime.now;
		my $epoch = $dt.posix.Int;
		my $short = $epoch.base(36);
		%!arch{$short}=$cmd;
		"$.path/$.file".IO.spurt: to-json(%!arch, :sorted-keys);
		my @files;
		for $cmd.words[2..*].grep(/^<![-]>/) {
			@files.push: /'*'/ ?? shell("ls $_",:out).out.slurp.words.Slip !! $_ 	
		}
		@files .= grep( *.IO.f );
		"$.path/{$short}.zip".IO.unlink if "$.path/{$short}.zip".IO.e;
		my $p = run "zip", "$.path/{$short}.zip", @files, :out, :err;
		if $p.exitcode { say "couldn't archive this onefite instance."; say $p.err.slurp; return 1 }
		self;
	}
}
