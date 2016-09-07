#include <parser.h>
#include <operations.h>

#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>

Parser::Parser() {
	symbolTable = new SymbolTable();

	sections = new map<char*, Section*>();

	instrHandler = new InstructionsHandler();

	startAddress = 0;

}

Parser::~Parser() {
	delete symbolTable;

	delete sections;

	if (writer != NULL)
		delete writer;
	
	delete instrHandler;

}

void Parser::parse(char* inputFile, char* outputFile) {

	ifstream input;

	short instr;

	string readString;
	char* currentRow;
	
	input.open(inputFile, std::ios::in);

	if (input.is_open()) {

		// Prvi prolaz
		while (getline(input, readString)) {

			if (readString.empty()) {
				continue;			
			}
			
			currentRow = new char[readString.size() + 1];
			memcpy(currentRow, readString.c_str(), readString.size());
			currentRow[readString.size()] = '\0';

			if (strstr(currentRow, ".text") || strstr(currentRow, ".data") || strstr(currentRow, ".bss")) {
				char* tokens = strtok(currentRow, " ");

				if (tokens != NULL) {

					bool find = false;

					for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++) {
			
						if (!strcmp(tokens, it->second->getName())) {
							find = true;
						}			

					}

					if (!find) {

						currentSection = new Section(tokens);

						sections->insert(std::pair<char*, Section*>(tokens, currentSection));
		
						symbolTable->insert(tokens, tokens, 0, true, 0, SECTION);

						tokens = strtok(NULL, " ");

						if (tokens != NULL) {
	
							currentSection->setStartAddress(atoi(tokens));
	
						}

					} else {
						// greska
					}

				} else {
					// greska
				}		

				continue;
			}

			if (strstr(currentRow, ".extern")) {
				currentRow += 7;
				
				char* tokens = strtok(currentRow, ", ");
			
				while (tokens != NULL) {
					
					symbolTable->insert(tokens, NULL, 0, false, 0, NO_TYPE);
					tokens = strtok(NULL, ", ");

				}

				continue;
			}

			if (strstr(currentRow, ".global")) {
				continue;
			}

			if (strstr(currentRow, ":")) {
				char* tokens = strtok(currentRow, ": ");

				symbolTable->insert(tokens, currentSection->getName(), currentSection->getLocalCounter(), true, 0, NO_TYPE);

				tokens = strtok(NULL, ": ");

				if (tokens != NULL) {
					currentSection->incrementLocalCounter(2);				
				}

				continue;
			}

			if (strstr(currentRow, ".end")) {
				currentRow += 4;				

				char* tokens = strtok(currentRow, " ");

				if (tokens != NULL) {
					
					Symbol* startSymbol = symbolTable->get(tokens);

					if (startSymbol != NULL) {
						
						if (currentSection->getStartAddress() != -1) {
							startAddress = currentSection->getStartAddress()
								+ startSymbol->getOffset();

						}

					} else {
						// greska

					}

				}				

				break;
			}

			if (strstr(currentRow, ".char")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
			
					while (tokens != NULL) {

						currentSection->incrementLocalCounter(1);

						tokens = strtok(NULL, ", ");					
					}			
	
				} else {
					// greska				
				}

				continue;
			}

			
			if (strstr(currentRow, ".word")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {

					while (tokens != NULL) {

						currentSection->incrementLocalCounter(2);
						
						tokens = strtok(NULL, ", ");
					}
				} else {
					// greska				
				}

				continue;
			}
			
			if (strstr(currentRow, ".skip")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
					
					currentSection->incrementLocalCounter(atoi(tokens));

				} else {
					// greska				
				}

				continue;			
			}
			
			if (strstr(currentRow, ".align")) {
				currentRow += 6;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
				
					int power = atoi(tokens) - 1;
					int alignment = 2 << power;

					int mod = currentSection->getLocalCounter() % alignment;

					tokens = strtok(NULL, ", ");

					if (tokens != NULL) {

						tokens = strtok(NULL, ", ");

						if (tokens != NULL) {

							int maxalign = atoi(tokens);

							if (alignment - mod > maxalign) {
								currentSection->incrementLocalCounter(maxalign);
							} else {
								currentSection->incrementLocalCounter(alignment - mod);
							}

						} else {

							currentSection->incrementLocalCounter(alignment - mod);
						}

					} else {

						currentSection->incrementLocalCounter(alignment - mod);

					}
					

				} else {
					// greska				
				}

				continue;
			}

			currentSection->incrementLocalCounter(instrHandler->solveCounterIncrement(currentRow));

		}	

		// drugi prolaz		
		input.seekg(0, input.beg);

		instrHandler->setSymbolTable(symbolTable);
		instrHandler->setSections(sections);

		for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
			it->second->setLocalCounter(0);

		while (getline(input, readString)) {

			if (readString.empty()) {
				continue;
			}

			currentRow = new char[readString.size() + 1];
			memcpy(currentRow, readString.c_str(), readString.size());
			currentRow[readString.size()] = '\0';

			if (strstr(currentRow, ".end")) {
				break;
			}

			
			if (strstr(currentRow, ".text") || strstr(currentRow, ".data") || strstr(currentRow, ".bss")) {
				char* tokens = strtok(currentRow, " ");
			
				if (tokens != NULL) {

					for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
						if (!strcmp(tokens, it->second->getName())) {
							currentSection = it->second;
							break;
						}				

					if (currentSection == NULL) {
						
						// greska

					}

				} else {
					//greska
				}		

				continue;
			}

			if (strstr(currentRow, ".extern")) {
				continue;
			}

			if (strstr(currentRow, ".global")) {
				currentRow += 7;

				char* tokens = strtok(currentRow, ", ");
				
				while (tokens != NULL) {
					
					Symbol* symbol = symbolTable->get(tokens);

					symbol->setLocal(false);

					tokens = strtok(NULL, ", ");
					
				}

				continue;
			}


			// kao sadrzaj za inicijalizaciju 1B : broj, karakter;
			if (strstr(currentRow, ".char")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
					
					while (tokens != NULL) {

						if (tokens[0] == '\'') {
							currentSection->insert(tokens[1]);
						} else {
							currentSection->insert(atoi(tokens));
						}			

						tokens = strtok(NULL, ", ");						
					}

				} else {
					// greska
									
				}

				continue;
			}


			// kao sadrzaj za inicijalizaciju 2B: broj, adresa labele
			if (strstr(currentRow, ".word")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
					
					while (tokens != NULL) {

						Symbol* symbol = symbolTable->get(tokens);
						
						if (symbol != NULL) {

							if (!symbol->getLocal()) {

								if (symbol->getSection() == NULL) {

									currentSection->getRelocationTable()->
										insert(currentSection->getLocalCounter(), R_16, symbol->getOrdinal());							
									currentSection->insert(0);
									currentSection->insert(0);
								
								} else {
									
									short content = symbol->getOffset();
						
									Symbol* sectionSymbol = symbolTable->get(symbol->getSection());
					
									Section* section = NULL;

									if (strcmp(sectionSymbol->getLabel(), currentSection->getName()))
										for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
											if (!strcmp(sectionSymbol->getLabel(), it->second->getName())) {
												section = it->second;
												break;
											}

						

									if (section != NULL) {
														
										if (section->getStartAddress() != -1) {
											content += section->getStartAddress();
										} else {
											content = 0;				

											currentSection->getRelocationTable()->
												insert(currentSection->getLocalCounter(), R_16, symbol->getOrdinal());
										}

									} else {

										if (currentSection->getStartAddress() != -1) {
											content += currentSection->getStartAddress();
										} else {
											content = 0;

											currentSection->getRelocationTable()->
												insert(currentSection->getLocalCounter(), R_16, symbol->getOrdinal());
										}

									}

									currentSection->insert(((char*)&(content))[0]);
									currentSection->insert(((char*)&(content))[1]);

								}

							} else {
								
								short content = symbol->getOffset();
						
								Symbol* sectionSymbol = symbolTable->get(symbol->getSection());
					
								Section* section = NULL;

								if (strcmp(sectionSymbol->getLabel(), currentSection->getName()))
									for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++)
										if (!strcmp(sectionSymbol->getLabel(), it->second->getName())) {
											section = it->second;
											break;
										}

						

								if (section != NULL) {
														
									if (section->getStartAddress() != -1) {
										content += section->getStartAddress();
									} else {
										currentSection->getRelocationTable()->
											insert(currentSection->getLocalCounter(), R_16, sectionSymbol->getOrdinal());
									}

								} else {

									if (currentSection->getStartAddress() != -1) {
										content += currentSection->getStartAddress();
									} else {
										currentSection->getRelocationTable()->
											insert(currentSection->getLocalCounter(), R_16, sectionSymbol->getOrdinal());
									}

								}

								currentSection->insert(((char*)&(content))[0]);
								currentSection->insert(((char*)&(content))[1]);
							}

						} else {
						
							int content = atoi(tokens);

							currentSection->insert(((char*)&(content))[0]);
							currentSection->insert(((char*)&(content))[1]);

						}
						
						tokens = strtok(NULL, ", ");						
					}

				} else {
					// greska				
				}

				continue;
			
			}

			// za fill atribut: broj ili karakter
			if (strstr(currentRow, ".skip")) {
				currentRow += 5;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
					
					int toSkip = atoi(tokens);
					
					char toFill = 0;

					tokens = strtok(NULL, ", ");

					if (tokens != NULL) {
						
						if (tokens[0] == '\'') {
							toFill = tokens[1];
						} else {
							toFill = atoi(tokens);
						}

					}

					for (int i = 0; i < toSkip; i++) {
						currentSection->insert(toFill);
					}

				} else {
					// greska				
				}

				continue;
			}
			
			// za fill atribut: broj, karakter
			if (strstr(currentRow, ".align")) {
				currentRow += 6;
				
				char* tokens = strtok(currentRow, ", ");

				if (tokens != NULL) {
				
					int power = atoi(tokens) - 1;
					int alignment = 2 << power;

					int mod = currentSection->getLocalCounter() % alignment;

					int escape = alignment - mod;

					char toFill = 0;

					tokens = strtok(NULL, ", ");

					if (tokens != NULL) {

						if (tokens[0] == '\'') {
							toFill = tokens[1];
						} else {
							toFill = atoi(tokens);
						}		

						tokens = strtok(NULL, ", ");

						if (tokens != NULL) {

							int maxalign = atoi(tokens);

							escape = (escape > maxalign) ? maxalign : escape;

						}

						for (int i = 0; i < escape; i++)
							currentSection->insert(toFill);

					} else {
						
						for (int i = 0; i < escape; i++)
							currentSection->insert(toFill);

					}
					

				} else {
					// greska				
				}

				continue;
			}

			instrHandler->decodeInstructionLine(currentRow, currentSection);
		
		}
		
		
	} else {
		// greska	
	}

	// Write

	writer = new ELFWriter(startAddress);

	writer->openOutputStream(outputFile);

	writer->prepareForWrite(symbolTable, sections);

	writer->write();

		
	input.close();
	
}
