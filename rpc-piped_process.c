#define _GNU_SOURCE

#include <assert.h>
#include <err.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static void
force_cpu (int cpu)
{
  cpu_set_t cpu_set;
  CPU_ZERO (&cpu_set);
  CPU_SET (cpu, &cpu_set);
  sched_setaffinity (0, sizeof (cpu_set_t), &cpu_set);
}

int
main (int argc, char **argv)
{
  int arguments[2];
  int cpu_number;
  clock_t from_tick, to_tick;
  long long steps_number;
  int results[2];
  unsigned long total_time;

  if (argc != 3)
    errx (1, "Usage: %s ITERATIONS CPUS", argv[0]);
  steps_number = atoll (argv[1]);
  cpu_number = atoi (argv[2]);
  if (steps_number < 0)
    err (2, "ITERATIONS must be a positive number (%s)", argv[1]);
  if (cpu_number != 1 && cpu_number != 2)
    err (3, "CPUS must be equal to 1 or 2 (%s)", argv[2]);

  pipe (arguments);
  pipe (results);

  if (fork ())
    {
      if (cpu_number == 1)
	force_cpu (0);
      else if (cpu_number == 2)
	force_cpu (1);
      from_tick = clock ();
      for (unsigned n = 0; n < steps_number; ++n)
	{
	  uint64_t buf = n;
	  write (arguments[1], &buf, sizeof (buf));
	  read (results[0], &buf, sizeof (buf));
	  assert (buf == n * 2);
	}
      to_tick = clock ();
    }
  else
    {
      force_cpu (0);
      while (1)
	{
	  uint64_t buf;
	  read (arguments[0], &buf, sizeof (buf));
	  buf *= 2;
	  write (results[1], &buf, sizeof (buf));
	}
      assert (!"Executed");
    }

  total_time = (to_tick - from_tick) / (CLOCKS_PER_SEC / 1000);
  printf ("It took %s %ld ms to perform %s calls", argv[0], total_time,
	  argv[1]);
  if (total_time > 0)
    printf (" (%lld calls per second)\n", 1000 * steps_number / total_time);
  else
    putchar ('\n');
}
