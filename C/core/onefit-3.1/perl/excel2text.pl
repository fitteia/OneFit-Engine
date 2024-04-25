#!/usr/bin/perl 
use strict;
use warnings;
#===========================================================
#  DECLARATIONS
#===========================================================
use Env qw(PATH HOME ONEFIT_ROOT);
use lib "$ENV{ONEFITROOT}/OneFit/lib/perl5/site_perl/";
use Spreadsheet::ParseExcel;
#===========================================================
#  PROGRAM PROPER
#===========================================================
my $file_name = shift || usage();

parse_excel("\n"," ",$file_name);

exit 0;
#===========================================================
#  SUBROUTINES
#===========================================================
sub parse_excel {

    my %arg = ( record_sep => $_[0],
                field_sep  => $_[1],
                xls        => undef );
    shift;
    shift;
    if ( @_ == 1 ) {
        $arg{xls} = shift;
    } elsif ( not @_ % 2 ) {
        %arg = ( %arg, @_ );
    }
#   print %arg;
#    exit;
    -e $arg{xls} or 
        die "Must provide valid XLS file! $arg{xls}, $!\n";

# create a ParseExcel object
    my $excel_obj = Spreadsheet::ParseExcel->new();

    my $workbook = $excel_obj->Parse($arg{xls});

# make sure we're in business
    die "Workbook did not return worksheets!\n"
        unless ref $workbook->{Worksheet} eq 'ARRAY';

# we need to get each worksheet from the workbook in turn

    for my $worksheet ( @{$workbook->{Worksheet}} ) {

    # {Cells}[row][col]

        my $last_col = $worksheet->{MaxCol};
        my $last_row = $worksheet->{MaxRow};

        for my $row ( 0 .. $last_row ) {

            for my $col ( 0 .. $last_col ) {

                my $cell = $worksheet->{Cells}[$row][$col];

                print ref $cell ?
                    $cell->Value : '';

                print $arg{field_sep} unless $col == $last_col;
            }
            print $arg{record_sep}; # record ends
        }
        print "\n"; # worksheet ends
    }
}
#===========================================================
sub usage {

    my ( $tool ) = $0 =~ m,([^\/]+$),;

    print <<HERE;
------------------------------------------------------------
USAGE:

   $tool EXCEL_FILE.xls [field_delim] [record_delim]

Takes an Excel file, parses it into plain text delimited 
fields and rows, and sends the results to STDOUT. The default 
field and record delimiters are "\\t" and "\\n" if neither 
is given.
------------------------------------------------------------
HERE
    exit 0;
}
#===========================================================
