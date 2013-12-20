#!/usr/bin/perl
use strict;
use warnings;
use Time::HiRes qw(usleep);
use Digest::MD5 qw(md5_hex);

if (!scalar @ARGV) {
    print "$0 <height of frame> <width of frame> [wait in seconds] [refresh 1/0]$/";
    print
        '   Reads from stdin in frames of a given height'.$/.
            '   clearing the screen between each frame. Piping'.$/.
            '   input in strongly recomended.'.$/;
    exit;    
}

my $height = 10;
$height = $1 if (scalar @ARGV > 0 && $ARGV[0] =~ /(\d+)/);
exit 1 unless $height;

my $width = 10;
$width = $1 if (scalar @ARGV > 1 && $ARGV[1] =~ /(\d+)/);
exit 2 unless $width;

my $wait = 0;
$wait = $1 if (scalar @ARGV > 2 && $ARGV[2] =~ /(\d*\.?\d+)/);
$wait = 0 unless $wait;

$wait *= 1000000;

my $refresh = 1;
$refresh = $1 if (scalar @ARGV > 3 && $ARGV[3] =~ /(\d*\.?\d+)/);

my $lastRun = 0;
my $compare = 0;
while (1)
{   
    my @lines;
    for my $i(1..$height)
    {
        exit if eof STDIN;
        my $line = <STDIN>;
        chomp $line;
        
        push (@lines, "|$line|");
    }

    my $thisRun = md5_hex(join ('', @lines));

    if ($thisRun eq $compare)
    {
        sleep 1;
        exit;
    }

    $compare = $lastRun;
    $lastRun = $thisRun;

    system('clear') if $refresh;

    print '+'; print '-' for (1..$width); print "+".$/;
    print join ($/, @lines).$/;
    print '+'; print '-' for (1..$width); print "+".$/;

    usleep $wait;
}
