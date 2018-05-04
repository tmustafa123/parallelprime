//
// define the function translating a char into another char
// for the sake of simplicity:
// just turns letters capital to small and viceversa,
// loosing some time
//

#include<cctype>
#include <iostream>
#include <mutex>
#include <deque>
#include <chrono>
#include <cstddef>
#include <string>
#include<array>
#include<atomic>
#include <cmath>

class prime
{
private:
	std::mutex mtx, mtx_;
	std::deque<int> task;
	std::deque<int> result;
	bool ready = false;
	int *N, *totalPrimes;
	int SIZE,i= 0;
	std::atomic<int> local_Reduce = { 0 };

	

public:
	
	prime(std::string s) { std::cout << "Created " << s << " queue " << std::endl; }
	prime() {}

	/*create 2 arrays . One will hold the number of primes already computed by one worker so the results can be used to find a number equal or higher
	than the ones already in the list. So each worker needs to to recompute something already done by someone. The other keeps the tasks. */
	void create(int size)
	{
		this->N = new int[size];
		this->totalPrimes = new int[size];
		this->SIZE = size;
	}

	/* push the random tasks and initialize the array to -1 that hols the results of the total primes*/
	void add_task(int value,int i)
	{
		{
			task.push_back(value);
			this->totalPrimes[i] = -1;
			this->N[i] = value;
			std::cout << " GENERATED : " << this->N[i] <<std::endl;

		}
	}
	

	int assign_task()
	{
		
		/*need to lock multiple accesses */
		std::unique_lock<std::mutex> lck(mtx);
		/* if the queue is empty i.e all tasks have been computed set a flag*/
		if (task.empty())
			return -1;

		else
		{
			int temp = task.front();
			task.pop_front();
			return temp;
		}
		
	}

	/* push the results into the shared DS*/
	void add_result(int totalprimes)
	{
		{
			std::unique_lock<std::mutex> lck(mtx_);
			result.push_back(totalprimes);
			
		}

	}
	void get_result()
	{
		for (auto it = result.begin(); it != result.end(); ++it)
		{
			std::cout << " RESULT : " << *it << std::endl;
		}
	}

	/* taken from your code*/
	bool is_prime(int n) 
	{
		int limit = std::sqrt(n);
		for (int i = 2; i <= limit; i++) 
		{
			if (n % i == 0) 
			{
				return false;
			}
		}
		return true;
	}
	/*this function finds the number of primes within a range defined by the start and stop parameters*/
	int total_primes(int start, int stop)
	{
		/*initialize to 0 before a new computation*/
		local_Reduce = 0;
		for (size_t i = start; i <= stop; i++)
		{
			if (is_prime(i))
			{
				local_Reduce++;
			}
		}
		
		return local_Reduce;
	}
	/* this takes a task a computes the prime numbers between 1-num*/
	int getPrime(int num,int delay)  
	{
		int start = 0;
		int  end = 0;
		int temp = 0;
		int index = -1;
		int highest = 0;
		std::atomic<int> total = { 0 };

		/*parse the list to check whats already calculated and use the results*/
		for (size_t i = 0; i < this->SIZE; i++)
		{
			total = 0;
			if ( N[i] < num && totalPrimes[i] != -1)
			{
				
				if (highest < N[i])
				{
					highest = N[i];
					/*keep the result of the number of primes lower than num. This will be added to the result of the computation of the new range*/
					temp = this->totalPrimes[N[i]];
				}
				/* set the start to the next number after the one already known*/
				start = highest + 1;
				end = num;
				index = i;
			}
			
			/* never calculated. new prime numbers */
			else if (N[i] == num && totalPrimes[i] == -1) 
			{
				start = 1;
				end = num;
				temp = 0;
				index = i;
			}
			/* result already available */
			else if (N[i] == num && totalPrimes[i] != -1)
			{

				add_result(this->totalPrimes[i]);
				return this->totalPrimes[i];
				
			}
		
		}
		/* call the function that returns the number of prime numbers between 'start' and 'end' */
		total=this->total_primes(start, end) + temp;
		this->totalPrimes[index] = total;
		/* push the results*/
		
		/*add some delay now*/
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		add_result(total);
		return total;
	}


};

/* used from the code provided*/
void active_delay(int msecs)
{
	// read current time
	auto start = std::chrono::high_resolution_clock::now();
	auto end = false;
	while (!end) 
	{
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
		if (msec>msecs)
			end = true;
	}
	return;
}











