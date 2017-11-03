#!/usr/bin/perl
my $paup_mat_path = "mrp-mat.dat";
my $output_path = "MRPtree.dat";
my $paup_dir_path = ".";

if ($#ARGV >= 0)
{
    $paup_mat_path = $ARGV[0];
}
if ($#ARGV >= 1)
{
    $output_path = $ARGV[1];
}
if ($#ARGV >= 2)
{
    $paup_dir_path = $ARGV[2];
}

my $cmd = "echo n |$paup_dir_path/paup4a158_ubuntu64 $paup_mat_path";
print "$cmd\n";
`$cmd`;

open (MRPOUT,"PAUP.out") or die "MRP OUTPUT was not found\n";
my ($line1, $MRPtree);
while ($line1 = <MRPOUT>){
#    print "\n\n$line1";
    if ($line1 =~ /tree MajRule.+=.+ (.+);/){
	$MRPtree = $1;
	last;
    }
}
if (!defined($MRPtree)){
    die "MRP tree was not found on output file!!!!\n";
}
open (MRPTREE,">$output_path");
print MRPTREE "$MRPtree".";\n";
close MRPTREE;


