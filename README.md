# rt-app

Sources for a real-time mobile robotics application exploiting the Linux kernel and SCHED_DEADLINE.

## Notice

![RTFM AHEAD](data:image/png;base64,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)

***MAKE SURE TO RUN `make docs` AND READ THE DOCUMENTATION THROUGHLY PRIOR TO BUILDING AND ATTEMPTING TO RUN THE DEMO.***

## Design

<!--
```mermaid
flowchart LR;
subgraph main
m_main(("main()"))
end
subgraph odo
o_worker("O_Worker()")

end
m_main-- "pthread_create()" --o_worker
```
-->

The application is currently structured as a single process, which in turn spawns a number of threads. The **dispatcher** thread, or main thread, is the thread created by the Linux shell when we launch the application. A variable number of **worker** threads are created and by the dispatcher, and marked for scheduling with SCHED_DEADLINE.

## Structure

The project is structured as follows:

- `docs` - The project's documentation
- `src` - The demo application's source code
   - `main.c` - The entry point of the application, and core of the dispatcher
   - `h_*.h` - Helpers and hooks
     - `h_sched.h` - Useful hooks for scheduling and such
     - `h_time.h` - Hooks to interact with timers and to do timestamping
   - `a_*.<c,h>` - Adapters for various hardware platforms
     - `a_otto.<c,h>` - The adapter for IRAlab's OTTO mobile platform
   - `m_*.<c,h>` - The modules of the application, each exposing a worker
     - `m_odo.<c,h>` - The Odometry module

## Building

On apt-based system do:

```
$ sudo apt install build-essential git doxygen libnanopb-dev nanopb
```

to install dependencies, and:

```
$ make
```

to build an executable.

## Modules

The project follows a modular approach: each module must implement a quite basic public interface, consisting only of a generic worker function, which signature must be `void* (*name)(void*)`.

### Odometry

1. Encoder
   1. Coefficiente che dice a quale frazione di un arco corrisponde un tick (schedina)
1. Rototraslazione
   1. Traslo in CIR (discorde con y)
   1. Ruoto (discorde $\Delta\theta$)
   1. Ritraslo (concorde con y)
