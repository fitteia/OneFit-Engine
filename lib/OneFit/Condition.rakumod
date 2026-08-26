unit module OneFit::Condition;

class X::OneFit::Condition is Exception is export {
    has Str $.condition is required;

    method message() {
        "Invalid OneFit condition: '$!condition'"
    }
}

class CompiledCondition is export {
    has Str $.source is required;
    has Code $.evaluator is required;

    method evaluate(*%columns --> Bool:D) {
        so $!evaluator(%columns)
    }
}

# Precedence, loosest to tightest: or > and > not > comparison (non-
# chaining) > add/sub > mul/div/mod > unary +/- > ** (right-associative,
# binds TIGHTER than unary minus - "-2**2" is -4, not 4, matching both
# onefite-native's own rust/grammar/src/condition.rs and ordinary math/
# Python semantics) > primary (number/variable/parenthesized
# sub-expression, where the parens can wrap anything back up to a full
# `expression` - so both "(c1<2)" and "(c1+1)" are valid primaries).
grammar ConditionGrammar {
    token TOP        { ^ <.ws> <expression> <.ws> $ }
    rule expression  { <or-expression> }
    rule or-expression  { <and-expression> [ <or-op> <and-expression> ]* }
    rule and-expression { <not-expression> [ <and-op> <not-expression> ]* }
    # [ <not-op> ]* , NOT a bare <not-op>* - a `rule`'s automatic
    # whitespace-eating (sigspace) applies BETWEEN elements of a
    # bracketed sequence/group, including between successive
    # repetitions of a quantified GROUP, but not between repetitions of
    # a directly-quantified single token atom - confirmed live: a bare
    # <not-op>* left "not not c1" unmatched (only the first "not"
    # consumed, then the required trailing whitespace before the second
    # "not" was never eaten), while wrapping the same token in its own
    # single-element group fixes it.
    rule not-expression { [ <not-op> ]* <comparison> }
    rule comparison  { <add-expression> [ <comparison-op> <add-expression> ]? }
    rule add-expression { <mul-expression> [ <add-op> <mul-expression> ]* }
    rule mul-expression { <unary-expression> [ <mul-op> <unary-expression> ]* }
    rule unary-expression { [ <unary-op> ]* <pow-expression> }
    rule pow-expression { <primary> [ <pow-op> <unary-expression> ]? }
    rule primary     { <number> | <variable> | <function-call> | '(' <expression> ')' }
    rule function-call { <function-name> '(' <expression> ')' }

    token or-op         { 'or'  <!before <[\w]>> | '||' }
    token and-op        { 'and' <!before <[\w]>> | '&&' }
    token not-op        { 'not' <!before <[\w]>> | '!' <!before '='> }
    token comparison-op { '<=' | '>=' | '==' | '!=' | '<' | '>' }
    token add-op        { <[+\-]> }
    token mul-op        { '*' <!before '*'> | '/' | '%' }
    token unary-op      { <[+\-]> }
    token pow-op        { '**' }
    # Real onefite's own condition mechanism is literally Perl `eval()`,
    # so `abs(...)` is a real, legal expression there - not part of
    # onefite-native's own rust/grammar/src/condition.rs (which supports
    # no function calls at all), added here as a deliberate, small
    # extension beyond that reference scope. Kept as one named
    # alternative (rather than a generic identifier-plus-args rule) so
    # an unrecognized function name fails to parse cleanly instead of
    # silently matching as something else.
    token function-name { 'abs' }
    token variable      { 'c' <[1..3]> <!before <[\w]>> }
    token number        { [ \d+ [ '.' \d* ]? | '.' \d+ ] [ <[eE]> <[+\-]>? \d+ ]? }
}

class ConditionActions {
    method TOP($/)        { make $<expression>.made }
    method expression($/) { make $<or-expression>.made }

    method number($/) {
        my $value = +$/.Str;
        make -> %columns { $value };
    }

    method variable($/) {
        my $name = $/.Str;
        make -> %columns {
            die "Missing data column $name" unless %columns{$name}:exists;
            +%columns{$name};
        };
    }

    method function-call($/) {
        my $name = $<function-name>.Str;
        my $arg = $<expression>.made;
        make -> %columns {
            given $name {
                when 'abs' { abs($arg(%columns)) }
            }
        };
    }

    method primary($/) {
        make $<number> ?? $<number>.made
            !! $<variable> ?? $<variable>.made
            !! $<function-call> ?? $<function-call>.made
            !! $<expression>.made;
    }

    method pow-expression($/) {
        my $base = $<primary>.made;
        unless $<pow-op> {
            make $base;
            return;
        }
        my $exponent = $<unary-expression>.made;
        make -> %columns { $base(%columns) ** $exponent(%columns) };
    }

    method unary-expression($/) {
        my $operand = $<pow-expression>.made;
        # Cumulative sign over every prefix +/- (handles "--c1"/"-+c1"
        # the same way repeated unary minus/plus always would) -
        # applied to the WHOLE pow-expression below it, so unary minus
        # binds looser than **: "-2**2" negates 2**2 (4), giving -4, not
        # (-2)**2 (4) - matches condition.rs's own unary()/pow_expr()
        # call order exactly.
        my $sign = [*] $<unary-op>.map({ $_.Str eq '-' ?? -1 !! 1 });
        make $sign == 1
            ?? $operand
            !! -> %columns { $sign * $operand(%columns) };
    }

    method mul-expression($/) {
        my @operands = $<unary-expression>.map(*.made);
        my @ops = $<mul-op>».Str;
        make -> %columns {
            my $acc = @operands[0](%columns);
            for @ops.kv -> $i, $op {
                my $rhs = @operands[$i + 1](%columns);
                given $op {
                    when '*' { $acc *= $rhs }
                    when '/' {
                        die "Division by zero" if $rhs == 0;
                        $acc /= $rhs;
                    }
                    when '%' { $acc %= $rhs }
                }
            }
            $acc;
        };
    }

    method add-expression($/) {
        my @operands = $<mul-expression>.map(*.made);
        my @ops = $<add-op>».Str;
        make -> %columns {
            my $acc = @operands[0](%columns);
            for @ops.kv -> $i, $op {
                my $rhs = @operands[$i + 1](%columns);
                $acc = $op eq '+' ?? $acc + $rhs !! $acc - $rhs;
            }
            $acc;
        };
    }

    method comparison($/) {
        my $left = $<add-expression>[0].made;
        unless $<comparison-op> {
            # Deliberately NOT boolean-coerced here (matches
            # condition.rs's own cmp_expr, which returns its lhs
            # UNCHANGED when no comparison operator follows) - a bare
            # add-expression used as a numeric subexpression (inside
            # parens, or as abs()'s argument) needs to stay numeric so
            # an enclosing arithmetic context still sees its real value,
            # e.g. "(c1+1)==5" or "abs(c1-c2)==3". Truthiness is only
            # ever applied where a value is actually being combined as a
            # boolean - not-expression/and-expression/or-expression
            # below - via Raku's own Num.Bool (0 is false, any nonzero
            # is true), the same semantics condition.rs's own truthy()
            # helper implements by hand.
            make $left;
            return;
        }

        my $right = $<add-expression>[1].made;
        my $operator = $<comparison-op>.Str;
        make -> %columns {
            my $a = $left(%columns);
            my $b = $right(%columns);
            given $operator {
                when '<'  { $a <  $b }
                when '<=' { $a <= $b }
                when '>'  { $a >  $b }
                when '>=' { $a >= $b }
                when '==' { $a == $b }
                when '!=' { $a != $b }
            }
        };
    }

    method not-expression($/) {
        my $operand = $<comparison>.made;
        my $count = $<not-op>.elems;
        # No not/! at all: pass the operand through UNCHANGED, same as
        # comparison's own bare case above and for the same reason - a
        # bare comparison result may still be a raw numeric subexpression
        # (e.g. the "c1-c2" inside "abs(c1-c2)==3" passes through
        # add-expression -> comparison -> not-expression -> ... with no
        # operator at ANY of these levels, and must stay numeric the
        # whole way). Only actually negating (odd count) or explicitly
        # double-negating (even count > 0, condition.rs's own not_expr
        # forces this to a canonical 0.0/1.0 too, never the original
        # magnitude - confirmed by tracing its recursive truthy() calls)
        # coerces to boolean.
        if $count == 0 {
            make $operand;
            return;
        }
        my $negated = $count % 2;
        make -> %columns {
            $negated ?? !$operand(%columns) !! so $operand(%columns)
        };
    }

    method and-expression($/) {
        my @operands = $<not-expression>.map(*.made);
        # Same passthrough rule as comparison/not-expression above: with
        # only one operand (no "and"/"&&" actually present), return it
        # UNCHANGED rather than forcing a boolean result - condition.rs's
        # own and_expr returns its single operand raw in exactly this
        # case (its loop simply never runs).
        if @operands == 1 {
            make @operands[0];
            return;
        }
        # Real, deliberate divergence from onefite-native's own
        # rust/grammar/src/condition.rs, not an oversight: that
        # reference implementation's or_expr/and_expr always evaluate
        # BOTH sides before combining them (`let rhs = self.and_expr()?;`
        # runs unconditionally), so it doesn't short-circuit either -
        # but genuine Perl `and`/`&&` DOES, and this condition mechanism
        # exists specifically to replace a real Perl eval() call, so
        # matching Perl's actual behavior here is more correct than
        # matching the Go port's own same gap. An eager
        # `@operands.map({...}).all` would evaluate every operand to
        # build the Junction before `so` ever looks at it, which is
        # exactly what silently turns "c1!=0 and 1/c1>2" into a division
        # by zero for c1=0 - a real bug a reviewer caught live. This
        # loop stops at the first false operand instead.
        make -> %columns {
            my $result = True;
            for @operands -> $op {
                unless so $op(%columns) {
                    $result = False;
                    last;
                }
            }
            $result;
        };
    }

    method or-expression($/) {
        my @operands = $<and-expression>.map(*.made);
        if @operands == 1 {
            make @operands[0];
            return;
        }
        # Short-circuits at the first true operand - see and-expression's
        # own doc comment above for why this deliberately diverges from
        # condition.rs's own non-short-circuiting or_expr.
        make -> %columns {
            my $result = False;
            for @operands -> $op {
                if so $op(%columns) {
                    $result = True;
                    last;
                }
            }
            $result;
        };
    }
}

sub compile-condition(Str:D $source --> CompiledCondition:D) is export {
    my $condition = $source.trim;
    my $match = ConditionGrammar.parse($condition, :actions(ConditionActions.new));
    X::OneFit::Condition.new(:$condition).throw unless $match;
    CompiledCondition.new(:source($condition), :evaluator($match.made));
}

