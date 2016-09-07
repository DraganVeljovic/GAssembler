#include <operations.h>

#include <iostream>
using namespace std;

InstructionsHandler::InstructionsHandler() {

	init();	

}

InstructionsHandler::~InstructionsHandler() {
	for (int i = 0; i < REGISTER_NUMBER; i++)
		delete registers[i];

	delete [] registers;

	for (int i = 0; i < SFR_NUMBER; i++) 
		delete sfr[i];

	delete [] sfr;

	for (int i = 0; i < INSTRUCTION_NUMBER; i++)
		delete instructions[i];

	delete [] instructions;
}

void InstructionsHandler::init() {
	
	registers = new Register*[REGISTER_NUMBER];

	sfr = new Register*[SFR_NUMBER];

	instructions = new Instruction*[INSTRUCTION_NUMBER];

	registers[0] = new Register("r0", "R0");
	registers[1] = new Register("r1", "R1");
	registers[2] = new Register("r2", "R2");
	registers[3] = new Register("r3", "R3");
	registers[4] = new Register("r4", "R4");
	registers[5] = new Register("r5", "R5");
	registers[6] = new Register("r6", "R6");
	registers[7] = new Register("r7", "R7");
	registers[8] = new Register("pc", "PC");
	registers[9] = new Register("sp", "SP");
	registers[10] = new Register("psw", "PSW");
	registers[11] = new Register("pmt", "PMT");

	sfr[0] = new Register("pc", "PC");
	sfr[1] = new Register("sp", "SP");
	sfr[2] = new Register("psw", "PSW");
	sfr[3] = new Register("pmt", "PMT");

	instructions[0] = new Instruction("add", 0);
	instructions[1] = new Instruction("sub", 1);
	instructions[2] = new Instruction("mul", 2);
	instructions[3] = new Instruction("div", 3);
	instructions[4] = new Instruction("cmp", 4);
	instructions[5] = new Instruction("and", 5);
	instructions[6] = new Instruction("or", 6);
	instructions[7] = new Instruction("not", 7);
	instructions[8] = new Instruction("test", 8);
	instructions[9] = new Instruction("ldr", 9);
	instructions[10] = new Instruction("str", 10);
	instructions[11] = new Instruction("je", 11);
	instructions[12] = new Instruction("jne", 12);
	instructions[13] = new Instruction("jge", 13);
	instructions[14] = new Instruction("jg", 14);
	instructions[15] = new Instruction("jle", 15);
	instructions[16] = new Instruction("jl", 16);
	instructions[17] = new Instruction("jp", 17);
	instructions[18] = new Instruction("jn", 18);
	instructions[19] = new Instruction("jo", 19);
	instructions[20] = new Instruction("jno", 20);
	instructions[21] = new Instruction("call", 21);

	// isti opcode razlikuje se ostatak instrukcije
	instructions[22] = new Instruction("ret", 88);
	instructions[23] = new Instruction("iret", 89);
	instructions[24] = new Instruction("jmp", 90);

	instructions[25] = new Instruction("push", 23);
	instructions[26] = new Instruction("pop", 24);
	instructions[27] = new Instruction("movtosfr", 25);
	instructions[28] = new Instruction("movfromsfr", 26);
	instructions[29] = new Instruction("mov", 27);
	instructions[30] = new Instruction("in", 28);
	instructions[31] = new Instruction("out", 29);
	instructions[32] = new Instruction("shr", 30);
	instructions[33] = new Instruction("shl", 31);
}
