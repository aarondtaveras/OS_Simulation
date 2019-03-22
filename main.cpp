/**
	Aaron D. Taveras, 10/2/2018
	CSCI 34000, SEC-03, Prof.Schweitzer
	Programming Assignment 1
*/ 


// IF FIELD IS NOT FILLED OUT, DON'T DISPLAY. (NO -1'S OR EMPTY STRINGS.)

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <queue>

int PID_COUNT = 0;

using namespace std;

struct PCB{
	int pid;
	string file_name;
	int mem_location;
	bool read_operation;
	int file_len;
	PCB(){
		pid = PID_COUNT++;
		file_name = "";
		mem_location = 0;
		read_operation = false;
		file_len = 0;
	}
};


int main(int argc, char * argv[])
{
	int num_printers,num_hdd,num_flash;
	/*
		Sys gen and introduction.
	*/
	cout << '\n' << "******* WELCOME TO S(OS)S V 0.0.1 *******"	<< '\n' <<
	"This is the sys gen section. " << '\n' << "Enter the number of printers: " << endl;
	cin >> num_printers;
	if(!cin){
		cout << "That is not a number. Aborting... ";
		return 1;
	}
	cout << "Enter the number of hard disk drives." << endl;
	cin >> num_hdd;
	if(!cin){
		cout << "That is not a number. Aborting...";
		return 1;
	}
	cout << "Enter the number of flash drives." << endl;
	cin >> num_flash;
	if(!cin){
		cout << "That is not a number. Aborting...";
		return 1;
	}
	/*
		Running section. 
	*/
	cout << '\n' << "******* SYS GEN SUCCESSFUL! *******" << '\n' <<
	"Entering running section..." << endl;

	// Create our queues for each device which contain PCB's.

	deque<PCB> ready_q;
	vector<deque<PCB>> printer_q(num_printers), hdd_q(num_hdd), flash_q(num_flash);
	string input;
	char snapshot_char;
	int num_input, index;
	cin.ignore();

	while(true){
		cout << "Issue your command." << endl;
		// cin.ignore('\n');
		getline(cin,input);
		if(input=="A"){
			ready_q.push_back(PCB());
			// cout << "You have just pushed process with PID " << PID_COUNT-1 << " to the ready queue." << endl;
		} 
		else if(input=="t"){
			cout << "Terminating process with pid: " << ready_q.front().pid << endl;
			ready_q.pop_front();
		}	
		else if(input[0]=='d'){
			index = input[1]-'0';
			bool can_insert = true;
			if(index > num_hdd || index <= 0){
				cout << "This disk doesn't exist. Aborting..." << endl;
				return 1; 
			}
			if(!ready_q.empty()){
				PCB temp;
				cout << "Enter the file name." << endl;
				// cin.ignore(100,'\n');
				getline(cin,input);
				temp.file_name = input;
				cout << "Enter the starting location in memory (hex)." << endl;
				cin >> num_input;
				if(!cin){
					cout << "That is not a hex number. Aborting...";
					return 1;
				}
				temp.mem_location = num_input;
			
				cout << "Is this a read/write operation? Enter 'r' or 'w'." << endl;
				cin.ignore(100,'\n');
				getline(cin,input);
				if(input.size()!=1){
					cout << "Please enter 'r' or 'w' ONLY." << endl;
					break;
				} else {
					input[0]=='r' ? temp.read_operation = true : input[0];
					input[0]=='w' ? temp.read_operation = false : input[0];
				}
				if(!temp.read_operation){
					cout << "Enter the size of the file you are writing to (in hex)." << endl;
					cin >> num_input;
					if(!cin){
						cout << "That is not a hex number. Aborting...";
						return 1;
					}
					temp.file_len = num_input;
					cin.ignore(100,'\n');
				}
				// check if mem location valid
				for(int i=0;i<num_hdd;i++){
					for(PCB p: hdd_q[i]){
						if(temp.mem_location >= p.mem_location && temp.mem_location <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(temp.mem_location + temp.file_len >= p.mem_location && temp.mem_location + temp.file_len <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(p.mem_location >= temp.mem_location && p.mem_location <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
						else if(p.mem_location + p.file_len >= temp.mem_location && p.mem_location + p.file_len <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
					}
				}

				if(can_insert){
					cout << " *** NEW PROCESS *** " << "\n" << temp.pid << "\n" <<
					temp.file_name << "\n" << temp.mem_location << "\n" << 
					temp.file_len << endl;
					hdd_q[index-1].push_back(temp);
				} else cout << "This memory location is occupied. Unable to add process." << endl;
				
			} else {
				cout << "Ready queue is empty. Please add process first." << endl;
			}

		}
		else if(input[0]=='D'){
			index = input[1]-'0';
			if(index > num_hdd || index <= 0){
				cout << "This disk doesn't exist. Aborting..." << endl;
				return 1; 
			} else if(hdd_q[index-1].empty()){
				cout << "This disk's queue is empty." << endl;
			} else {
				cout << "Process with PID " << hdd_q[index-1].front().pid << " has been completed." << endl;
				PCB temp = hdd_q[index-1].front();
				hdd_q[index-1].pop_front();
				temp.file_name = "";
				temp.mem_location = -1;
				temp.read_operation = false;
				temp.file_len = -1;
				ready_q.push_back(temp);
			}
			
		}
		else if(input[0]=='p'){
			// Receive file name...
			index = input[1]-'0';
			bool can_insert;
			if(index > num_hdd || index <= 0){
				cout << "This printer doesn't exist. Aborting..." << endl;
				return 1; 
			}
			if(!ready_q.empty()){
				PCB temp;
				cout << "Enter the file name." << endl;
				getline(cin,input);
				temp.file_name = input;
				cout << "Enter the starting location in memory (hex)." << endl;
				cin >> num_input;
				if(!cin){
					cout << "That is not a hex number. Aborting...";
					return 1;
				}
				temp.mem_location = num_input;
				// Printers can only write - no reads!
				cout << "Enter the size of the file you are writing to (in hex)." << endl;
				cin >> num_input;
				if(!cin){
					cout << "That is not a hex number. Aborting...";
					return 1;
				}
				temp.file_len = num_input;
				cin.ignore(100,'\n');
				// check if mem location valid
				for(int i=0;i<num_printers;i++){
					for(PCB p: printer_q[i]){
						if(temp.mem_location >= p.mem_location && temp.mem_location <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(temp.mem_location + temp.file_len >= p.mem_location && temp.mem_location + temp.file_len <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(p.mem_location >= temp.mem_location && p.mem_location <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
						else if(p.mem_location + p.file_len >= temp.mem_location && p.mem_location + p.file_len <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
					}
				}

				if(can_insert){
					cout << " *** NEW PROCESS *** " << "\n" << temp.pid << "\n" <<
					temp.file_name << "\n" << temp.mem_location << "\n" << 
					temp.file_len << endl;
					printer_q[index-1].push_back(temp);	
				} else cout << "This memory location is occupied. Unable to add process." << endl;  
			}
			else {
				cout << "Ready queue is empty. Please add process first." << endl;
			}
			
		}
		else if(input[0]=='P'){
			index = input[1]-'0';
			if(index > num_hdd || index <= 0){
				cout << "This printer doesn't exist. Aborting..." << endl;
				return 1; 
			} else if(printer_q[index-1].empty()){
				cout << "This printer's queue is empty." << endl;
			} else {
				cout << "Process with PID " << printer_q[index-1].front().pid << " has been completed." << endl;
				PCB temp = printer_q[index-1].front();
				printer_q[index-1].pop_front();
				temp.file_name = "";
				temp.mem_location = -1;
				temp.read_operation = false;
				temp.file_len = -1;
				ready_q.push_back(temp);
			}
			
		}
		else if(input[0]=='f'){
			index = input[1]-'0';
			bool can_insert;
			if(index > num_hdd || index <=0){
				cout << "This flash drive doesn't exist. Aborting..." << endl;
				return 1; 
			}
			if(!ready_q.empty()){
				PCB temp;
				cout << "Enter the file name." << endl;
				cin.ignore(100,'\n');
				getline(cin,input);
				temp.file_name = input;
				cout << "Enter the starting location in memory (hex)." << endl;
				cin >> num_input;
				if(!cin){
					cout << "That is not a hex number. Aborting...";
					return 1;
				}
				temp.mem_location = num_input;
				cout << "Is this a read/write operation? Enter 'r' or 'w'." << endl;
				cin.ignore(100,'\n');
				getline(cin,input);
				if(input.size()!=1){
					cout << "Please enter 'r' or 'w' ONLY." << endl;
					break;
				} else {
					input[0]=='r' ? temp.read_operation = true : input[0];
					input[0]=='w' ? ready_q.front().read_operation = false : input[0];
				}
				if(!temp.read_operation){
					cout << "Enter the size of the file you are writing to (in hex)." << endl;
					cin >> num_input;
					if(!cin){
						cout << "That is not a hex number. Aborting...";
						return 1;
					}
					cin.ignore(100,'\n');
					temp.file_len = num_input;
				}

				// see if mem is occupied 
				for(int i=0;i<num_flash;i++){
					for(PCB p: flash_q[i]){
						if(temp.mem_location >= p.mem_location && temp.mem_location <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(temp.mem_location + temp.file_len >= p.mem_location && temp.mem_location + temp.file_len <= p.mem_location + p.file_len){
							can_insert = false;
						}
						else if(p.mem_location >= temp.mem_location && p.mem_location <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
						else if(p.mem_location + p.file_len >= temp.mem_location && p.mem_location + p.file_len <= temp.mem_location + temp.file_len){
							can_insert = false;
						}
					}
				}

				if(can_insert){
					cout << " *** NEW PROCESS *** " << "\n" << temp.pid << "\n" <<
				 	temp.file_name << "\n" << temp.mem_location << "\n" << 
					temp.file_len << endl;

					flash_q[index-1].push_back(temp);
				} else cout << "This memory location is occupied. Unable to add process." << endl;
					
			}
			else {
				cout << "Ready queue is empty. Please add a process first." << endl;
			}

		}
		else if(input[0]=='F'){
			index = input[1]-'0';
			if(index > num_hdd || index <= 0){
				cout << "This flash drive doesn't exist. Aborting..." << endl;
				return 1; 
			} else if(flash_q[index-1].empty()){
				cout << "This flash drive's queue is empty." << endl;
			} else {
				cout << "Process with PID " << flash_q[index-1].front().pid << " has been completed." << endl;
				PCB temp = flash_q[index-1].front();
				flash_q[index-1].pop_front();
				temp.file_name = "";
				temp.mem_location = -1;
				temp.read_operation = false;
				temp.file_len = -1;
				ready_q.push_back(temp);
			}
			
		}
		else if(input=="S"){
			cout << "Enter your decision for the snapshot operation: 'r'| 'p' | 'd' | 'f' " << endl;
			cin >> snapshot_char;
			switch(snapshot_char){
				case'r':
					for(PCB p: ready_q){
						cout << "PID: " << p.pid << "\n" << "FILE NAME: " <<  p.file_name << "\n" << "MEM LOCATION: "<< p.mem_location << "\n" << 
						"READ OPERATION? " << p.read_operation << "\n" << "FILE LEN: " << p.file_len << endl;
					}
					break;
				case'p':
					for(int i=0;i<num_printers;i++){
						cout << "PRINTER " << i+1 << " : " << endl;
						for(PCB p: printer_q[i]){
							if(!printer_q[i].empty()){
								cout << "PID: " << p.pid << "\n" << "FILE NAME: " <<  p.file_name << "\n" << "MEM LOCATION: "<< p.mem_location << "\n" << 
								 "FILE LEN: " << p.file_len << endl;
							}
						}
					}
					break;
				case'd':
					for(int i=0;i<num_hdd;i++){
						cout << "DISK " << i+1 << " : " << endl;
						for(PCB p: hdd_q[i]){
							if(!hdd_q[i].empty()){
								cout << "PID: " << p.pid << "\n" << "FILE NAME: " <<  p.file_name << "\n" << "MEM LOCATION: "<< p.mem_location << "\n" << 
								"READ OPERATION? " << p.read_operation << "\n" << "FILE LEN: " << p.file_len << endl;	
							}
						}
					}
					break;
				case'f':
					for(int i=0;i<num_flash;i++){
						cout << "FLASH " << i+1 << " : " << endl;
						for(PCB p: flash_q[i]){
							if(!flash_q[i].empty()){
								cout << "PID: " << p.pid << "\n" << "FILE NAME: " <<  p.file_name << "\n" << "MEM LOCATION: "<< p.mem_location << "\n" << 
								"READ OPERATION? " << p.read_operation << "\n" << "FILE LEN: " << p.file_len << endl;
							}
						}
					}
					break;
				default:
					cout << "Invalid input for snapshot command." << endl;
					break;
			}
			cin.ignore(100,'\n');
		}
		else {
			cout << input << endl;
			cout << "Invalid/null command entered." << endl;
		}
	}
	// Ready queue is now empty, and program terminates.
	return 0;
}
