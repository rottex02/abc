# xv6 Scheduling Algorithms: Simple Notes

## 1. Introduction

The scheduler decides **which process gets the CPU next**. xv6 normally uses **Round Robin (RR)**: each process runs for one timer tick, then the CPU moves to the next one in the process table.

Our patches replace this with other policies: **FCFS, SJF, SRTF and Priority**. Every patch is a `git diff` that you apply on a clean xv6 copy.

## 2. How RR works in xv6 (the part we change)

- `trap.c`: on every timer tick, it calls `yield()`. This is what makes xv6 preemptive.
- `proc.c` has `scheduler()`. It loops over the process table and runs the next RUNNABLE process.
- `proc.h` has `struct proc`. This is where we add new fields like `ctime`, `burst`, `priority`.

## 3. What every patch changes

| File | Change |
| --- | --- |
| `param.h` | Adds `SCHEDPOLICY` to choose the policy (and constants like default burst) |
| `proc.h` | Adds a new field to `struct proc` |
| `proc.c` | Sets the field in `allocproc()`, rewrites `scheduler()` to pick the best process |
| `trap.c` | Turns the timer `yield()` off (non-preemptive) or leaves it on (preemptive) |
| `syscall.c/.h`, `sysproc.c`, `usys.S`, `user.h`, `defs.h` | Add a new system call (only SJF, SRTF and Priority) |
| `Makefile` | Adds the test program to `UPROGS` |

## 4. The algorithms

### FCFS (First Come First Served): non-preemptive

- **Idea:** the process that arrived first runs first, and keeps the CPU until it finishes.
- **Field added:** `ctime` = the tick when the process was created.
- **Scheduler:** pick the RUNNABLE process with the smallest `ctime`.
- **trap.c:** no `yield()` on timer tick.

### SJF (Shortest Job First): non-preemptive

- **Idea:** the process with the smallest burst (run time) goes first and runs till the end.
- **Field added:** `burst`.
- **Syscall:** `setburst(pid, burst)` so a program can tell the kernel how long its job is.
- **Scheduler:** pick the RUNNABLE process with the smallest `burst`.
- **trap.c:** no `yield()` on timer tick.

### SRTF (Shortest Remaining Time First): preemptive

- **Idea:** like SJF, but if a shorter job shows up, it takes over the CPU.
- **Field added:** `remaining`.
- **Syscall:** `setburst(pid, burst)` sets `remaining`.
- **Scheduler:** pick the smallest `remaining`.
- **yield():** reduce `remaining` by 1 each tick.
- **trap.c:** keep the timer `yield()`, so the scheduler re-checks every tick.

### Priority scheduling: preemptive

- **Idea:** the process with the highest priority runs first.
- **Fields added:** `priority` (1 to 10, default 5) and `lastrun`.
- **Syscall:** `nice(n)` sets the priority of the calling process.
- **Scheduler:** pick the highest `priority`. If two are equal, pick the one that ran least recently (round robin among equals).

## 5. The common pattern in scheduler()

All the patches use the same two steps:

```c
struct proc *best = 0;

// Step 1: find the best RUNNABLE process
for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
  if(p->state != RUNNABLE)
    continue;
  if(best == 0 || /* p is better than best */)
    best = p;
}

// Step 2: run it
if(best){
  p = best;
  c->proc = p;
  switchuvm(p);
  p->state = RUNNING;
  swtch(&(c->scheduler), p->context);
  switchkvm();
  c->proc = 0;
}
```

Only the `/* p is better than best */` condition changes:

| Policy | Condition for "better" |
| --- | --- |
| FCFS | `p->ctime < best->ctime` |
| SJF | `p->burst < best->burst` |
| SRTF | `p->remaining < best->remaining` |
| Priority | `p->priority > best->priority` |

## 6. Quick comparison

| Policy | Preemptive? | Picks | Needs new syscall? |
| --- | --- | --- | --- |
| RR (default) | Yes | next in table | No |
| FCFS | No | earliest arrival | No |
| SJF | No | smallest burst | Yes (`setburst`) |
| SRTF | Yes | smallest remaining time | Yes (`setburst`) |
| Priority | Yes | highest priority | Yes (`nice`) |

## 7. Likely question

> Modify xv6 to use **\[FCFS / SJF / SRTF / Priority\]** scheduling instead of Round Robin. Add the needed field to `struct proc`, change `scheduler()` to pick the next process by the policy, and change `trap.c` if needed. Add a system call if the policy needs one. Write a user program that creates a few child processes and shows the order in which they run.

## 8. How to test

1. Apply the patch: `git apply 612415103-FCFS.patch` (or the one you need).
2. Write the test program (for example `fcfstest.c`) and add it to `UPROGS` in the Makefile.
3. In the test program, `fork()` 3 to 4 children. Give them different work (a loop of different lengths) and, if needed, different burst times (`setburst`) or priorities (`nice`).
4. Make each child print its pid when it finishes.
5. Run `make qemu` and start the test program. Check that the finishing order matches the policy:
   - **FCFS:** children finish in the order they were created.
   - **SJF / SRTF:** the shortest job finishes first.
   - **Priority:** the highest priority child finishes first.

## 9. Points to remember

- Turning off `yield()` in `trap.c` makes a policy **non-preemptive**.
- The test programs are **not inside the patch files**. You need to write them.
- `priority.patch` only changes the **time slice length** by priority, so it is not true priority scheduling. `priority_actual.patch` is the proper one.
