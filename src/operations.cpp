#include <operations.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

char InstructionsHandler::getInstructionCode(char* instruction) {

	for (int i = 0; i < INSTRUCTION_NUMBER; i++) {
		if (!strcmp(instructions[i]->getMnemonic(), instruction)) {
			return instructions[i]->getOpcode();		
		}	
	}

	return -1;
}

char InstructionsHandler::getRegister(char* reg, bool loadFlag, bool jumpFlag, bool stackFlag) {
	for (char i = 0; i < GPR_NUMBER; i++) {
		if (!strcmp(reg, registers[i]->getName()) || !strcmp(reg, registers[i]->getNameUC())) {			
			return i;		
		}	
	}

	if (jumpFlag) {
		return 12;
	}

	if (loadFlag) {
		if (!strcmp(reg, "pc") || !strcmp(reg, "PC")) {
			return 7;
		}	
	}

	if (stackFlag) {
		if (!strcmp(reg, "psw") || !strcmp(reg, "PSW")) {
			return 8;
		}	

		if (!strcmp(reg, "pmt") || !strcmp(reg, "PMT")) {
			return 9;
		}
	}

	return -1;
}

char InstructionsHandler::getSFRRegister(char* reg) {
	for (char i = 0; i < SFR_NUMBER; i++) {
		if (!strcmp(reg, sfr[i]->getName()) || !strcmp(reg, sfr[i]->getNameUC())) {			
			return i;		
		}	
	}

	return -1;
}

short InstructionsHandler::literalLabelDecoder(char* value, Section* currentSection, bool jumpFlag) {

		Symbol* possibleSymbol = symbolTable->get(value);

		if (possibleSymbol != NULL) {

			if (!possibleSymbol->getLocal()) {

				if (possibleSymbol->getSection() == NULL) {
					if (jumpFlag) {

						currentSection->getRelocationTable()->
							insert(currentSection->getLocalCounter() + 4, R_PC16, possibleSymbol->getOrdinal());
											
					} else {
			
						currentSection->getRelocationTable()->
							insert(currentSection->getLocalCounter() + 4, R_16, possibleSymbol->getOrdinal());
					}

					return 0;

				} else {

					short offset = possibleSymbol->getOffset();

					Symbol* sectionSymbol = symbolTable->get(possibleSymbol->getSection());
		
					Section* section = NULL;

					if (strcmp(sectionSymbol->getLabel(), currentSection->getName())) 
						for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
							if (!strcmp(sectionSymbol->getLabel(), it->second->getName())) {
								section = it->second;
									break;
							}
			
					if (section != NULL) {
				
						if (section->getStartAddress() != -1) {
							offset += section->getStartAddress();
						} else {
							if (jumpFlag) {

								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_PC16, possibleSymbol->getOrdinal());
							
							} else {
			
								currentSection->getRelocationTable()->
										insert(currentSection->getLocalCounter() + 4, R_16, possibleSymbol->getOrdinal());	

							}

						}

					} else {
				
						if (currentSection->getStartAddress() != -1) {
							offset += currentSection->getStartAddress();
						} else {
							if (jumpFlag) {

								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_PC16, possibleSymbol->getOrdinal());

							} else {
			
								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_16, possibleSymbol->getOrdinal());	

							}

						}
					}		

					return offset;
			
				}
 
			} else {

					short offset = possibleSymbol->getOffset();

					Symbol* sectionSymbol = symbolTable->get(possibleSymbol->getSection());
		
					Section* section = NULL;

					if (strcmp(sectionSymbol->getLabel(), currentSection->getName())) 
						for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
							if (!strcmp(sectionSymbol->getLabel(), it->second->getName())) {
								section = it->second;
									break;
							}
			
					if (section != NULL) {
				
						if (section->getStartAddress() != -1) {
							offset += section->getStartAddress();
						} else {
							if (jumpFlag) {

								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_PC16, sectionSymbol->getOrdinal());
							
							} else {
			
								currentSection->getRelocationTable()->
										insert(currentSection->getLocalCounter() + 4, R_16, sectionSymbol->getOrdinal());	

							}

						}

					} else {
				
						if (currentSection->getStartAddress() != -1) {
							offset += currentSection->getStartAddress();
						} else {
							if (jumpFlag) {

								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_PC16, sectionSymbol->getOrdinal());

							} else {
			
								currentSection->getRelocationTable()->
									insert(currentSection->getLocalCounter() + 4, R_16, sectionSymbol->getOrdinal());	

							}

						}
					}		

					return offset;

			}

 		} else {
			
			return atoi(value);

		}
	
}

// 5 bit signed immediate
char InstructionsHandler::getImmediate(char* value) {

	int integerValue = atoi(value);

	if (integerValue < -16 || integerValue > 15) {
		return 100; // error code
	} else {
		return integerValue;
	}

}

// 4 bit unsigned immediate
char InstructionsHandler::getUnsignedImmediate(char* value) {

	int integerValue = atoi(value);

	if (integerValue < 0 || integerValue > 15) {
		return 100; // error code
	} else {
		return integerValue;
	}
}

// 9 bit signed immediate
short InstructionsHandler::getSignedShortImmediate(char* value) {

	if (symbolTable->get(value) == NULL) {	
		int integerValue = atoi(value);

		if (integerValue < -256 || integerValue > 255) {
			return 1000; // error flag	
		} else {
			return integerValue;	
		}
	} else {
		return 1000;
	}
}

// 7 bit signed immediate
char InstructionsHandler::getSignedCharImmediate(char* value) {

	int integerValue = atoi(value);

	if (integerValue < -64 || integerValue > 63) {
		return 100;
	} else {
		return integerValue;
	}
}

int InstructionsHandler::decodeInstructionLine(char* instructionLine, Section* currentSection) {

	char* tokens; char a;// a ? 

	bool addImmediate = false;
	
	if (strstr(instructionLine, ":")) {
		tokens = strtok(instructionLine, ": ");
		tokens = strtok(NULL, ", ");	
	} else {
		tokens = strtok(instructionLine, ", ");
	}

	if (tokens == NULL) return -1;
	char opcode = getInstructionCode(tokens);

	char dest, src, imm = 100; 
	short instruction = 0x0000;
	short literal = 0x0000;
	short shimm = 0;

	switch (opcode) {

		// add, sub, mul, div, cmp
		case 0: case 1: case 2: case 3: case 4:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			imm = getImmediate(tokens);

			instruction = (instruction |= (opcode & 0x1F)) << 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 5;
			instruction |= (imm & 0x1F);

			break;

		// and, or, not, test
		case 5: case 6: case 7: case 8:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, false, false, false);

			instruction = (instruction |= (opcode & 0x1F)) << 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 5;

			break;

		// ldr
		case 9: 
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, true, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			
			literal = literalLabelDecoder(tokens, currentSection, false);			
			addImmediate = true;
		
			instruction = (instruction |= (opcode & 0x1F)) << 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 5;
			instruction |= (0x02 & 0x1F);

			break;

		// str
		case 10:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, true, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			imm = getImmediate(tokens);
			
			instruction = (instruction |= (opcode & 0x1F)) << 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 5;
			instruction |= (imm & 0x1F);


			break;

		case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, true, false);

			if (dest != 12) {
				tokens = strtok(NULL, ", ");

				if (tokens == NULL) return -1;
				imm = getSignedCharImmediate(tokens);
			
			} else {	

				literal = literalLabelDecoder(tokens, currentSection, true);			
				addImmediate = true;		
			
			}

			instruction = (instruction |= (opcode & 0x1F)) << 4;
			instruction = (instruction |= (dest & 0x0F)) << 7;
			
			if (dest != 12)			
				instruction |= (imm & 0x3F);
			else 
				instruction |= (0x02 & 0x3F);

			break;


 		// rey/iret
 		case 88: case 89:
			tokens = strtok(NULL, ", ");

			instruction = (instruction |= (opcode & 0x7F)) << 9;
			
			break;

		// jmp
		case 90:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			shimm = getSignedShortImmediate(tokens);

			if (shimm == 1000) {
				literal = literalLabelDecoder(tokens, currentSection, false);				
				addImmediate = true;
			}		

			instruction = (instruction |= (!addImmediate ? (opcode & 0x7F) : ((opcode & 0x7F) | 0x01))) << 9;
			instruction |= (!addImmediate ? (literal & 0x01FF) : (0x02 & 0x1F));

			break;			

		// push, pop
		case 23: case 24:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, true);

			instruction = (instruction |= (opcode & 0x1F))	<< 4;
			instruction = (instruction |= (dest & 0x0F)) << 7;	


			break;

		// movtosfr, movfromsfr
		case 25: case 26: 
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getSFRRegister(tokens);

			instruction = (instruction |= (opcode & 0x1F))	<< 3;
			instruction = (instruction |= (src & 0x07)) << 2;	
			instruction = (instruction |= (dest & 0x03)) << 6;

			break;

		// mov, in, out
		case 27: case 28: case 29:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, false, false, false);

			instruction = (instruction |= (opcode & 0x1F))	<< 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 5;	

			break;

		// shr, shl
		case 30: case 31: 
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			src = getRegister(tokens, false, false, false);

			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			imm = getUnsignedImmediate(tokens);

			instruction = (instruction |= (opcode & 0x1F))	<< 3;
			instruction = (instruction |= (dest & 0x07)) << 3;
			instruction = (instruction |= (src & 0x07)) << 4;
			instruction = (instruction |= (imm & 0x0F)) << 1;

		break;


	};

	currentSection->insert(((char*)&(instruction))[0]);
	currentSection->insert(((char*)&(instruction))[1]);

	if (addImmediate) {
		
		// jump da se preskoci literal	
		currentSection->insert(0x02);
		currentSection->insert(0xB4);
	
		// literal
		currentSection->insert(((char*)&(literal))[0]);	
		currentSection->insert(((char*)&(literal))[1]);
	}
}

int InstructionsHandler::solveCounterIncrement(char* instructionLine) {

	char* tokens;
	short shimm = 0x0000;

	if (strstr(instructionLine, ":")) {
		tokens = strtok(instructionLine, ": ");
		tokens = strtok(NULL, ", ");	
	} else {
		tokens = strtok(instructionLine, ", ");
	}

	if (tokens == NULL) return -1;
	char opcode = getInstructionCode(tokens);

	char dest;

	switch (opcode) {

		// ldr - uvek 6B
		case 9:	
			return 6;
		
		// uslovni skokovi - format reg, imm : 2B, format label(imm) : 6B
		case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			dest = getRegister(tokens, false, true, false);

			if (dest != 12) {
				return 2;		
			} else {	
				return 6;
			}

		// jmp
		case 90:
			tokens = strtok(NULL, ", ");

			if (tokens == NULL) return -1;
			shimm = getSignedShortImmediateWithoutST(tokens);

			if (shimm == 1000) {
				return 6;
			} else {
				return 2;
			}		
		
		default:
			return 2;
	};
}

void InstructionsHandler::setSymbolTable(SymbolTable* symbolTable) {
	this->symbolTable = symbolTable;
}

short InstructionsHandler::getSignedShortImmediateWithoutST(char* value){
	
	short integerValue = atoi(value);

	if (integerValue == 0) {

		if (strlen(value) == 1) {
			return integerValue;
		} else {
			return 1000;
		}		
	
	} else {
		
		if (integerValue < -256 || integerValue > 255) {
			return 1000; // error flag	
		} else {
			return integerValue;	
		}
	}
}

void InstructionsHandler::setSections(map<char*, Section*>* sections) {
	this->sections = sections;
}
