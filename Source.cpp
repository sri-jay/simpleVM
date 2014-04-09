#include "definitions.h"

using namespace std;

/*Defining instruction classes and instruction executors*/
class instruction
{
private:
	string instruction_;
	int *data;

public:
	instruction()
	{
		instruction_ = START;
		data = new int[2];
	}

	instruction(string instruct_ = START, int argi=0, int argj=0)
	{	
		data = new int[2];

		instruction_ = instruct_;
		data[0] = argi;
		data[1] = argj;
	}
	
	instruction(const instruction& copy_this_object)
	{
		data = new int[2];

		instruction_ = copy_this_object.instruction_;
		data[0] = copy_this_object.data[0];
		data[1] = copy_this_object.data[1];
	}

	string get_instruction()
	{
		return instruction_;
	}

	int* get_arguments()
	{
		return data;
	}

};
/*Defing a VM which runs on a thread of its own*/
class VM_BASE_DATA
{
	public:
		int registers[4];
		vector<instruction> instruction_set;

		VM_BASE_DATA()
		{
			VM_INIT();
		}

		void VM_INIT()
		{
			for (int x : registers)
			{
				x = 0;
			}
		}
		

		vector<instruction> get_program_code()
		{
			return instruction_set;
		}
};

/*class execute instruction executes instructions inside the VM*/
class VM: public VM_BASE_DATA
{
	private:
		int program_counter;

	public:
		VM()
		{
			program_counter = 0;
		}

		/*Get next instruction and update Program Counter*/
		instruction fetch_next_instruction()
		{
			instruction temp = 	instruction_set[program_counter];
			program_counter++;
			return temp;
		}

		void get_register_contents()
		{
			cout << "Register # :" << REGISTER_ONE << " " << registers[0] << endl;
			cout << "Register # :" << REGISTER_TWO << " " << registers[1] << endl;
			cout << "Register # :" << REGISTER_THREE << " " << registers[2] << endl;
			cout << "Register # :" << REGISTER_FOUR << " " << registers[3] << endl;
			cout << endl << endl;
		}

		int get_program_counter()
		{
			return program_counter;
		}
		
		/*Run on thread*/
		void vm_start_executing(bool do_stack_trace = false,int process_speed = 2000)
		{
			cout << "INSIDE" << endl;
			bool is_running = true;
			int snap_ = 0;

			while (is_running)
			{
				snap_++;

				instruction INSTRUCTION = (instruction)fetch_next_instruction();
				if (do_stack_trace)
				{	
					cout << "Snapshot @ cycle # " << snap_ << endl;
					get_register_contents();
				}

				//yes _ in front of sleep
				Sleep(process_speed);

				is_running = execute_manager(INSTRUCTION);
			}

			//Stopped execution
		}
		/*Get instruction and execute*/
		bool execute_manager(instruction& exec_)
		{
			bool is_running = true;
		
			if (exec_.get_instruction() == START)
			{
				is_running = true;
			}

			if (exec_.get_instruction() == MOVIR)
			{
				movlr(exec_.get_arguments());
			}

			if (exec_.get_instruction() == MOVRR)
			{
				movrr(exec_.get_arguments());
			}

			if (exec_.get_instruction() == ADDIR)
			{
				addir(exec_.get_arguments());
			}
			if (exec_.get_instruction() == ADDRR)
			{
				addrr(exec_.get_arguments());
			}

			if (exec_.get_instruction() == STOP)
			{
				is_running = false;
			}

			return is_running;
		}

		/*Functions*/
		void movlr(int *data)
		{
			int literal = data[0];
			registers[data[1]] = literal;
		}
		void movrr(int *data)
		{
			int data_reg_one = registers[data[0]];
			int data_reg_two = registers[data[1]];
			registers[data[1]] = data_reg_one;
		}
		void addir(int *data)
		{
			int literal = data[0];
			int register_data = registers[data[1]];
			register_data = literal + register_data;
			registers[data[1]] = register_data;
		}
		void addrr(int *data)
		{
			int data_one = registers[data[0]];
			int data_two = registers[data[1]];

			registers[data[1]] = data_one + data_two;
		}
		
};

class hypervisor : public VM
{	
	private:
	
		vector< pair< 

			pair< future<void> ,int > , 
			pair< future<int>,promise<int> > 
		
		> > thread_pool;

		map<char, string> hypervisor_script;
		vector< pair<VM,int> > virtual_machines;
		int vm_instance_count;
		bool is_running;

	public:
		hypervisor()
		{
			vm_instance_count = 0;
			console_setup();
			is_running = true;
			hypervisor_console();
		}

		int create_VM_instance()
		{
			VM vm_instance;
			vm_instance_count++;

			pair<VM,int> vm_instance_ = make_pair(vm_instance, vm_instance_count);

			virtual_machines.push_back(vm_instance_);

			return vm_instance_count;
		}

		void memory_image(VM &vm_instance)
		{
			vm_instance.get_register_contents();
			int program_counter = vm_instance.get_program_counter();

			cout << program_counter << endl;
		}

		void load_client_program_in_instance()
		{
			int id;
			bool defined = true;

			cout << "Enter the vm instance Id#" << endl;
			get_vm_instances();

			cin >> id;

			for (pair<VM, int> &vm_instance : virtual_machines)
			{
				if (vm_instance.second == id)
				{
					load_client_program(vm_instance);
					defined = true;
					break;
				}
				else
				{
					defined = false;
				}
			}

			if (defined)
			{
				cout << "Instance has been Loaded with the program" << endl;
				get_vm_instances();
			}
			
		}

		void load_client_program(pair<VM,int> &vm_instance)
		{
			cout << "Register contents Before Load" << endl;
			memory_image(vm_instance.first);

			instruction start(START, 0, 0);
			instruction stop(STOP, 0, 0);		
			instruction a(MOVIR, 100, REGISTER_ONE);
			instruction b(MOVIR, 100, REGISTER_TWO);
			instruction c(ADDRR, REGISTER_ONE, REGISTER_TWO);

			cout << "Register contents after load" << endl;
			memory_image(vm_instance.first);

			vm_instance.first.instruction_set.push_back(start);
			vm_instance.first.instruction_set.push_back(a);
			vm_instance.first.instruction_set.push_back(b);
			vm_instance.first.instruction_set.push_back(c);
			vm_instance.first.instruction_set.push_back(stop);

			run_VM_program(vm_instance);

			cout << "Register contents after program execution" << endl;
			memory_image(vm_instance.first);
		}

		//&vm_instance is important, otherwise copy constructor, not same object!!!!
		void run_VM_program(pair<VM,int> &vm_instance, bool stack_trace = true)
		{
			
				//Promise thread that ill send data in future
				promise<int> thread_promise;
				future<int> thread_data_channel = thread_promise.get_future();

				//Async task, using <future>, creates new thread to execute
				//launch::async force creates new thread

				//vm_instance.first.vm_start_executing(true,500);

				auto thread_future = async(launch::async, &VM::vm_start_executing, vm_instance.first,stack_trace,500);

				thread_future.get();
			
			
		}

		void get_vm_instances()
		{
			cout << "Active VM :" << endl;
			for (pair<VM,int> vm_instance : virtual_machines)
			{
				cout << "Instance Id# :" << vm_instance.second << endl;
			}
		}

		void hypervisor_console()
		{
			char opt;

			while (is_running)
			{
				cout << "Enter a command or press O to show a list of commands" << endl;
				cin >> opt;
				
				switch (opt)
				{
					case 'c': {

							int id = create_VM_instance();
							cout << "VM created with ID#" << id << endl;
							break;

					}

					case 'l': get_vm_instances();
						break;

					case 'L': load_client_program_in_instance();
						break;

				}
			}
		}

		void console_setup()
		{
			hypervisor_script['c'] = "Create a VM";
			hypervisor_script['l'] = "List Virtual Machines";

			hypervisor_script['L'] = "Load a VM with a program";
			
			hypervisor_script['O'] = "List commands";
			hypervisor_script['K'] = "Exit";
		}
};		

int main()
{
	hypervisor hyp;

	cin.get();
	cin.get();
}