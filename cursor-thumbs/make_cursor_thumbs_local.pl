#!/bin/perl
#----------------------------------------------------------------------
# Description:
# Author: steev <>
# Created at: śro, 9 lut 2022, 14:07:26 CET
# Computer: shodan 
# System: Linux 5.14.16-arch1-1 on x86_64
#
# Copyright (c) 2022 steev  All rights reserved.
#
#----------------------------------------------------------------------
# Configure section:
use strict;
use warnings;
use Data::Dumper;
use File::Copy;

my @names = ("default","arrow","left_ptr","right_ptr");
my @folders = glob("~/.icons/*");
my @conf;
my $dir;
my $name;

mkdir "/tmp/cursor";
die "cannot create directory /tmp/dursor\n" unless -d "/tmp/cursor";

for $dir (@folders) {
    next unless -d $dir;
    next unless -d "$dir/cursors";
    next if -f "$dir/cursors/thumbnail.png";
    undef $name;
    for (@names) { 
	if ( -f "$dir/cursors/$_") { $name = "$dir/cursors/$_"; last; }
    }
    die "$dir" 	unless defined $name;
    `xcur2png -d /tmp/cursor -c /tmp/cursor/cursor.conf "$name" >/dev/null 2>&1`;
    open CONF,"</tmp/cursor/cursor.conf" or die "Cannot open conf file\n";
    $name = <CONF>;
    @conf = <CONF>;
    close CONF;
    @conf = split " ",$conf[$#conf];
    $name = $conf[3];
    copy("$name","$dir/cursors/thumbnail.png");
}
