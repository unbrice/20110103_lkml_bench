#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t
times_two (uint64_t n)
{
  return n * 2;
}

int
main (int argc, char **argv)
{
  int cpu_number;
  clock_t from_tick, to_tick;
  long long steps_number;
  unsigned long total_time;

  if (argc != 3)
    errx (1, "Usage: %s ITERATIONS CPUS", argv[0]);
  steps_number = atoll (argv[1]);
  cpu_number = atoi (argv[2]);
  if (steps_number < 0)
    err (2, "ITERATIONS must be a positive number (%s)", argv[1]);
  if (cpu_number != 1 && cpu_number != 2)
    err (3, "CPUS must be equal to 1 or 2 (%s)", argv[2]);

  from_tick = clock ();
  for (unsigned n = 0; n < steps_number; ++n)
    {
      if (n * 2 != times_two (n))
	errx (1, "Wrong result !");
    }
  to_tick = clock ();

  total_time = (to_tick - from_tick) / (CLOCKS_PER_SEC / 1000);
  printf ("It took %s %ld ms to perform %s calls", argv[0], total_time,
	  argv[1]);
  if (total_time > 0)
    printf (" (%lld calls per second)\n", 1000 * steps_number / total_time);
  else
    putchar ('\n');
}
