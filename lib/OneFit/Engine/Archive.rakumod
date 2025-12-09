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
		my @a = $s.words;	
		my $add-options = @a.elems>1 ?? @a[1..*].join(' ') !! ();
		given @a[0] {
			when /:i last <ws> '-' <ws> \d+ / { $selected =@keys[@keys.elems - 1 - $s.split('-')[1].trim.Int] }
			when /^\d+/ { $selected = @keys[@a[0].Int] }
		   	default { $selected }
		}
		try { 
			my $cmd = %!arch{$selected}.subst('#','\#').subst(/ <ws> '--ar' \w* <ws> /,' ');
			note "===> trying to execute: unzip -o $!path/{$selected}.zip; $cmd $add-options";
			shell("unzip -o $!path/{$selected}.zip; $cmd  $add-options");
	   	}
		if $! { note "     couldn't execute" ~ %!arch{$selected} ~ $add-options }
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
				when * < 1024 	 { $size =  ($size).fmt("%.0f B") } 
				when * < 1024**2 { $size =  ($size / 1e3).fmt("%.0f kB") } 
				when * < 1024**3 { $size =  ($size / 1e6).fmt("%.0f MB") } 
				default			 { $size =  ($size / 1e9).fmt("%.0f GB") } 
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
		my @words = $cmd.words;
		@words[2] = qq{"@words[2]"};
		%!arch{$short} = @words.join(' ');
		"$.path/$.file".IO.spurt: to-json(%!arch, :sorted-keys);
		my @files;
		for $cmd.words[2..*].grep(/^ <-[-]> <-[,]>* /) {
			@files.push: /'*'/ ?? shell("ls $_",:out).out.slurp.words.Slip !! $_ 	
		}
		@files .= grep( *.IO.f );
		"$.path/{$short}.zip".IO.unlink if "$.path/{$short}.zip".IO.e;
		my $p = run "zip", "$.path/{$short}.zip", @files, :out, :err;
		if $p.exitcode { say "couldn't archive this onefite instance."; say $p.err.slurp; return 1 }
		self;
	}
}
