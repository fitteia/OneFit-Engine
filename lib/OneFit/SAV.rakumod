unit module OneFit::SAV;

# Native reader/writer for OneFit-Engine's CGI.pm .sav format:
# percent-encoded name=value records, one value per line, terminated by
# CGI.pm's bare "=" end marker.  This deliberately has no Perl dependency.

sub is-unreserved(UInt:D $byte --> Bool:D) {
    so (0x41 <= $byte <= 0x5A)
        || (0x61 <= $byte <= 0x7A)
        || (0x30 <= $byte <= 0x39)
        || $byte == 0x5F
        || $byte == 0x2E
        || $byte == 0x7E
        || $byte == 0x2D;
}

sub hex-value(UInt:D $byte) {
    given $byte {
        when 0x30..0x39 { $byte - 0x30 }
        when 0x41..0x46 { $byte - 0x41 + 10 }
        when 0x61..0x66 { $byte - 0x61 + 10 }
        default         { Nil }
    }
}

# Match CGI::Util::escape: operate on UTF-8 bytes; leave only
# [A-Za-z0-9_.~-] literal; encode everything else using uppercase %XX.
sub sav-escape(Str:D $value --> Str:D) is export {
    $value.encode('utf8').list.map(-> $byte {
        is-unreserved($byte)
            ?? $byte.chr
            !! sprintf('%%%02X', $byte)
    }).join;
}

# Match CGI::Util::unescape's leniency: '+' is accepted as a space and only
# well-formed %XX sequences are decoded.  A malformed '%' remains literal.
sub sav-unescape(Str:D $value --> Str:D) is export {
    my @input = $value.encode('utf8').list;
    my @output;
    my $i = 0;

    while $i < @input.elems {
        if @input[$i] == 0x2B {
            @output.push: 0x20;
            $i++;
        }
        elsif @input[$i] == 0x25 && $i + 2 < @input.elems {
            my $hi = hex-value(@input[$i + 1]);
            my $lo = hex-value(@input[$i + 2]);
            if $hi.defined && $lo.defined {
                @output.push: $hi * 16 + $lo;
                $i += 3;
            }
            else {
                @output.push: @input[$i];
                $i++;
            }
        }
        else {
            @output.push: @input[$i];
            $i++;
        }
    }

    Buf.new(@output).decode('utf8-c8');
}

# Parse SAV text.  A key seen once becomes a Str; a repeated key becomes an
# Array, preserving value order like a multi-valued CGI.pm param.
sub from-sav(Str:D $content --> Hash:D) is export {
    my %values;

    for $content.lines -> $line {
        last if $line eq '=';
        next unless $line.contains('=');

        my ($encoded-name, $encoded-value) = $line.split('=', 2);
        my $name = sav-unescape($encoded-name);
        next if $name eq '.cgifields';

        my $value = sav-unescape($encoded-value // '');
        if %values{$name}:exists {
            %values{$name} = [%values{$name}] unless %values{$name} ~~ Positional;
            %values{$name}.push: $value;
        }
        else {
            %values{$name} = $value;
        }
    }

    %values;
}

sub scalar-to-sav-str($value --> Str:D) {
    $value.defined ?? $value.Str !! '';
}

# Serialize a flat engine hash.  Positional values become repeated records;
# keys are sorted so native SAV output is reproducible.
sub to-sav(Associative:D $engine --> Str:D) is export {
    my @records;

    for $engine.keys.sort -> $name {
        my $escaped-name = sav-escape($name.Str);
        my $value = $engine{$name};

        if $value ~~ Positional {
            for $value.list -> $item {
                @records.push: $escaped-name ~ '='
                    ~ sav-escape(scalar-to-sav-str($item));
            }
        }
        else {
            @records.push: $escaped-name ~ '='
                ~ sav-escape(scalar-to-sav-str($value));
        }
    }

    @records.push: '=';
    @records.join("\n") ~ "\n";
}

sub read-sav(IO() $file --> Hash:D) is export {
    from-sav($file.IO.slurp(:enc<utf8-c8>, :close));
}

sub write-sav(IO() $file, Associative:D $engine --> Nil) is export {
    $file.IO.spurt(to-sav($engine), :enc<utf8-c8>);
}
