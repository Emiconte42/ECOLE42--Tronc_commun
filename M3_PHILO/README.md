
*This project was completed as part of the 42 curriculum by emiconte.*

# PHILOSOPHERS

## Description

The goal of this project is to simulate philosophers sitting around a table.
Each philosopher always does the same things:

eat
sleep
think

To eat, a philosopher needs 2 forks.
Problem: the forks are shared.

So, we have to manage:

- multiple threads at the same time
- shared resources
- without breaking everything

The real challenge is:

- avoiding data races (2 threads trying to modify the same variable at the same time)
- avoiding deadlocks (two threads waiting for each other -> deadlock, they wait forever, sad!)
- avoiding philosopher death
- using mutexes = locks that protect shared data
- 1 thread = 1 philosopher

## Instructions

### Compilation

```
make
```
### Execution

```
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```
Example
```
./philo 5 800 200 200
```

Parameters:
- number_of_philosophers: number of philosophers
- time_to_die: time before dying (ms)
- time_to_eat: time to eat
- time_to_sleep: time to sleep
- number_of_times_each_philosopher_must_eat: optional

### Project Details

BASE:
- 1 philosopher = 1 thread
- 1 fork = 1 mutex
- 2 forks are needed to eat

PROBLEM:

If you do things wrong:
- direct deadlock
- or starvation
- or data race

So you must:
- lock at the right time
- unlock properly
- protect important variables

What is important:
- last_meal must be protected
- forks must be protected
- prints must be clean (not mixed)

STOP
- The program stops if a philosopher dies or everyone has eaten enough

LOGS

Required format:

- timestamp X has taken a fork
- timestamp X is eating
- timestamp X is sleeping
- timestamp X is thinking
- timestamp X died

## Resources:

- man pthread_create
- man pthread_mutex_lock
- man gettimeofday
- https://medium.com/@denaelgammal/dining-philosophers-problem-42-project-guide-mandatory-part-a20fb8dc530e
- https://en.wikipedia.org/wiki/Dining_philosophers_problem

## Use of AI

AI was only used as a tool to clarify some theoretical principles and to help write the README in English.