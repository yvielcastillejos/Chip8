#include "chip8.hpp"
#include <iostream>
#include <stdint.h>
#include <cstring>
using namespace std;

Chip8::Chip8(Display* display){
	this->display = display;
	PGR_loaded = false;
}


void Chip8::Chip8_clear(){
	/* This Function will reset the values*/
	//this-> waitingForInput = updateDisplay = false;
	this->I = 0;
	this->SP = 0;
	this->delay_timer = 0;
	this->sound_timer = 0;
	this->PC = 0x200;
	memset(this->Stack, 0, sizeof(Stack));
        memset(this->V, 0, sizeof(V));
	(this->display) -> clear();
	(this->speaker).Stop();
}

void Chip8::push(uint16_t v){
	// Increment stack pointer by 1
	SP = SP++
	// Add the address at the top of the stack
	Stack[SP] = v;
	// get the first four bits
	SP = SP & 0x0F;
}

void Chip8::pop(){
	SP = SP--;
	uint16_t output = Stack[SP];
	SP = SP & 0x0F;
	// return from the subroutine in the stack
	PC = output;
}

const uint8_t font_sprites[0x80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F    
};

void Chip8::pwr(){
	Chip8_clear();
	memset(MEMORY, 0, sizeof(MEMORY));
	memcpy( MEMORY, font_sprites, sizeof(font_sprites));
}

char Chip8::updateInput(){
	return keypad.update();
}

void Chip8::load(const char* file_path){
	char readByte;
	uint16_t currentAddress = PGR_START;
	std::ifstream file(path, std::ios::in|std::ios::binary);

    if( file.is_open() ) {
        while( !file.eof() ) {
            file.get( readByte );
            MEMORY[ currentAddress ] = (int)readByte;
            currentAddress++; 
        }
        PGR_loaded = true;
        file.close();
    }
    
    return;

}

void Chip8::select(uint16_t ins){
	/*nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
n or nibble - A 4-bit value, the lowest 4 bits of the instruction
x - A 4-bit value, the lower 4 bits of the high byte of the instruction
y - A 4-bit value, the upper 4 bits of the low byte of the instruction
kk or byte - An 8-bit value, the lowest 8 bits of the instruction
*/
	uint16_t addr = (ins)&0x0FFF;
	uint8_t n = ins & 0x000F;
	uint8_t x = (ins>>8) & 0x000F;
	uint8_t y = (ins>>4) & 0x000F;
	uint8_t byte = opcode & 0x00FF;


	/* SEE INSTRUCTIONS AT http://devernay.free.fr/hacks/chip8/C8TECH10.HTM */
	switch((ins & 0xF000)>>12){
		case 0:{
			if (byte == 0xE0){
				// 00E0 CLS clear the display
				display -> clear();
				updateDisplay = true;
			}
			else if (byte == 0xEE){
				// RETReturn from a subroutine.
				pop();
			}
			break;
		}
		case 1:{
			// Jump addr to first NNN of opcode (-1 because of stack defn)
			PC = addr-1;
			break;
		}
		case 2:{/* Call subroutine at nnn. The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.*/
			// place program counter into stack
			push(PC);
			PC = addr-1;
			break;
		}
		case 3:{//Skip next instruction if Vx = kk.
			if (V[x] == byte){
				PC = PC + 2;
			break;
			}
		}
                case 4:{//Skip next instruction if Vx != kk.
                        if (V[x] != byte){
                                 PC = PC + 2;
                        }
			break;
		}
                case 5:{
			//Skip next instruction if Vx = Vy.
                         if (V[x] == V[y]){
                                  PC = PC + 2;
                         }
                         break;
		}
                case 6:{
			V[x] = byte;
			break;
		}
                case 7:{
			V[x] = V[x] + byte;
			break;
		}
                case 8:{
			switch(n){
				case 0:{
					V[x] = V[y];
					break;
					}
				case 1:{
                                        V[x] = V[x] | V[y];
                                        break;
                                        }
                                case 2:{
                                        V[x] = V[x] & V[y];
                                        break;
                                        }
                                case 3:{
                                        V[x] = V[x] ^ V[y];
                                        break;
                                        }
				case 4:{
                                        unsigned sum  = V[x] + V[y];
					if (sum > 255){
						V[0xF] = 1; 
					}
					else{
						V[0xF] = 0;
					}
					V[x] = result & 0xFF;
                                        break;
                                        }
                                case 5:{
                                        V[0xF] = V[x] > V[y];
					V[x] = V[x] - V[y];
                                        break;
                                        }
                                case 6:{
                                        V[0xF] = (V[x] & 0x01) > 0;
					V[x] = V[x] / 2;
                                        break;
                                        }
                                case 7:{
                                        V[0xF] = V[y] > V[x];  
					V[x] = V[y] - V[x];        
                                        break;
                                        }
                                case 0xE:{
                                        V[0xF] = (V[x] & 0x80) > 0;
					V[x] = V[x] * 2;
                                        break;
                                        }
				}
			}
			break;
		}
		case 9: {
			if( V[x] != V[y] ) {
				PC = PC+2;
			break;
			}
		}
		case 0xA: {
			I = addr;
			break;
		}
		case 0xB: {
			PC = addr + V[0] - 1;
			break;
		}
		
		 case 0xC: {
			int random_number = rand()%255;
			V[x] = random_number & byte;
			break;
		}







}
