#ifndef PARSER_H
#define PARSER_H

#include <symbolTable.h>
#include <section.h>
#include <operations.h>
#include <elf.h>

using namespace std;

class Parser {
	private:

		SymbolTable* symbolTable;

		map<char*, Section*>* sections;
	
		Section* currentSection;

		int startAddress;

		InstructionsHandler* instrHandler;

		ELFWriter* writer;

	public:

		Parser();

		~Parser();

		void parse(char* inputFile, char* outputFile);
};

#endif
