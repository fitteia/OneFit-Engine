unit module OneFit::Engine::MinuitStatus;

# Match OFE-Go's versioned diagnostics without changing MINUIT commands or
# results. Historical MIGRAD evidence is separate from later MINOS/final state.
sub matrix-from-report(@input) {
    my @lines;
    for @input -> $line {
        last if $line ~~ /^ \s* '**' \s* \d+ \s* '**'/;
        @lines.push: $line;
    }
    my $upper = @lines.join("\n").uc;
    my $source = (@lines.first({ .uc.contains('ERROR MATRIX') || .uc.contains('ERR MATRIX') }) // '').trim;
    my $state = 'unknown';
    if $upper.contains('NOT POS-DEF') || $upper.contains('NOT POSITIVE DEFINITE') || $upper.contains('NOT POSITIVE-DEFINITE') {
        $state = 'not-positive-definite';
    } elsif $upper.contains('FORCED POS-DEF') || $upper.contains('MADE POSITIVE') {
        $state = 'forced-positive-definite';
    } elsif $upper.contains('NO ERROR MATRIX') {
        $state = 'none';
    } elsif $upper.contains('APPROXIMATE') || $upper.contains('CURRENT GUESS') {
        $state = 'approximate';
    } elsif $upper.contains('ERROR MATRIX ACCURATE') {
        $state = 'accurate';
    } elsif $upper.contains('ERROR MATRIX UNCERTAINTY=') && $upper.contains('EXT PARAMETER') && !$upper.contains('MINOS ERRORS') {
        $state = 'accurate';
    }
    ($state, $source)
}

our sub minuit-status(Str $text --> Hash) is export {
    my %status = migrad => 'unknown', migradErrorMatrix => 'unknown', errorMatrix => 'unknown';
    my @lines = $text.lines;
    my @reports;
    for @lines.kv -> $i, $line {
        if $line ~~ / 'FROM' \s+ $<command>=(<[A..Z0..9]>+) \s+ 'STATUS=' \s* $<status>=(.*?) \s+ \d+ \s+ 'CALLS' / {
            @reports.push: { index => $i, command => ~$<command>, status => $<status>.Str.trim, line => $line.trim };
        }
    }
    my $last-migrad = -1;
    for @reports.kv -> $i, %report { $last-migrad = $i if %report<command> eq 'MIGRAD' }
    for @reports.kv -> $i, %report {
        my $end = $i+1 < @reports.elems ?? @reports[$i+1]<index> !! @lines.elems;
        my ($matrix, $source) = matrix-from-report(@lines[(%report<index>+1) ..^ $end]);
        if $i == $last-migrad {
            %status<migrad> = %report<status>;
            %status<migradLine> = %report<line>;
            %status<migradErrorMatrix> = $matrix;
        }
        if %report<command> eq 'MINOS' && $i > $last-migrad {
            %status<minos> = %report<status>;
            %status<minosLine> = %report<line>;
        }
        if $i == @reports.end {
            %status<finalCommand> = %report<command>;
            %status<finalStatus> = %report<status>;
            %status<errorMatrix> = $matrix;
            %status<errorMatrixLine> = $source if $source.chars;
        }
    }
    %status
}

our sub collect-fit-status(%engine, Str $path, @blocks, Bool :$mixed=False, Bool :$hybrid=False --> Hash) is export {
    my @status;
    my $global = !(%engine<FitType> // '').contains('Individual') || $hybrid;
    for @blocks -> $block {
        my $name = "fit{$block.No+1}.log";
        my $scope = 'block';
        if $global && (!$mixed || $hybrid) { $name = 'fit.log'; $scope = 'global' }
        my $text = (try "$path/$name".IO.slurp(:enc<latin-1>)) // '';
        my %status = minuit-status($text);
        %status<block> = $block.No + 1;
        %status<tag> = $block.Tag;
        %status<scope> = $scope;
        %status<log> = $name;
        @status.push: %status;
    }
    { version => 1, blocks => @status.Array }
}
