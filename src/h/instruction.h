#ifndef INSTRUCTION_H
#define INSTRUCTION_H

class Instruction {
private:
	char* mnemonic;
	char opcode;

public:
	Instruction(char* mnemonic, char opcode);

	Instruction();

	~Instruction();

	char getOpcode();

	char* getMnemonic();
	
	void setMnemonic(char* mnemonic);

	void setOpcode(char opcode); 
};

#endif
