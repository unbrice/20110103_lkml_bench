#define _GNU_SOURCE

#include <assert.h>
#include <err.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct thread_args
{
  uint64_t in;
  uint64_t out;
  pthread_barrier_t barrier;
};

static void
force_cpu (int cpu)
{
  cpu_set_t cpu_set;
  CPU_ZERO (&cpu_set);
  CPU_SET (cpu, &cpu_set);
  sched_setaffinity (0, sizeof (cpu_set_t), &cpu_set);
}

void *
thread_routine (void *args_void)
{
  struct thread_args *args = args_void;
  force_cpu (0);
  while (1)
    {
      pthread_barrier_wait (&args->barrier);
      args->out = args->in * 2;
      pthread_barrier_wait (&args->barrier);
    }
  assert (!"Executed");
  return NULL;
}

int
main (int argc, char **argv)
{
  int cpu_number;
  clock_t from_tick, to_tick;
  long long steps_number;
  struct thread_args args;
  pthread_t thread;
  unsigned long total_time;

  if (argc != 3)
    errx (1, "Usage: %s ITERATIONS CPUS", argv[0]);
  steps_number = atoll (argv[1]);
  cpu_number = atoi (argv[2]);
  if (steps_number < 0)
    err (2, "ITERATIONS must be a positive number (%s)", argv[1]);
  if (cpu_number == 1)
    force_cpu (0);
  else if (cpu_number == 2)
    force_cpu (1);
  else
    err (3, "CPUS must be equal to 1 or 2 (%s)", argv[2]);

  pthread_barrier_init (&args.barrier, NULL, 2);
  pthread_create (&thread, NULL, thread_routine, &args);

  from_tick = clock ();
  for (unsigned n = 0; n < steps_number; ++n)
    {
      args.in = n;
      pthread_barrier_wait (&args.barrier);
      pthread_barrier_wait (&args.barrier);
      assert (args.out == n * 2);
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
