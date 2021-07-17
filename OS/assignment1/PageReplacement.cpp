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

int main(int argc,char *argv[]){
	// function of reading data from file
	istringstream iss;
	int page_size;
	int frame_size;
	string page_replacement_algorithm;
	string command;
	string process_id;

	ifstream ifs;
	ifs.open(argv[1], ios::in);

	if (ifs.is_open()){
		string info;
		
		while(getline(ifs, info)){

