
# -* perl *-
# the location of perl binary will be set by "configure"
# first the basic tests

$num_cases = 29;

$case = shift;
if ($case) {
  $print_detailed = 1;

  if (!($case eq "all")) {
    do {
      push @test_cases, $case;
    } while ($case = shift);
    reverse @test_cases;
  }
}
if (!@test_cases[0]) {
  for ($i = 1; $i <= $num_cases; $i++) {
    push @test_cases, $i;
  }
}

@test_args = ( "-d n", "-d a", "-d f", "-d n -t", "-d a -t", "-d f -t" );

%result_files = (
    "-d n" => "smodels.none",
    "-d a" => "smodels.all",
    "-d f"=> "smodels.facts",
    "-d n -t" => "text.none",
    "-d a -t"=> "text.all",
    "-d f -t" => "text.facts" );

foreach $i (@test_cases) {
    print "Checking t$i.lp...";
    $this_error = 0;
    foreach $arg (@test_args) {
	`../src/lparse $arg t$i.lp > tmpfile`;
	`diff tmpfile results/t$i.$result_files{$arg}`;
	if ($?) {
	    $errors = 1;
	    $this_error = 1;
	    if ($print_detailed) {
		printf("\n$result_files{$arg}...");
		if ($this_error) {
		    print "fail\n";
		} else {
		    print "pass\n";
		}
	    }
	}
    }
	
    if ($this_error && !$print_detailed) {
	print "fail\n";
    } else {
	print "pass\n";
    }
}

#other tests
print "Checking hashing.lp...";
$this_error = 0;
`../src/lparse -t -D hashing.lp > tmpfile`;
`diff tmpfile results/hashing.text`;
if ($?) {
    $errors = 1;
    $this_error = 1;
}

if ($this_error) {
    print "fail\n";
} else {
    print "pass\n";
}

unlink("tmpfile");
    
exit($errors);
