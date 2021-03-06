#include <iostream>
#include <stdint.h>



// Start address of Chip-8
const uint16_t START_ADDR = 0x200;
const uint16_t START_FONT = 0x0;


// Chip-8 language is capable of accessing up to 4KB of RAM
const unsigned int MEM_SIZE = 4096;
// The stack is used to store the address the interpreter should return to and is an array of 16 16 bit values
const unsigned int STACK_SIZE = 16;
const unsigned int NUM_REG = 16;

class Chip8{
	public:
		Chip8(Display* display);
		void pwr();
		// For resetting
		void Chip8_clear();
        	void load(const char* path);
        	void select(uint16_t ins);
        	void Cycle();
		void timeupdate();

	protected:
		Display* display;
		Keypad keypad;
		Speaker speaker;

		uint8_t MEMORY[MEM_SIZE];
                // Stack pointer (8 bits) points to the topmost level of the stack
		uint8_t SP;
		// we have 16 16 bit stacks for storing the addr the interpreter should return to when finished running a function
		uint16_t Stack[STACK_SIZE];

		// Register
		uint8_t V[NUM_REG];
		// I is 16 bit register used to store memory addr
		uint16_t I;
		
		// Program counter - 16 bit used to store the currently executing address
		uint16_t PC;
		unsighed short opcode;
		

		// Timers Registers
		uint8_t delay_timer;
		uint8_t sound_timer;

		
		bool PGR_loaded;
	//	bool waitingForInput;
	//	bool updateDisplay;
		unsigned accum_time; //Accums delta time between frames, for timers update

		//stack helpers
		void push(uint16_t v);
		void pop();  
		};
