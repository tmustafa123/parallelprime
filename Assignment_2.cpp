// Assignment_2.cpp : Defines the entry point for the console application.
//
#include<iostream>
#include<atomic>
#include<chrono>
#include <vector>
#include <thread>
#include"prime.h"
#include<random>
#include<algorithm>


std::mutex mutex;
prime task;


void print_(int task, std::thread::id this_id);
int random_();
void worker_farm(prime& tq,int delay);

int main(int argc, char * argv[])
{
	if (argc == 1) // means sequential version 
	{
		std::cout << "Usage is: " << argv[0] << "  tasks  workers delay " << std::endl;
		return(0);
	}

	int n = atoi(argv[1]);
	int nw = atoi(argv[2]);
	int delay = atoi(argv[3]);
	
	
	/********************* EMITTER STAGE START ****************************/
	
	/*Creates 2 arrays . One for holding the random tasks calculated and one holds the number of primes from 1-N . Where N is a random task*/
	task.create(n);
	
	/*Generate random integers and push them into a queue of integers*/
	for (size_t i = 0; i < n; i++)
	{
		task.add_task(random_(),i);	
	}

	/********************* EMITTER STAGE ENDS ****************************/

	/* Get the time before setting the parallel environment */

	std::vector<std::thread> tid;
	auto t1 = std::chrono::high_resolution_clock::now();

	
	
	/********************* WORKER STAGE STARTS ****************************/

	/* Start the threads where each thread works on a task */
	for (int i = 0; i<nw; i++) 
	{
		tid.push_back(std::thread(worker_farm, std::ref(task),delay));
	}
	
	/* wait for the threads to sychronize */
	for (int i = 0; i<nw; i++)
		tid[i].join();
	
	/********************* WORKER STAGE ENDS ****************************/


	/********************* COLLECTOR STAGE STARTS ****************************/
	/*output the results*/

	//task.get_result();

	/********************* COLLECTOR STAGE ENDS ****************************/

	/*get the time after the excecution of the parallel tasks*/
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = t2 - t1;
	std::cout << "\nTOTAL TIME ELASPED :  " << elapsed.count() << "  Seconds" <<std::endl;

	
    return 0;
}



void worker_farm(prime& tq,int delay)
{
	std::atomic<int> Reduce = { 0 };
	/* each worker computes the number of tasks given*/
	std::atomic<int> tasks = { 0 };
    
	
	std::thread::id this_id = std::this_thread::get_id();
	/*passive waiting for tasks to be taken*/
	while (true)
	{
		/* pop a task form the queue */
		int task = tq.assign_task();
		/* if the worker gets a valid task */
		if (task > 0)
		{
			/* increment the number of tasks recieved */
			tasks++;
			
			Reduce = tq.getPrime(task,delay);
		}
		else
		{
			/* queue is empty . No more tasks*/
			break;
		}
		
	}
	print_(tasks, this_id);
	
	
	
}
void print_(int task,  std::thread::id this_id)
{
	std::unique_lock<std::mutex> lck(mutex);
	std::cout << "TOTAL TASKS RECIEVED BY THREAD : " << this_id << " : " << task << std::endl;

}
int random_()
{
	
	std::random_device rd;

	// Initialize Mersenne Twister pseudo-random number generator
	std::mt19937 gen(rd());

	// Generate pseudo-random numbers
	// uniformly distributed in range (1, 1000)
	std::uniform_int_distribution<> dis(1, 1000);

	return dis(gen);

}
