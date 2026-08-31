class Build {
    my constant @SITE-ROOTS = <include lib src>;
    my constant @REQUIRED-TOOLS = <
        afactors-xyz fft cop gfilt ccat pcop pdf2mp4 epstopdf
    >;
    my constant @REQUIRED-NATIVE = (
        'lib/libminuit.a',
        'lib/libuserlib.a',
        'include/globals.h',
        'include/mixed.h',
        'include/userlib.h',
    );

    # zef calls this hook in its build work directory.  Native compilation is
    # deliberately performed by INSTALL before zef is invoked; the hook only
    # records native files if that staged installation is present.
    method build($workdir) {
        self.write-site-manifest($workdir, :strict(False));
        True;
    }

    method write-site-manifest(
        IO() $workdir,
        IO() :$bindir,
        Str :$output = 'MANIFEST.site',
        Bool :$strict = False,
        --> IO::Path:D
    ) {
        my $root = $workdir.IO.absolute.IO;
        my @site-files;
        for @SITE-ROOTS -> $name {
            my $directory = $root.add($name);
            next unless $directory.d;
            @site-files.append: self!files-under($directory).map({
                .relative($root).Str
            });
        }

        my $bin-directory = $bindir.defined ?? $bindir.IO.absolute.IO !! Nil;
        if $bin-directory.defined && $bin-directory.d {
            @site-files.append: @REQUIRED-TOOLS.map({ $bin-directory.add($_) })
                .grep({ .f || .l }).map(*.absolute);
        }

        @site-files = @site-files.grep({
            self!is-native-site-file($_, :has-bindir($bin-directory.defined))
                && $_ ne $output
                # Externally-installed tool entries are absolute paths of exact,
                # already-known-good names from @REQUIRED-TOOLS - never a
                # .precomp/backup/object artifact, so these exclusions (aimed at
                # the relative in-tree walk above) must not run against them. An
                # unanchored '/.'-anywhere check would otherwise reject any tool
                # whose install root happens to sit under a dotted directory
                # (e.g. ~/.local/...), found live.
                && ( $_.IO.is-absolute || (
                    $_ !~~ / [ '.precomp' | '/.' <-[/]>+ ] /
                    && $_ !~~ / [ '-e' | '.bak' | '~' ] $/
                    && $_ !~~ / [ '.o' | '.obj' ] $/
                ) )
        }).unique.sort;

        if $strict {
            my @missing = @REQUIRED-NATIVE.grep({ !$root.add($_).f });
            my @library-names = $root.add('lib').d
                ?? $root.add('lib').dir.grep(*.f).map(*.basename)
                !! ();
            @missing.push: 'lib/libonefit-VERSION.a'
                unless @library-names.grep(/^ 'libonefit-' \d .* '.a' $/);
            @missing.push: 'lib/libonefit-modelos-VERSION.a'
                unless @library-names.grep(/^ 'libonefit-modelos-' .* '.a' $/);
            @missing.push: 'lib/libonefit-util-VERSION.a'
                unless @library-names.grep(/^ 'libonefit-util-' .* '.a' $/);
            if $bin-directory.defined {
                @missing.append: @REQUIRED-TOOLS.map({ $bin-directory.add($_) })
                    .grep({ !(.f || .l) }).map(*.absolute);
            }
            else {
                @missing.push: 'BINDIR (required to validate installed C tools)';
            }

            die "Native OneFit installation is incomplete; missing:\n  "
                ~ @missing.join("\n  ") ~ "\n"
                if @missing;
        }

        my $manifest = $root.add($output);
        $manifest.spurt(@site-files.join("\n") ~ (@site-files ?? "\n" !! ''));
        note "===> wrote $output ({@site-files.elems} native site files)";
        $manifest;
    }

    method !is-native-site-file(Str:D $path, Bool :$has-bindir --> Bool:D) {
        return True if $has-bindir && $path.IO.is-absolute;
        return True if $path.starts-with(any(<include/ src/>));
        return False unless $path.starts-with('lib/');
        so $path.ends-with(any(<.a .so .dylib .h .dat>));
    }

    method !files-under(IO::Path:D $directory --> Seq:D) {
        gather {
            sub walk(IO::Path:D $path) {
                for $path.dir.sort(*.Str) -> $entry {
                    next if $entry.basename eq any(<.git .precomp>);
                    $entry.d ?? walk($entry) !! take($entry) if $entry.f || $entry.d;
                }
            }
            walk($directory);
        }
    }
}
