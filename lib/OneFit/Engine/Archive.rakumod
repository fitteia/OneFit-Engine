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

	multi method fit($s, *@_) {
		my @keys =  %!arch.keys.sort;
		my $selected = @keys.tail;
		my @add-options=@_.so ?? @_ !! False;
		given $s {
			when /:i last <ws> '-' <ws> \d+ / { $selected =@keys[@keys.elems - 1 - $s.split('-')[1].trim.Int] }
			when /\d+/ { $selected = @keys[$s.Int] }
		   	default { $selected }
		}
		try { 
			my $cmd = %!arch{$selected}.subst('#','\#').subst(/ <ws> '--ar' \w* <ws> /,'');
			note "===> trying to execute: unzip -o $!path/{$selected}.zip; $cmd";
			shell("unzip -o $!path/{$selected}.zip; $cmd {|@add-options}");
	   	}
		if $! { note "     couldn't execute" ~ %!arch{$selected} }
	}

	multi method list() {
		%!arch.pairs.sort(*.keys).values;
	}

	multi method print() {
		my $width = %!arch.keys.elems.chars;
		my $i=0;
		for %!arch.pairs.sort(*.keys)  -> $p {
			my $size ="$.path/{$p.key}.zip".IO.s.Int;
		  	given $size {
				when * < 1024 	 { $size =  ($size).fmt("%.0fB") } 
				when * < 1024**2 { $size =  ($size / 1e3).fmt("%.0fkB") } 
				when * < 1024**3 { $size =  ($size / 1e6).fmt("%.0fMB") } 
				default			 { $size =  ($size / 1e9).fmt("%.0fGB") } 
			}
			say $i.fmt("\%{$width}d") 
			~ ": " 
			~ DateTime.new($p.key.parse-base(36)) 
			~ " {$p.key}.zip " 
			~ "[" ~ $size ~ "]"
			~ " <== " 
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
