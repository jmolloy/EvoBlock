#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;

open my $fd, "<", "solutions/cgp.c" or die "Can't open solutions/cgp.c";
local $/ = undef;
my $src = <$fd>;
close $fd;

my %params = ('mutation_rate' => [2, 50, 8],
              'population_size' => [8, 64, 8]);

my $reps = 10;
my $gen_max = 100000;

my @tests;

# mutation_rate
for(my $i = $params{'mutation_rate'}[0]; $i <= $params{'mutation_rate'}[1]; $i += $params{'mutation_rate'}[2]) {

  for(my $j = $params{'population_size'}[0]; $j <= $params{'population_size'}[1]; $j += $params{'population_size'}[2]) {
    push @tests, {'mutation_rate' => $i, 'population_size' => $j};
  }
}

my $num_tests = scalar @tests;
print "$num_tests tests to perform, $reps times.\n";

my $i = 0;
foreach my $t (@tests) {
  print "$i / $num_tests\n";

  my $pop_size = $t->{'population_size'};
  my $mutation_rate = $t->{'mutation_rate'};

  my $_gen_max = int($gen_max / ($pop_size/8));

  my $src_cpy = $src;
  $src_cpy =~ s/#define POP_SIZE 8/#define POP_SIZE $pop_size/;
  $src_cpy =~ s/#define GEN_MAX 200000/#define GEN_MAX $_gen_max/;
  $src_cpy =~ s/#define MUTATION_RATE 6/#define MUTATION_RATE $mutation_rate/;

  open my $fd, ">", "solutions/cgp_test.c" or die "Can't open solutions/cgp_test.c";
  print $fd $src_cpy;
  close $fd;

  `scons`;
  $t->{'gens'} = [];
  for(my $j = 0; $j < $reps; $j++) {
    print "\t Repeat $j\n";
    my $output = `./evoBlockSim-cgp_test`;
    if ($output =~ m/Solution found/) {
      my ($gens) = $output =~ m/Gen: (\d+),/;
      push @{$t->{'gens'}}, $gens;
    } else {
      push @{$t->{'gens'}}, 0;
    }
  }

  print Dumper(\$t);

  $i++;
}

open my $fd2, ">", "results";
print $fd2 Dumper(\@tests);
