/*
created by:YIU YEUNG NG
ID:a1798286   
time:06.08.2020
Contact Email: a1798286@adelaide.edu.au
Include int main(int argc,char *argv[])
input: argv[1]
output: Screen

input sample:
ID arrival_time priority age total_tickets_required
for example: s1 3 1 0 50

output sample:
ID, arrival and termination times, ready time and durations of running and waiting
*/

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <bitset>
#include <list>
#include <stdexcept>
#include <functional>
#include <utility>
#include <ctime>
#include <stdio.h> 
#include <string.h>

using namespace std;

//customer struct
struct customer 
{	//constructor
	customer(const string& Id, const int& Arrival_time, const int& Priority, const int& Age, const int& Total_tickets_required):
		id(Id), arrival_time(Arrival_time), priority(Priority), age(Age), total_tickets_required(Total_tickets_required) {}
	
	//first batch info (according to file data reading order)
	string id;
	int arrival_time;
	int priority;
	int age;
	int run;
	int total_tickets_required;
	//secon batch
	int total_tickets_bought;
	int worked_alr;
	//third batch 
	int weighted_time_quantum;
	int waiting_time;
	int ready_time;
	int running_time;
	int end_time;
	
};
//global variables
vector<customer* > customer_pool;
vector<customer* > public_queue;
vector<customer* > pre_queue_1;
vector<customer* > pre_queue_2;
vector<customer* > queue_1;
vector<customer* > queue_2;
vector<customer* > finish_queue;
vector<customer* > promote_queue;
vector<customer* > demote_queue;

int time_units_running = 0;

int largest_arrival_time;



//test function
void test_queue(vector<customer* > queue)
{
	
	for (int i = 0; i < queue.size(); ++i)
	//for (std::vector<customer* >::iterator i = queue.begin(); i != queue.end(); ++i)
	{
		printf("%s %i %i %i %i %i\n", queue[i]->id.c_str(), queue[i]->arrival_time, queue[i]->total_tickets_required, queue[i]->running_time, queue[i]->priority, queue[i]->age); 
	}

}

void test_queue1(vector<customer* > queue)
{
	
	for (int i = 0; i < queue.size(); ++i)
	//for (std::vector<customer* >::iterator i = queue.begin(); i != queue.end(); ++i)
	{
		printf("%s %i %i %i %i %i %i\n", queue[i]->id.c_str(), queue[i]->arrival_time, queue[i]->total_tickets_required, queue[i]->running_time, queue[i]->priority, queue[i]->run, queue[i]->waiting_time); 
	}

}

void test_queue2(vector<customer* > queue)
{
	
	for (int i = 0; i < queue.size(); ++i)
	//for (std::vector<customer* >::iterator i = queue.begin(); i != queue.end(); ++i)
	{
		printf("%s %i %i %i %i %i %i\n", queue[i]->id.c_str(), queue[i]->arrival_time, queue[i]->total_tickets_required, queue[i]->running_time, queue[i]->priority, queue[i]->age, queue[i]->waiting_time); 
	}

}
//transform id from string to id for comparison
int id_str_to_int(string id)
{
	int ori_length = id.size()-1;
	string str_id = id.substr(1, ori_length);

	int int_id;
	istringstream(str_id) >> int_id;

	return int_id;
	
}

//sort method of queue 1
bool sort_queue_1(customer* i, customer* j)
{
	
	if (i -> priority != j -> priority) 
    {
        return i -> priority < j -> priority;

    } else if (i -> arrival_time != j -> arrival_time)
    {
        return i -> arrival_time < j -> arrival_time;

    } else 
    {

        return id_str_to_int(i -> id) < id_str_to_int(j -> id);

    }
}


//function to sort queue 1
void queue1_swap_position(vector<customer* > &pre_queue_1)
{

    sort(pre_queue_1.begin(), pre_queue_1.end(), sort_queue_1);
}

void assign_queue1_after_first_run(customer* pre_queue_1_customer)
{	
	//No need to handle the first one (non-preemptively)
	if (queue_1.size() == 0){
		queue_1.push_back(pre_queue_1_customer);
		return;
	}

	for (int i = 1; i < queue_1.size()-1; ++i)
	{
		if (queue_1[i]->priority > pre_queue_1_customer->priority)
		{
			queue_1.insert(queue_1.begin() + i, pre_queue_1_customer);
			return;
		} 
	}	

	int tmp_p = queue_1.size()-1;

	if (queue_1.size()>1 && pre_queue_1_customer->priority < queue_1[tmp_p]->priority)
	{
		queue_1.insert(queue_1.begin() + tmp_p, pre_queue_1_customer);
		return;
	}
	// At this point, the pre_queue_1_customer must be the last one
	queue_1.push_back(pre_queue_1_customer);
}

//sort method of queue 2
bool sort_queue_2(customer* i, customer* j)
{

	if (i -> total_tickets_required != j -> total_tickets_required)
    {
        return i -> total_tickets_required < j -> total_tickets_required;

    } else if (i -> priority != j -> priority)
    {
        return i -> priority < j -> priority;

    } else if (i -> arrival_time != j -> arrival_time)
    {
        return i -> arrival_time < j -> arrival_time;

    } else {

        return id_str_to_int(i -> id) < id_str_to_int(j -> id);

    }
}

//function to sort queue 2
void queue2_swap_position(vector<customer* > &pre_queue_2)
{

    sort(pre_queue_2.begin(), pre_queue_2.end(), sort_queue_2);

}

void assign_queue2_after_first_run(customer* pre_queue_2_customer)
{				
	if (queue_2.size() == 0){
		queue_2.push_back(pre_queue_2_customer);
		return;
	}

	if (queue_2[0]->total_tickets_required > pre_queue_2_customer->total_tickets_required)
	{
		vector<customer *> first;
		first.push_back(pre_queue_2_customer);
		first.insert(first.end(), queue_2.begin(), queue_2.end());
		queue_2 = first;
		return;
	}

	for (int i = 0; i < queue_2.size()-1; ++i)
	{
		if (queue_2[i+1]->total_tickets_required > pre_queue_2_customer->total_tickets_required)
		{
			queue_2.insert(queue_2.begin() + i, pre_queue_2_customer);
			return;
		} 
	}

	queue_2.push_back(pre_queue_2_customer);

}

//function to insert promoted customer from queue 2 to queue 1 
void queue2_promote(customer* promoted_customer)
{

	promote_queue.push_back(promoted_customer);
	
}

//function to insert demoted customer from queue 1 to queue 2
void queue1_demote(customer* demoted_customer)
{

	demote_queue.push_back(demoted_customer);

}


//function to insert customer who've finished purchase to the 'finish queue'
void add_to_finish(customer* customer)
{

	finish_queue.push_back(customer);

}

//function to return the current time units
int return_time_units(int time_units_running)
{

	return time_units_running;
}

//calculation of weighted time quantum for getting customer's ticket quota
int calculate_wTq(customer* customer)
{

	int weighted_time_quantum;
	int pNum = customer -> priority;
	weighted_time_quantum = (10 - pNum)* 10;

	return weighted_time_quantum;
}

//function to get customer's ticket quota
int ticket_quota(customer* customer)
{

	int ticket_quota;
	int wTq = calculate_wTq(customer);
	ticket_quota = wTq/5;

	return ticket_quota;
}

//function to get the largest arrival time of customers
int get_largest_arrival_time(vector<customer* > &customer)
{

	vector<int> temp_arr;
	for (int i = 0; i < customer.size(); ++i)
	{
		temp_arr.push_back(customer[i]->arrival_time);
	}

	return *max_element(temp_arr.begin(), temp_arr.end()); 
}


int get_smallest_arrival_time(vector<customer* > &customer)
{
	vector<int> temp_arr;
	for (int i = 0; i < customer.size(); ++i)
	{
		temp_arr.push_back(customer[i]->arrival_time);
	}

	return *min_element(temp_arr.begin(), temp_arr.end()); 
}
//function to increment the age by 5 units after each run of the first customer of queue 1
void q1_running_update_queue_2_age(vector<customer* > &queue_2)
{

	for (int i = 0; i < queue_2.size(); ++i)
	{
		queue_2[i]->age += 5;
	}
}

//function to increment queue 2 customers's age by 5 units after each run of the first customer of queue 2
void q2_running_update_queue_2_age(vector<customer* > &queue_2)
{

	for (int i = 1; i < queue_2.size(); ++i)
	{
		queue_2[i]->age += 5;
	}
}

//print function
void print_queue_time_status(vector<customer* > queue)
{
    int i;
    cout<<"name   arrival   end   ready   running   waiting"<<endl;
    for(i=0; i<queue.size(); i++) // every result
    {
    	printf("%s %i %i %i %i %i\n", queue[i]->id.c_str(), queue[i]->arrival_time, queue[i]->end_time, queue[i]->ready_time, queue[i]->running_time, queue[i]->waiting_time);
    }
    cout<<endl;
}

//assign customers from public queue to queue 1 and queue 2 according to customer's priority
void assign_queue(vector<customer* > public_queue)
{
	for (int i = 0; i < public_queue.size(); ++i)
	{

		if(public_queue[i]->priority <= 3)
		{

		pre_queue_1.push_back(public_queue[i]);

		} else {

		pre_queue_2.push_back(public_queue[i]);

		}
	}
}

//main function
int main(int argc,char *argv[]){
	// function of reading data from file
	istringstream iss;
	string newId;
	int newArrival_time;
	int newPriority;
	int newAge;
	int newPurchase;

	ifstream ifs;
	ifs.open(argv[1], ios::in);

	if (ifs.is_open()){
		string info;
		
		while(getline(ifs, info)){
			istringstream iss (info);
			iss >> newId;
			iss >> newArrival_time;
			iss >> newPriority;
			iss >> newAge;
			iss >> newPurchase;

			customer* temp = new customer(newId, newArrival_time, newPriority, newAge, newPurchase);
			customer_pool.push_back(temp);
		}
	}		

	time_units_running = get_smallest_arrival_time(customer_pool);
	largest_arrival_time = get_largest_arrival_time(customer_pool);


	while(true)
	{	
		if (customer_pool.size() == 0 && queue_1.size() == 0 && queue_2.size() == 0 && time_units_running >= largest_arrival_time)
		{
			break;
		}	
			vector<customer* > remaining_customer;

			for (int i = 0; i < customer_pool.size(); ++i)
			{	// customer from customer pool are added into public queue when the time units meets their arrival time
				if (customer_pool[i] -> arrival_time == time_units_running)
				{
				
					public_queue.push_back(customer_pool[i]);

						
				} else 
				{
					remaining_customer.push_back(customer_pool[i]);
				}
			}
			customer_pool = remaining_customer;
			
			//assign customers to queue 1 and 2 according to their priorities
			assign_queue(public_queue);

			//then empty public queue after each round of assigning customers into queue 1 and 2
			public_queue.erase(public_queue.begin(), public_queue.end());

			// sort the pre_queue_1 before pushing into queue_1
			if (pre_queue_1.size() > 0)
			{
				queue1_swap_position(pre_queue_1);
			}
			// sort the pre_queue_2 before pushing into queue_2
			if (pre_queue_2.size() > 0)
			{
				queue2_swap_position(pre_queue_2);
			}		

			// in the beginning of the programme, both queue 1 & 2 are empty, simply insert all the customers to queue 1 & 2
			if (time_units_running == 0)
			{		
				// add sorting result into queue 1
				for (int i = 0; i < pre_queue_1.size(); ++i)
				{
					queue_1.push_back(pre_queue_1[i]);
				}
					
				pre_queue_1.clear();
			

				// add sorting result into queue 2
				for (int i = 0; i < pre_queue_2.size(); ++i)
				{
					queue_2.push_back(pre_queue_2[i]);
				}
					
				pre_queue_2.clear();

			// from the second run of programme onwards
			} else
			{	
				// handle the order of inserting customer into queue 1
				// new arrival customer -> customer used up quota for once -> promoted customer

				// new arrival customer
				if (pre_queue_1.size() > 0)
				{
					for (int i = 0; i < pre_queue_1.size(); ++i)
					{
						assign_queue1_after_first_run(pre_queue_1[i]);
					}

					pre_queue_1.clear();
				}

				// condition 1: the first customer in queue 1 has bought all tickets he/she needs
				if (queue_1.size() > 0)
				{

					if (queue_1[0]->total_tickets_required == 0){
							
						queue_1[0]->end_time = return_time_units(time_units_running);
						finish_queue.push_back(queue_1[0]);
						queue_1.erase(queue_1.begin());
					// condition 2: the first customer in queue 1 hasn't bought all tickets he/she needs while using up all quotas for a run
					} else if (queue_1[0]->total_tickets_bought == ticket_quota(queue_1[0]))
					{	

						queue_1[0]->total_tickets_bought -= ticket_quota(queue_1[0]); //reset total_tickets_bought to zero for next counting 

						queue_1[0]->run += 1;

						if (queue_1[0]->run == 2)
						{
							queue_1[0]->priority += 1;
							queue_1[0]->run -= 2;		// reset run to zero for next counting
						}

						if (queue_1[0]->priority > 3)
						{
							queue1_demote(queue_1[0]);
							queue_1.erase(queue_1.begin());

						} else // if priority no. is still 1-3
						{	
							int last_index = queue_1.size(); //calculate the no. of customer in queue 1
							customer* tmp_first;
							//if the priority of customer queue_1[0] is 3 
							if (queue_1[last_index - 1]->priority == queue_1[0]->priority)
							{		
								queue_1.push_back(queue_1[0]);
								queue_1.erase(queue_1.begin());

							//if the priority of customer queue_1[0] is the largest now
							} else if (queue_1[last_index - 1]->priority < queue_1[0]->priority)
							{
								queue_1.push_back(queue_1[0]);
								queue_1.erase(queue_1.begin());

							// if the priority of customer queue_1[0] is 1 or 2
							} else {
								int tmp_index; //store the index of first customer with larger priority no.

								for (int i = 1; i < queue_1.size(); ++i)
								{
											
									if (queue_1[i]->priority > queue_1[0]->priority)
									{
											tmp_index = i;		
											break;
									}
								}

								tmp_first = queue_1[0];
								queue_1.insert(queue_1.begin() + tmp_index, tmp_first);
								queue_1.erase(queue_1.begin());
							}

						}
					}
					
				}

				// promoted customer
				if (promote_queue.size() > 0)
				{
					for (int i = 0; i < promote_queue.size(); ++i)
					{	

						assign_queue1_after_first_run(promote_queue[i]);
					}
					
					promote_queue.clear();
				}

				/***************************************/

				// Kick away the finished customer in q2
				if (queue_2.size() > 0){
					if (queue_2[0]->total_tickets_required == 0){
							
						queue_2[0]->end_time = return_time_units(time_units_running);
						finish_queue.push_back(queue_2[0]);
						queue_2.erase(queue_2.begin());
					} 
				}

				// handle the order of inserting customer into queue 2
				// demoted customer -> new arrival customer

				// demoted customer 
				if (demote_queue.size() > 0)
				{
						
					for (int i = 0; i < demote_queue.size(); ++i)
					{
						assign_queue2_after_first_run(demote_queue[i]);
					}

					demote_queue.clear();

					queue2_swap_position(queue_2);

				}

				// new arrival customer
				if (pre_queue_2.size() > 0)
				{
					for (int i = 0; i < pre_queue_2.size(); ++i)
					{
						assign_queue2_after_first_run(pre_queue_2[i]);
					} 

					pre_queue_2.clear();

					queue2_swap_position(queue_2);
				}
			}

			//if queue 1 is not empty, then run queue 1 first
			if (queue_1.size() > 0)
			{

				queue_1[0]->total_tickets_required -= 1;
				queue_1[0]->total_tickets_bought += 1;
				queue_1[0]->running_time += 5;
				queue_1[0]->worked_alr += 1; //for counting whether a customer has started his/her first run

				//only update ready time when work_alr is 1
				if (queue_1[0]->worked_alr == 1)
				{
    				queue_1[0]->ready_time = (return_time_units(time_units_running));
				}
				// increment the running time for those who have wait after their first run among queue_1[1] to queue_1[i] 
				for (int i = 1; i < queue_1.size(); ++i)
				{	
					if (queue_1[i]->worked_alr >= 1)
					{
						queue_1[i]->waiting_time += 5;
					}
						
				}
				// increment the running time for those who have wait after their first run among queue_2[0] to queue_2[i]
				for (int i = 0; i < queue_2.size(); ++i)
				{
					if (queue_2[i]->worked_alr >= 1)
					{
						queue_2[i]->waiting_time += 5;
					}
				}	
				// increment the age counting of each customer in queue 2
				q1_running_update_queue_2_age(queue_2);

			// Only handle queue 2 when queue 1 is empty
			} else if (queue_1.size()==0 && queue_2.size()>0)
			{	
				int temp_age = queue_2[0]->age;
				queue_2[0]->age -= temp_age;
			    queue_2[0]->total_tickets_required -= 1;
				queue_2[0]->total_tickets_bought += 1;
				queue_2[0]->running_time += 5;
				queue_2[0]->worked_alr += 1;

				if (queue_2[0]->worked_alr == 1)
				{
    				queue_2[0]->ready_time = (return_time_units(time_units_running));
				}

				for (int i = 1; i < queue_2.size(); ++i)
				{
					if (queue_2[i]->worked_alr >= 1)
					{
						queue_2[i]->waiting_time += 5;
					}
				}	

				q2_running_update_queue_2_age(queue_2);
			}

			vector<customer* > remaining_queue2;

			for (int i = 0; i < queue_2.size(); ++i)
			{

				if (queue_2[i]->age == 100) 
				{	
					queue_2[i]->age -= 100;
					queue_2[i]->priority -= 1;
				}

				if (queue_2[i]->priority == 3) 
				{	
					queue_2[i]->total_tickets_bought = 0;
					queue2_promote(queue_2[i]);
					
				} else 
				{
					remaining_queue2.push_back(queue_2[i]);
				}

			}

			queue_2 = remaining_queue2;

			/*****************************************************************/
			//for test
			// if (time_units_running >= 2445 && time_units_running <= 2455)
			// {
			// 	    cout << "Time unit" << " " << time_units_running << endl;
			// 	    cout << "Q1:" << endl;
			// 	    test_queue1(queue_1);
			// 	    cout << "-" << endl;
			// 	    cout << "Q2:" << endl;
			// 	    test_queue2(queue_2);
			// 	    cout << "-" << endl;
				    // cout << "PQ:" << endl;
				    // test_queue(promote_queue);
				    // cout << "-" << endl;
				    // cout << "Finish:" << endl;
				    // print_queue_time_status(finish_queue);
				    // cout << "customer_pool" << endl;
				    // test_queue(customer_pool);
		
				    // cout << "customer_pool" << endl;
				    // test_queue(customer_pool);
				    // cout << "public queue" << endl;
				    // test_queue(public_queue);
				    // cout << "count time: " << endl;
				    // print_queue_time_status(finish_queue);
					// test_queue(customer_pool);
			//}
			

			// time units increment by 5 every loop
			time_units_running += 5;

	}

	print_queue_time_status(finish_queue);

	return 0;
}


		

	




		

		


	
	









	
		










