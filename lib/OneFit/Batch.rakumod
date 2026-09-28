#| Parallel independent fits: `onefite fit A B ...` where every argument is
#| a complete fit - a saved engine (.json/.sav), a packed job
#| '#alias,data...' or a jobs file @fits.txt - runs each fit as its own
#| `onefite fit` process, --jobs at a time, each in its own folder of a new
#| batch folder, and records them in batch.json. The same rules as the Go
#| port's (onefite-native cmd/onefite/batch.go).
unit module OneFit::Batch;

use JSON::Fast;

sub is-saved-fit(Str $a --> Bool) is export { so $a.lc.ends-with('.json' | '.sav') }

#| Splits on "," but not on "\," (a comma inside a file name), unescaping those.
sub split-packed(Str $s) is export {
    my @parts = '';
    my $i = 0;
    while $i < $s.chars {
        my $c = $s.substr($i, 1);
        if $c eq '\\' && $s.substr($i + 1, 1) eq ',' { @parts[*-1] ~= ','; $i += 2; next }
        if $c eq ',' { @parts.push(''); $i++; next }
        @parts[*-1] ~= $c;
        $i++;
    }
    @parts
}

#| '#alias,data...' - the alias shorthand, then a comma not escaped as "\,".
sub is-packed-job(Str $a --> Bool) is export { so $a.starts-with('#') && split-packed($a).elems >= 2 }

sub is-jobs-file(Str $a --> Bool) is export { so $a.starts-with('@') && $a.chars > 1 }

sub is-complete-fit(Str $a --> Bool) is export { is-saved-fit($a) || is-packed-job($a) || is-jobs-file($a) }

#| Every argument a complete fit, and more than one fit (or a jobs file,
#| which may hold several). One packed job alone is not parallel: it is the
#| single fit `fit '#alias' data...`.
sub is-parallel-fits(@pos --> Bool) is export {
    so @pos.elems > 0 && all(@pos.map({ is-complete-fit(~$_) })) && (@pos.elems >= 2 || is-jobs-file(~@pos[0]))
}

# The canonical name of each fit option spelling (as the Go port's
# fitFlagAliasCanonical), so a line's --fm replaces the batch's
# --fit-methods, its --g the batch's --global, and so on.
my constant %canonical =
    np => 'no-plot', q => 'quiet', rc => 'reduced-chi2', rchi2 => 'reduced-chi2', eb => 'error-bars',
    errorbars => 'error-bars', pco => 'R2', peco => 'R2', pearson-correlation => 'R2', g => 'global',
    fm => 'fit-methods', n => 'num', N => 'num', Num => 'num', npts => 'num', SymbSize => 'symbol-size',
    ssz => 'symbol-size', data-label => 'data-labels', fi => 'fit-if', pi => 'plot-if', ac => 'aux-code',
    AC => 'aux-code', AuxCode => 'aux-code', auxcode => 'aux-code', se => 'set-err', err => 'set-err',
    sf => 'sef-R1-file', r => 'range', e => 'export', i => 'individual', a => 'define-alias',
    da => 'define-alias', alias => 'define-alias', dali => 'define-alias', o => 'save-to', st => 'save-to',
    to => 'save-to', ro => 'remove-outliers', lx => 'logx', log-x => 'logx', xlog => 'logx', loglin => 'logx',
    ly => 'logy', log-y => 'logy', ylog => 'logy', linlog => 'logy', lxy => 'logxy', log-xy => 'logxy',
    xylog => 'logxy', loglog => 'logxy', ax => 'autox', auto-x => 'autox', ay => 'autoy', auto-y => 'autoy',
    axy => 'autoxy', auto-xy => 'autoxy', z => 'zip-to', zt => 'zip-to', mpeg4 => 'mp4', pc => 'print-cols',
    prco => 'print-cols', print-columns => 'print-cols', cols => 'print-cols', rd => 'use-ramdisk',
    ram => 'use-ramdisk', ramdisk => 'use-ramdisk', RAMDisk => 'use-ramdisk', ar => 'archive',
    wf => 'work-folder', sds => 'selected-dataset', selected-datasets => 'selected-dataset';

#| An option's canonical name: "--g" is "global", "--/plot" "plot", "--#a=2" "#a".
sub option-key(Str $opt) is export {
    my $k = $opt.subst(/^ '-'+ '/'? /, '').split('=', 2)[0];
    %canonical{$k} // $k
}

# Options a jobs-file line can't set for itself: where the fit's files go
# and how the batch runs belong to the whole batch, and --define-alias,
# --export and --archive aren't for parallel fits at all.
my constant %line-refused = set <path work-folder jobs use-ramdisk save-to zip-to define-alias export archive>;
# The three fit modes: one choice - a line naming any of them replaces all
# three of the batch's for that fit.
my constant %fit-modes = set <global hybrid individual>;

#| One jobs-file line: TAB-separated fields, the first a saved .json/.sav
#| or a model, the others its data files and - fields starting with "--" -
#| its own options. Relative paths are relative to the jobs file's folder.
sub parse-jobs-line(Str $line, Str $dir) is export {
    my &resolve = -> $p { $p.IO.is-absolute || $dir eq '.' || $dir eq '' ?? $p !! $dir.IO.add($p).Str };
    my ($first, @data, @opts);
    for $line.split("\t").kv -> $i, $f is copy {
        $f .= trim;
        if $f eq '' { next }
        elsif $i > 0 && $f.starts-with('--') { @opts.push($f) }
        elsif !$first.defined { $first = $f }
        else { @data.push(resolve($f)) }
    }
    my %j;
    if $first.defined && is-saved-fit($first) {
        die "a saved fit (.json/.sav) holds its own data - no data files after it: {$line.raku}" if @data;
        %j = kind => 'saved', model => Str, data => [resolve($first)];
    }
    elsif !$first.defined || !@data {
        die "a line is a saved fit (.json/.sav) or a model and its data files, separated by TABs, then its own --options: {$line.raku}";
    }
    else {
        %j = kind => 'line', model => $first, data => [@data];
    }
    my %modes;
    for @opts -> $o {
        my $k = option-key($o);
        die "--$k is for the whole batch, not one line: {$line.raku}" if %line-refused{$k};
        if %fit-modes{$k} {
            die "choose a fit mode with --$k alone: {$line.raku}" if $o.starts-with('--/') || $o ~~ / '=' [false|0] $ /;
            %modes{$k} = True;
        }
    }
    die "one fit mode per line - --global or --individual: {$line.raku}" if !%modes<hybrid> && %modes<global> && %modes<individual>;
    %j<options> = [@opts];
    %j<mode> = %modes<hybrid> ?? 'hybrid' !! %modes<global> ?? 'global' !! %modes<individual> ?? 'individual' !! Str;
    %j
}

#| The options one fit runs with: the batch's, except those its line sets
#| itself (all three fit modes when it chooses one), then the line's.
#| --individual is left out for a model and its data: a fresh fit is
#| individual unless global, and the fresh fit has no such option.
sub fit-options(@batch, %j) is export {
    my %line = (%j<options> // []).map({ option-key($_) => True });
    my @out = @batch.grep: { my $k = option-key($_); !%line{$k} && !(%j<mode> && %fit-modes{$k}) };
    @out.append: (%j<options> // []).grep({ %j<kind> eq 'saved' || option-key($_) ne 'individual' });
    @out
}

sub stem(Str $p) { $p.IO.basename.subst(/ '.' <-[.]>* $/, '') }

#| A saved fit's base name (run1), else the model's alias - or "fit" for a
#| full expression - and the first data file (2exp-d1); repeated names get
#| -2, -3.
sub name-fit-jobs(@jobs) {
    my %used;
    for @jobs -> %j {
        my $name;
        if %j<kind> eq 'saved' {
            $name = stem(%j<data>[0]);
        }
        else {
            my $model = %j<model>.trim;
            if $model.starts-with('#') { $model = $model.substr(1).trim }
            elsif $model.starts-with('alias:') { $model = $model.substr(6).trim }
            else { $model = 'fit' }
            $name = $model ~ '-' ~ stem(%j<data>[0]);
        }
        $name ~= '-' ~ %j<mode> if %j<mode>;
        $name = $name.subst(/ <-[A..Z a..z 0..9 . _ \-]>+ /, '_', :g).subst(/ ^ '_'+ | '_'+ $ /, '', :g);
        $name = 'fit' unless $name;
        %used{$name}++;
        $name ~= "-%used{$name}" if %used{$name} > 1;
        %j<name> = $name;
    }
}

#| The arguments as jobs, reading jobs files. Dies only on a jobs file it
#| cannot read or a malformed line; a missing saved fit or data file is
#| found when that fit runs (a failed fit, not an error here).
sub parse-fit-jobs(@pos) is export {
    my @jobs;
    my &add = -> %j { %j<id> = @jobs.elems + 1; @jobs.push(%j) };
    for @pos.map(~*) -> $a {
        if is-jobs-file($a) {
            my $file = $a.substr(1);
            die "jobs file $file: no such file" unless $file.IO.f;
            my $n = 0;
            for $file.IO.lines -> $raw {
                $n++;
                my $line = $raw.subst(/ \r $ /, '');
                next if $line.trim eq '' || $line.trim.starts-with('#!');
                my $where = "$file:$n";
                my %j = parse-jobs-line($line, $file.IO.dirname);
                CATCH { default { die "$where: { .message }" } }
                %j<arg> = $where;
                add(%j);
            }
        }
        elsif is-packed-job($a) {
            my @parts = split-packed($a);
            add(%( arg => $a, kind => 'packed', model => @parts[0], data => [@parts[1..*]] ));
        }
        else {
            add(%( arg => $a, kind => 'saved', model => Str, data => [$a] ));
        }
    }
    name-fit-jobs(@jobs);
    @jobs
}

# Options the launcher handles itself: never passed to the fits as given.
my constant %own = set <wf work-folder jobs rd ram ramdisk RAMDisk use-ramdisk>;
my constant %save-keys = set <o st save-to>;
my constant %zip-keys = set <z zt zip-to>;
my constant %refused = a => '--define-alias', da => '--define-alias', define-alias => '--define-alias', alias => '--define-alias',
                       e => '--export', export => '--export', ar => '--archive', archive => '--archive';

#| The options every fit gets (from MAIN's named arguments), the
#| --save-to/--zip-to patterns (must contain {name}); refused options die.
sub batch-options(%named) is export {
    my (@opts, $save-to, $zip-to, @problems);
    for %named.keys.sort -> $k {
        my $v = %named{$k};
        if %own{$k} { next }
        if %save-keys{$k} || %zip-keys{$k} {
            my $flag = %save-keys{$k} ?? '--save-to' !! '--zip-to';
            if %zip-keys{$k} && $v !~~ Bool && $v eq '' { @opts.push('--zip-to='); next }   # no zip, as for one fit
            if $v ~~ Bool || $v eq '' { @problems.push("$flag needs a value: a pattern with \{name}, e.g. $flag=\{name}{ %save-keys{$k} ?? '.json' !! '.zip' }"); next }
            if %save-keys{$k} { $save-to = ~$v } else { $zip-to = ~$v }
            next;
        }
        if %refused{$k} {
            @problems.push("%refused{$k} is about one fit, not a batch of independent fits - run that fit on its own");
            next;
        }
        next if $k.starts-with('#');
        @opts.push: $v ~~ Bool ?? ($v ?? "--$k" !! "--/$k") !! "--$k=$v";
    }
    for ('--save-to', $save-to), ('--zip-to', $zip-to) -> ($flag, $val) {
        @problems.push("$flag=$val would make every fit write the same file - give a pattern with \{name}, e.g. $flag=\{name}{ $val.IO.extension ?? '.' ~ $val.IO.extension !! '' }")
            if $val.defined && $val ne '' && !$val.contains('{name}');
    }
    # the model's own --#name[=value] options, as given
    for %named.keys.grep(*.starts-with('#')).sort -> $k {
        my $v = %named{$k};
        @opts.push: $v ~~ Bool ?? "--$k" !! "--$k=$v";
    }
    die @problems.sort.join('; ') if @problems;
    %( opts => @opts, save-to => $save-to, zip-to => $zip-to )
}

sub stamp() { DateTime.now(:timezone(0)).truncated-to('second').Str }

#| <base>/batch-YYYYmmdd-HHMMSS[-N], created, unique.
sub new-batch-folder(Str $base --> IO::Path) {
    $base.IO.mkdir;
    my $d = DateTime.now;
    my $stamp = sprintf('batch-%04d%02d%02d-%02d%02d%02d', $d.year, $d.month, $d.day, $d.hour, $d.minute, $d.whole-second);
    for 1 .. * -> $n {
        my $dir = $base.IO.add($n > 1 ?? "$stamp-$n" !! $stamp);
        next if $dir.e;
        return $dir if run('mkdir', ~$dir, :err).so;   # not -p: fails if another batch made it first
        die "cannot create $dir" unless $dir.e;
    }
}

#| The last lines of a fit's output, for its error: without the timing
#| notes and the dashed rules around messages.
sub last-lines(Str $s, Int $n) {
    my @lines = $s.lines.map(*.trim).grep({ .chars && !/^ '-'+ $/ && !/^ '===> ' ['Compile time' | 'Total execution time'] / });
    @lines ?? @lines.tail($n).join("\n") !! 'no output'
}

sub walk(IO::Path $d) {
    gather for $d.dir.sort(*.basename) -> $f {
        if $f.d && !$f.l { .take for walk($f) }
        elsif $f.f { take $f }
    }
}

#| The fit's files where the engine actually put them (its work subfolder
#| is <fit folder>/<input name or ofe-tmp or zip-to name>), relative to the
#| batch folder; a file not found is Nil.
sub find-artifacts(IO::Path $fit-dir, Str $abs-batch, $save-to, Str $name) {
    my %a = result => Any, zip => Any, pdf => Any, mp4 => Any, plots => Any;
    my &rel = { .relative($abs-batch) };
    my @files = walk($fit-dir);
    my @pdfs = @files.grep(*.basename eq 'All.pdf');
    my @mp4s = @files.grep(*.basename eq 'All.mp4');
    my @agrs = @files.grep({ .basename.starts-with('fit-curves-') && .basename.ends-with('.agr') });
    my @zips = @files.grep({ .parent.absolute eq $fit-dir.absolute && .basename.ends-with('.zip') });
    %a<zip> = rel(@zips[0]) if @zips;
    %a<pdf> = rel(@pdfs[0]) if @pdfs;
    %a<mp4> = rel(@mp4s[0]) if @mp4s;
    %a<plots> = @agrs ?? rel(@agrs[0].parent) !! @pdfs ?? rel(@pdfs[0].parent) !! Any;
    if $save-to {
        my $p = $fit-dir.add($save-to.subst('{name}', $name, :g).IO.basename);
        %a<result> = rel($p) if $p.e;
    }
    %a
}

#| Stops a fit and everything it started: its process group.
sub terminate-group(Int $pid) {
    run 'perl', '-e', 'kill "-TERM", $ARGV[0]', $pid, :err;
    start { sleep 3; run 'perl', '-e', 'kill "-KILL", $ARGV[0]', $pid, :err }
}

#| Runs the jobs as separate `@command fit ...` processes (@command: the
#| raku executable and this program), $jobs at a time (0: CPUs), in a new
#| batch folder made in $base. $explicit-workers: --workers/--no-parallel
#| were given (the fits get those instead of the CPU share). Returns the
#| exit status: 0 all done, 1 any failed, 130 stopped.
sub run-parallel-fits(@jobs, :@opts, :$save-to, :$zip-to, Str :$base = '.', Int :$jobs = 0,
                      Bool :$explicit-workers = False, Bool :$quiet = False, :@command!, :%env = %*ENV,
                      Str :$engine = 'onefite-raku' --> Int) is export {
    my $cpus = $*KERNEL.cpu-cores;
    my $par = min($jobs > 0 ?? $jobs !! $cpus, @jobs.elems);
    my $workers = $explicit-workers ?? 0 !! max(1, $cpus div $par);
    my $batch = new-batch-folder($base);
    my $abs = $batch.absolute;
    my $width = @jobs.map(*<name>.chars).max;

    my %man = version => 1, engine => $engine, started => stamp(), finished => Any, state => 'running',
              jobs => $par, workers => $workers, options => [@opts],
              fits => [ @jobs.map: -> %j { %(
                  id => %j<id>, arg => %j<arg>, kind => %j<kind>, model => %j<model> // Any, data => [|%j<data>],
                  options => [|(%j<options> // [])], mode => %j<mode> // Any,
                  name => %j<name>, folder => %j<name>, state => 'queued', exit => Any, error => Any,
                  started => Any, finished => Any, seconds => Any, chi2 => Any,
                  result => Any, zip => Any, pdf => Any, mp4 => Any, plots => Any,
                  log => "%j<name>/onefite.log",
              ) } ];
    my $lock = Lock.new;   # %man, the console, the running fits
    my &write = {
        my $tmp = $batch.add('.batch.json.tmp');
        $tmp.spurt(to-json(%man, :sorted-keys) ~ "\n");
        $tmp.rename($batch.add('batch.json'));
    };
    my &say-line = -> $line { $lock.protect({ say $line }) unless $quiet };
    $lock.protect(&write);
    say-line("===> {@jobs.elems} fits, $par at once ({ $explicit-workers ?? 'their own workers' !! "$workers worker{ $workers == 1 ?? '' !! 's' } each" }), in $batch");

    my %running;         # id => pid
    my $stopped = False;
    my $sig = signal(SIGINT, SIGTERM).tap: {
        $lock.protect: { $stopped = True; terminate-group($_) for %running.values }
    };

    sink @jobs.race(:batch(1), :degree($par)).map: -> %j {
        my $i = %j<id> - 1;
        my &set = -> &fn { $lock.protect({ fn(%man<fits>[$i]); write() }) };
        my $fit-dir = $batch.add(%j<name>);
        my $log = $fit-dir.add('onefite.log');
        my @argv = |@command, 'fit', "--work-folder=$fit-dir.absolute()", |fit-options(@opts, %j);
        @argv.push("--save-to=" ~ $fit-dir.add($save-to.subst('{name}', %j<name>, :g).IO.basename).absolute) if $save-to;
        @argv.push("--zip-to=" ~ $fit-dir.add($zip-to.subst('{name}', %j<name>, :g).IO.basename).absolute) if $zip-to;
        @argv.push(%j<model>) unless %j<kind> eq 'saved';
        @argv.append(|%j<data>);
        my %child-env = %env;
        %child-env<ONEFITE_WORKERS> = $workers if $workers > 0;
        unless $lock.protect({ $stopped }) {
            $fit-dir.mkdir;
            my $t0 = now;
            # perl: the fit in its own process group (so stopping it also
            # stops the compilers and onefit-user runs it started), its
            # output in its onefite.log
            my $proc = Proc::Async.new('perl', '-e',
                'setpgrp(0,0); open(STDOUT, ">", shift) or die "$!\n"; open(STDERR, ">&STDOUT"); exec @ARGV or die "could not start $ARGV[0]: $!\n"',
                ~$log, |@argv);
            my $done = $proc.start(:ENV(%child-env));
            my $pid = await $proc.pid;
            $lock.protect: { %running{%j<id>} = $pid; terminate-group($pid) if $stopped };
            set(-> %f { %f<state> = 'running'; %f<started> = stamp() });
            my $res = await $done;
            $lock.protect({ %running{%j<id>}:delete });
            my $secs = (now - $t0).Num;
            my $text = $log.e ?? $log.slurp(:enc<utf8-c8>) !! '';
            my @m = $text.match(/ 'tchi2' \s* '=' \s* (<[-+0..9.eE]>+) /, :g);
            my $chi2 = @m ?? ~@m[*-1][0] !! Any;
            my %a = find-artifacts($fit-dir, $abs, $save-to, %j<name>);
            my $state = $lock.protect({ $stopped }) ?? 'stopped' !! $res.exitcode == 0 ?? 'done' !! 'failed';
            my $error = $state eq 'failed' ?? last-lines($text, 3) !! Any;
            set(-> %f {
                %f<state exit error finished seconds chi2> = $state, $res.exitcode, $error, stamp(), $secs, $chi2;
                %f{$_} = %a{$_} for <result zip pdf mp4 plots>;
            });
            my $line = sprintf("     %-{$width}s  %-7s %6.1f s", %j<name>, $state, $secs);
            $line ~= "  chi2 $chi2" if $chi2.defined;
            $line ~= "  - " ~ $error.subst("\n", ' | ', :g) if $error.defined;
            say-line($line);
        }
    }
    $sig.close;

    $lock.protect: {
        my ($done, $failed, $halted) = 0, 0, 0;
        for |%man<fits> -> %f {
            given %f<state> {
                when 'queued' | 'running' { %f<state> = 'stopped'; $halted++ }
                when 'stopped' { $halted++ }
                when 'failed'  { $failed++ }
                when 'done'    { $done++ }
            }
        }
        %man<finished> = stamp();
        %man<state> = $stopped ?? 'stopped' !! $failed ?? 'failed' !! 'done';
        write();
        say "===> $done done, $failed failed, $halted stopped - { $batch.add('batch.json') }" unless $quiet;
        note "onefite: { $stopped ?? 'the batch was stopped' !! "$failed of {@jobs.elems} fits failed" }" if $stopped || $failed;
        $stopped ?? 130 !! $failed ?? 1 !! 0
    }
}
