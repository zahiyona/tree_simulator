#!/usr/bin/perl  
# This script classifies trees into an equivalent relation based on agreement. 
# the variable $tree_type holds the type of trees that are compared (JK, exons etc.)
# It needs to be set according to the names of the files produced at prior steps.


use strict;
use warnings;
my  @l4 = (1,2,3,4);
my  @l5 = (3,2,3,4);
my $a1 = \@l5;
my $a2 = \@l5;
#print "$a1 $a2";
my $rr = "(". join ("," ,@l4) . ")";
if ($#ARGV < 0) {die "must supply a quartet file, \n";}
my $fquartets = $ARGV[0];
my $output_path = "mrp-mat.dat";
my $time_limit = 5;
if ($#ARGV >= 1)
{
    $time_limit = $ARGV[1];
}
if ($#ARGV >= 2)
{
    $output_path = $ARGV[2];
}
open (FQR, "$fquartets") or die "cannot open fiel $fquartets\n";
my $line  = <FQR>;
my @quartets = split(/\s+/,$line);
my %comp;    

foreach my $qr (@quartets){
#  print "qr $qr\n";
  if ($qr !~ /(\d+),(\d+)\|(\d+),(\d+)/){
    print "invalid qrt $qr\n";
    exit;
  }
  else{
    if (($1 eq $2) || ($1 eq $3) || ($2 eq $3) || ($1 eq $4) || ($2 eq $4) || ($3 eq $4)){
      print "ERROR ERROR!!! not all species are distincst $qr\n";
      exit;
    }
    foreach my $aa ($1, $2 , $3, $4){
#      print "checking for $aa\n";
      if (exists ($comp{$aa})){
	#		print "element $aa already exists\n";
      }
      else {
	#		print "element $aa not exists. adding\n";
	$comp{$aa} = 1;
      }
    }
  }
}

my %mrpMat;
my @ele = sort {$a <=> $b} (keys %comp);
my $n = $#ele + 1;
foreach my $qr (@quartets){
#  print "qr $qr\n";
  if ($qr !~ /(\d+),(\d+)\|(\d+),(\d+)/){
    print "invalid qrtt $qr\n";
    exit;
  }
  else{
    foreach my $e (@ele){
      if ($e eq $1){
	$mrpMat{$e} .= "1";
      }
      elsif ($e eq $2){
	$mrpMat{$e} .= "1";
      }
      elsif ($e eq $3){
	$mrpMat{$e} .= "0";
      }
      elsif ($e eq $4){
	$mrpMat{$e} .= "0";
      }
      else{
	$mrpMat{$e} .= "?";
      }
    }
  }
}


my $kk = $ele[0];
my $len = length($mrpMat{$kk});
print ">$output_path";
open (MRPF,">$output_path");
print MRPF "#nexus\n".
    "begin data;\n".
    "dimensions ntax=".(@ele) ." nchar=$len;\n".
    "format symbols=\"01\" missing=?;\n".
    "matrix\n";

foreach my $e (keys %mrpMat){
    if (length($mrpMat{$e}) ne  $len){
	die "length of $mrpMat{$e} differ from $len\n";
    }
    print MRPF "$e\t\t$mrpMat{$e}\n";
}
print MRPF ";\n".
    "end;\n\n".
    "begin paup;\n\n".
    "set Warnreset=no Autoclose=yes Notifybeep=no MaxTrees=200 Increase=Auto;\n".
    "hsearch NReps=2 Addseq=random TimeLimit=$time_limit;\n".
    "contree all/  strict=no majrule=yes  TreeFile=../../../PAUP.out replace=yes;\n" .
    "quit WarnTSave=No;\n\n".
    "end;\n";

close MRPF;

