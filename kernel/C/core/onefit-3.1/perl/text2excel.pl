#!/usr/bin/perl

use Env qw(PATH HOME ONEFITROOT);
use lib "$ENV{ONEFITROOT}/OneFit/lib/perl5/site_perl/";

#use MIME::Lite;  # we want to mail our excel sheet

my $file = $ARGV[0];
#my $file     = '/data/profit_forcast';
my $name     = "$ARGV[0]";
my $xls_data = text_to_excel( file      => $file,
                              delimiter => "\\s+",
                              name      => $name );
print $xls_data;
exit;
# we've done all the work. $xls_data IS the excel file in a raw
# format. we could do anything with it now, including writing it to a
# file, but let's send it via email.

#my $msg = MIME::Lite->new(From    => 'traitor@sedition.com',
#                          To      => 'tuna@fish.net',
#                          Cc      => 'traitor@sedition.com',
#                          Subject => $name,
#                          Type    => 'multipart/mixed')
#    or die "PROBLEM opening MIME object: $!";
#
#$msg->attach(Type        => 'application/vnd.ms-excel',
#             Disposition => 'attachment',
#             Data        => $xls_data,
#             Filename    => $name . '.xls')
#    or die "PROBLEM attaching Excel file: $!";
#
#$msg->send() or die "PROBLEM sending MIME mail: $!";
#
#print "Sent $name!\n"



sub text_to_excel {
# %args should look something like...
# ( delimiter => "\t",
#   recordsep => "\n",
#        file => "/path/to/file.txt"
#        name => "Sheet Title" )
# the only required args are delimiter and file

# we require instead of use to save on if we never end up using it in
# a larger script or CGI, but use statements at the top of the script
# are clearer for other programmers to follow.
    require Spreadsheet::WriteExcel;
    require IO::Scalar;

    my %args = @_;
    my ( $delimiter, $recordsep, $file, $name ) = 
        @args{qw( delimiter recordsep file name )};

    $delimiter and $file or 
        die "Must provide at least delimiter and file as args to" .
            "delimited_text_to_excel().";

    -e $file or
        die "There is no file: $file\n";

    open F, "<", $file or croak("Can't open $file: $!");
    $/ = $recordsep || "\n";
    my @data = <F>;
    close F;
    $/ = "\n";

    my $xls_str;
    tie *XLS, 'IO::Scalar', \$xls_str;

    my $workbook  = Spreadsheet::WriteExcel->new(\*XLS);

    my $worksheet = $workbook->addworksheet($name||'Page 1');

    for ( my $row = 0; $row < @data; $row++ ) {

        chomp( my @line = split /$delimiter/, $data[$row] );

        for ( my $col = 0; $col < @line; $col++ ) {
            $worksheet->write_string($row, $col, $line[$col] || "");
        }
    }
    $workbook->close();
    return $xls_str;
}
