#include <instruction.h>

#include <cstring>

Instruction::Instruction() {
	mnemonic = NULL;
}

Instruction::Instruction(char* mnemonic, char opcode) {

	this->mnemonic = new char[strlen(mnemonic) + 1];
	strcpy(this->mnemonic, mnemonic);

	this->opcode = opcode;
}

Instruction::~Instruction() {
	if (mnemonic != NULL) 
		delete [] mnemonic;
}

char Instruction::getOpcode() {
	return opcode;
}

char* Instruction::getMnemonic() {
	return mnemonic;
}

void Instruction::setMnemonic(char* mnemonic) {
	this->mnemonic = new char[strlen(mnemonic) + 1];
	strcpy(this->mnemonic, mnemonic);
}

void Instruction::setOpcode(char opcode) {
	this->opcode = opcode; 
}

