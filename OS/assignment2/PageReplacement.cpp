/*
created by:YIU YEUNG NG
ID:a1798286   
time:06.10.2020
Contact Email: a1798286@adelaide.edu.au
Include int main(int argc,char *argv[])
input: argv[1]
output: Screen
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

// struct of page table
struct page_table_entry
{	

	//0: invalid, 1: valid
	page_table_entry():
		in_or_v_bit(0), dirty_bit(0), shift_reg_bit("0") {}; //initialize the two bits of page, 0 = invalid and edited before poping out from frame
	// attributes
	int in_or_v_bit;
	int dirty_bit;
	int last_runnning_time;
	string shift_reg_bit;
	
};

// global variables
vector<page_table_entry* > page_table; // simulate the page table
vector<int> frame; // simulate the page frame in a vector
vector<int> fifo_frame; // simulate the page frame in a queue for FIFO  
int page_size;          // page or frame size, in byte
int frame_number;		// no. of frame
string page_replacement_algorithm; // the page replacement algorithm read from command line
int shift_register_bit_size;
int regular_interval;

int time_unit_counter = 0;

int events_in_trace; // count the no. of events
int disk_reads;		 // count the no. of read
int disk_write;		 // count the no. of write
int page_fault;		 // count the no. of page fault

// function declared in here

// reverse the process id
string reverse(string str)
{
	int str_length = str.length();

	for (int i = 0; i < str_length/2; ++i)
	{
		swap(str[i], str[str_length-i-1]);
	}

	return str;

}


// process_id to reference string
long long int generate_ref_str(string process_id, int page_size)
{	
	vector<long long int> temp_sum; // store the numeric value of process id
	long long int temp_int;
	long long int temp_int2;
	string rev_process_id;

	rev_process_id = reverse(process_id);  

	for (int i = 0; i < rev_process_id.size(); ++i)
	{	
		if (rev_process_id[i] == 'a')
		{
			temp_int = 10*pow(16, i);
			temp_sum.push_back(temp_int);

		} else if (rev_process_id[i] == 'b')
		{
			temp_int = 11*pow(16, i);
			temp_sum.push_back(temp_int);

		} else if (rev_process_id[i] == 'c')
		{
			temp_int = 12*pow(16, i);
			temp_sum.push_back(temp_int);

		} else if (rev_process_id[i] == 'd')
		{
			temp_int = 13*pow(16, i);
			temp_sum.push_back(temp_int); 

		} else if (rev_process_id[i] == 'e')
		{
			temp_int = 14*pow(16, i);
			temp_sum.push_back(temp_int);

		} else if (rev_process_id[i] == 'f')
		{
			temp_int = 15*pow(16, i);
			temp_sum.push_back(temp_int);

		} else if (isdigit(rev_process_id[i])!=0)
		{	
			temp_int = rev_process_id[i]-'0';
			temp_int2 = temp_int*pow(16, i);
			temp_sum.push_back(temp_int2);

		}
		
	}

	long long int sum; // store the result of addition
	long long int gen_ref_str;
    sum = accumulate(temp_sum.begin(),temp_sum.end(),0LL);
	gen_ref_str = sum/page_size;

	return gen_ref_str;

}

bool check_page_fault(int page_num)
{
	return (page_table[page_num]->in_or_v_bit == 0);
}

bool check_free_space()
{
	for (int i = 0; i < frame.size(); ++i)
	{
		if (frame[i] == -1)
		{
			return true;
		}
	}
	return false;
}

// test print of page table
void print_page_table(int start, int end)
{
    int i;
    cout<<"---Page table---" <<endl;
    for(i=start; i<=end; i++) // every result
    {
    	printf("page:%d; valid bit:%d; dirty_bit:%d\n", i, page_table[i]->in_or_v_bit, page_table[i]->dirty_bit);
    }
    cout<<endl;
}

void print_frame()
{	
	cout << "Time:" << time_unit_counter << endl;
	for (int i = 0; i < frame.size(); ++i)
	{
		cout << frame[i] << " " << "(" << page_table[frame[i]]->shift_reg_bit << ")";
	}
	cout << endl;
	cout << endl;
}

// FIFO algorithm
void first_in_first_out(int page_num, string command)
{	
	//Find the victim page
	int victim_page;
	victim_page = fifo_frame[0];	

	//Replace the victim page with page_num

	//Update new page entry
	page_table[page_num]->in_or_v_bit = 1;
	if (command.compare("W") == 0)
	{
		page_table[page_num]->dirty_bit = 1;
	}

	//Update victim page entry
	page_table[victim_page]->in_or_v_bit = 0;
	if (page_table[victim_page]->dirty_bit == 1)
	{
		disk_write += 1;
		page_table[victim_page]->dirty_bit = 0;
	}

	//Update fifo queue
	fifo_frame.push_back(page_num);
	fifo_frame.erase(fifo_frame.begin());

	//Update frames
	frame.push_back(page_num);
	frame.erase(frame.begin());
	
}

// LRU algorithm
void least_recent_used(int page_num, string command)
{	
	vector<int> lrt;
	int min;
	int victim;
	int victim_index;
 	
 	for (int i = 0; i < frame.size(); ++i)
 	{
 		lrt.push_back(page_table[frame[i]]->last_runnning_time);
 	}
	
 	min = *min_element(lrt.begin(), lrt.end());

 	for (int i = 0; i < frame.size(); ++i)
 	{
 		if (page_table[frame[i]]->last_runnning_time == min)
 		{
 			victim = frame[i];
 			victim_index = i;
 		}
 	}
 	////////////////
 	page_table[page_num]->in_or_v_bit = 1;
	if (command.compare("W") == 0)
	{
		page_table[page_num]->dirty_bit = 1;
	}

	////////////////
	page_table[victim]->in_or_v_bit = 0;
	page_table[victim]->last_runnning_time = 0;
	if (page_table[victim]->dirty_bit == 1)
	{
		disk_write += 1;
		page_table[victim]->dirty_bit = 0;
	}

 	frame.push_back(page_num);
 	frame.erase(frame.begin()+victim_index);

}

// function to convert shift register binary number into decimal value
int decimal_shift_register_value(string binary_reg_bit)
{	
	vector<int> sum_for_dec;
	int decimal, temp_int, temp_int2;
	string rev_bi_reg_bit;

	rev_bi_reg_bit = reverse(binary_reg_bit);

	for (int i = 0; i < rev_bi_reg_bit.size(); ++i)
	{
		temp_int = rev_bi_reg_bit[i]-'0';
		temp_int2 = temp_int*pow(2, i);
		sum_for_dec.push_back(temp_int2);
	}

	decimal = accumulate(sum_for_dec.begin(),sum_for_dec.end(),0LL);	
	return decimal;
}

// function to update the digit of shift register bit of pages
// void update_use_shift_reg_bit(int page_num)
// {	
// 	string tmp_reg_bit;
// 	tmp_reg_bit = page_table[page_num]->shift_reg_bit;

// 	if (tmp_reg_bit.size()==1)
// 	{
// 		page_table[page_num]->shift_reg_bit = '1';

// 	} else 
// 	{
// 		int index;
// 		index = tmp_reg_bit.size()-1;

// 		rotate(tmp_reg_bit.begin(), tmp_reg_bit.begin()+index, tmp_reg_bit.end());
// 		tmp_reg_bit[0] = '1';

// 		page_table[page_num]->shift_reg_bit = tmp_reg_bit;
// 	}
// }

void update_non_use_shift_reg_bit(int page_num)
{
	string tmp_reg_bit;
	tmp_reg_bit = page_table[page_num]->shift_reg_bit;

	if (tmp_reg_bit.size()==1)
	{
		page_table[page_num]->shift_reg_bit = '0';

	} else 
	{
		int index;
		index = tmp_reg_bit.size()-1;

		rotate(tmp_reg_bit.begin(), tmp_reg_bit.begin()+index, tmp_reg_bit.end());
		tmp_reg_bit[0] = '0';

		page_table[page_num]->shift_reg_bit = tmp_reg_bit;
	}
}

// ARB 
void additional_ref_bit(int page_num, string command)
{	
	vector<int> ad_ref_bit;
	int min;
	int temp_v;
	int victim;
	int victim_index;
 	
 	//get the reg bit of all pages
 	for (int i = 0; i < frame.size(); ++i)
 	{	
 		int temp_min_p;
 		temp_min_p = decimal_shift_register_value(page_table[frame[i]]->shift_reg_bit);
 		ad_ref_bit.push_back(temp_min_p);
 	}
	
 	min = *min_element(ad_ref_bit.begin(), ad_ref_bit.end());		

	for (int i = 0; i < frame.size(); ++i)
	{
		temp_v = decimal_shift_register_value(page_table[frame[i]]->shift_reg_bit);

		if (temp_v == min)
		{	
			victim = frame[i];
			victim_index = i;
			break;
		}
	}
 	
 	////////////////
 	page_table[page_num]->in_or_v_bit = 1;
	if (command.compare("W") == 0)
	{
		page_table[page_num]->dirty_bit = 1;
	}

	////////////////
	page_table[victim]->in_or_v_bit = 0;

	page_table[victim]->shift_reg_bit.clear();
	for (int i = 0; i < shift_register_bit_size; ++i)
	{
		page_table[victim]->shift_reg_bit.push_back('0');
	}

	if (page_table[victim]->dirty_bit == 1)
	{
		disk_write += 1;
		page_table[victim]->dirty_bit = 0;
	}

 	frame.push_back(page_num);
 	frame.erase(frame.begin()+victim_index);

 	//Update fifo queue
	fifo_frame.push_back(page_num);
	fifo_frame.erase(fifo_frame.begin()+victim_index);

	page_table[page_num]->shift_reg_bit[0] = '1';

	if (time_unit_counter % regular_interval==0)
	{	
		//update the register bit of all existed page in frame
		for (int i = 0; i < frame.size(); ++i)
		{	
			if (frame[i]!=-1)
			{
				update_non_use_shift_reg_bit(frame[i]);
			} 	
		}					
	}


}

// output the result
void print_result()
{
	printf("%s%i\n%s%i\n%s%i\n%s%i\n", "events in trace: ",events_in_trace,"total disk reads: ",disk_reads,"total disk writes: ",disk_write,"page faults: ",page_fault);
	
}


// main function
int main(int argc,char *argv[]){

	page_size = atoi(argv[2]);
	frame_number = atoi(argv[3]);
	string read_algo(argv[4]);
	page_replacement_algorithm = read_algo;
	
	if (page_replacement_algorithm.compare("ARB")==0)
	{
		shift_register_bit_size = atoi(argv[5]);
		regular_interval = atoi(argv[6]);

	}

	int max_page_index;
	max_page_index = generate_ref_str("ffffffff", page_size);

	for (int i = 0; i < max_page_index+1; ++i)
	{
		page_table_entry* temp = new page_table_entry();
		page_table.push_back(temp);
	}

	//initialize frames
	for (int i = 0; i < frame_number; ++i){
		frame.push_back(-1);
	}

	// function of reading data from file
	istringstream iss;

	string command;
	string process_id;


	ifstream ifs;
	ifs.open(argv[1], ios::in);

	if (ifs.is_open())
	{
		string input;

		while(getline(ifs, input))
		{	
			istringstream iss (input);
			iss >> command;
			iss >> process_id;
			// skip the first line of text read from file
			if (command.compare("#")==0)
			{	
				continue;
			}
			//increment time unit by 1 per loop
			time_unit_counter += 1;
			//increment events number by 1 per loop
			events_in_trace += 1;
			//convert process id to reference string
			int page_num;
			page_num = generate_ref_str(process_id, page_size);
			//initiate the shift register bit of page
			if (page_table[page_num]->shift_reg_bit.compare("0")==0)
			{
				page_table[page_num]->shift_reg_bit.clear();
				for (int i = 0; i < shift_register_bit_size; ++i)
				{
					page_table[page_num]->shift_reg_bit.push_back('0');
				}
			}
			
			
			page_table[page_num]->last_runnning_time = time_unit_counter;

			// If the page is not in memory, check_page_fault(page_num) returns true
			if (check_page_fault(page_num))
			{
				page_fault += 1;
				disk_reads += 1;
				
				//Check if there is free frame
				if (check_free_space()==false)
				{ 
					if (page_replacement_algorithm.compare("FIFO")==0)
					{	
						first_in_first_out(page_num, command);
					} 
					
					if (page_replacement_algorithm.compare("LRU")==0)
					{	
						least_recent_used(page_num, command);
					} 

					if (page_replacement_algorithm.compare("ARB")==0)
					{	
						additional_ref_bit(page_num, command);
					}
					
				} else 
				{	
					// If there is free frame, allocate the first -1 to page num
					for (int i = 0; i < frame.size(); ++i)
					{	
						if (frame[i]==-1)
						{
							frame[i]=page_num;
							break;
						}
					}

					page_table[page_num]->in_or_v_bit = 1;
					if (command.compare("W")==0)
					{
						page_table[page_num]->dirty_bit = 1;
					}

					fifo_frame.push_back(page_num);

					page_table[page_num]->shift_reg_bit[0] = '1';

					if (page_replacement_algorithm.compare("ARB")==0 && time_unit_counter % regular_interval==0)
					{	
						//update the register bit of all existed page in frame
						for (int i = 0; i < frame.size(); ++i)
						{	
							if (frame[i]!=-1)
							{
								update_non_use_shift_reg_bit(frame[i]); 
							}
						}					
					}

					

				}

			} else { //Page in memory

				if (command.compare("W")==0)
				{
					page_table[page_num]->dirty_bit = 1;
				}
				page_table[page_num]->shift_reg_bit[0] = '1';

				if (page_replacement_algorithm.compare("ARB")==0 && time_unit_counter % regular_interval==0)
				{	
					//update the register bit of all existed page in frame
					for (int i = 0; i < frame.size(); ++i)
					{	
						if (frame[i]!=-1)
						{
							update_non_use_shift_reg_bit(frame[i]); 
						}
					}					
				}

				

				
			}
			
			//print_frame();
		
		}
	}

	print_result();

	return 0;

}






