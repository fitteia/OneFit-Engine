class Build {
    my constant @SITE-ROOTS = <C include lib src>;
    my constant @REQUIRED-NATIVE = (
        'lib/libminuit.a',
        'lib/libuserlib.a',
        'include/globals.h',
        'include/mixed.h',
        'include/userlib.h',
        'C/core/onefit-3.1/afactors-xyz',
        'C/core/onefit-3.1/fft',
        'C/core/onefit-3.1/gfilt',
        'C/local/libuserlib.a',
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

        @site-files = @site-files.grep({
            self!is-native-site-file($_)
                && $_ ne $output
                && $_ !~~ / [ '.precomp' | '/.' <-[/]>+ ] /
                && $_ !~~ / [ '-e' | '.bak' | '~' ] $/
                && $_ !~~ / [ '.o' | '.obj' ] $/
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

            die "Native OneFit installation is incomplete; missing:\n  "
                ~ @missing.join("\n  ") ~ "\n"
                if @missing;
        }

        my $manifest = $root.add($output);
        $manifest.spurt(@site-files.join("\n") ~ (@site-files ?? "\n" !! ''));
        note "===> wrote $output ({@site-files.elems} native site files)";
        $manifest;
    }

    method !is-native-site-file(Str:D $path --> Bool:D) {
        return True if $path.starts-with(any(<C/ include/ src/>));
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
