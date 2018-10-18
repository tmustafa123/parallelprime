# PrimeNumbers

```Problem```

Implement, using only C++ standard mechanisms and threads, a program that computes in parallel a set of independent tasks, initially stored in a shared data structure, and delivers results using a second shared data structure. The number of tasks to be computed is known and accessible to the parallel executors, as well as the number of tasks already computed/to compute. 
Assume an input task is given by an integer number N and the result to compute is the number of prime numbers included in range [1-N]. The initial set of tasks is picked up randomly in the range [1-10K].
Once the problem is working, add some delay in the procedure obtaining a task to be computed and delivering a result and observe the impact on scalability.

