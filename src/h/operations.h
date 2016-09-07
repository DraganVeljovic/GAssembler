#ifndef OPERATIONS_H
#define OPERATIONS_H

#define SFR_NUMBER 4
#define GPR_NUMBER 8
#define REGISTER_NUMBER 12
#define INSTRUCTION_NUMBER 34

#include <symbolTable.h>
#include <section.h>
#include <relocationRecord.h>
#include <register.h>
#include <instruction.h>

class InstructionsHandler {
private:
	SymbolTable* symbolTable;

	map<char*, Section*>* sections;

	void init();

public:
	InstructionsHandler();

	~InstructionsHandler();	 
		
	Register** registers;

	Register** sfr;

	Instruction** instructions;

	void setSymbolTable(SymbolTable*);

	void setSections(map<char*, Section*>*);

	char getInstructionCode(char*);

	char getRegister(char*, bool, bool, bool);

	char getSFRRegister(char*);

	short literalImmediate(char*, Section*);

	short literalLabelDecoder(char*, Section*, bool);

	char getImmediate(char*);

	char getUnsignedImmediate(char*);
	
	char getSignedCharImmediate(char*);

	short getSignedShortImmediate(char* value); 

	short getSignedShortImmediateWithoutST(char*);

	int solveCounterIncrement(char*);

	int decodeInstructionLine(char*, Section*);

};

#endif
