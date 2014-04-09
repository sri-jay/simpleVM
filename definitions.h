#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <future>
#include <chrono>
#include <string>
#include <utility>
#include <atomic>
#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <Windows.h>

/*Defining instruction types*/
#define START "first_instruction_@_start_of_program"
#define MOVIR "move_immediate_value_to_register"
#define MOVRR "move_register_one_value_to_register_two"
#define ADDIR "add_immediate_value_to_value_in_rgister"
#define ADDRR "add_register_one_value_to_register_two_value_and_store_in_register_two"
#define STOP  "last_instruction_@_end_of_program"

/*Defining Registers as register[] indexes*/
#define REGISTER_ONE 1
#define REGISTER_TWO 2
#define REGISTER_THREE 3
#define REGISTER_FOUR 4
