#ifndef ELF_H
#define ELF_H

typedef unsigned short Elf32_Half;
typedef unsigned long Elf32_Word;
typedef unsigned long Elf32_Addr;
typedef unsigned long Elf32_Off; 

#include <section.h>
#include <symbolTable.h>
#include <relocationRecord.h>
#include <outputBuffer.h>
#include <stringTableBuilder.h>

#include <fstream>
#include <iostream>

#include <map>

class ELFWriter {
private:	
	static const int EI_NIDENT = 16;
	
	struct ELFHeader{
		unsigned char ident[EI_NIDENT];
		Elf32_Half type;
		Elf32_Half machine;
		Elf32_Word version;
		Elf32_Addr entry; // virtuelna adresa ulazne tacke programa
		Elf32_Off phoff; // offset u fajlu do zaglavlja programa (B)
		Elf32_Off shoff; // offset u fajlu do tabele zaglavlja sekcija
		Elf32_Word flags;
		Elf32_Half ehsize; // velicina ELF zaglavlja (B)
		Elf32_Half phentsize; // velicina jednog ulaza u tabeli zaglavlja programa
		Elf32_Half phnum; // broj ulaza u tabeli zaglavlja programa
		Elf32_Half shentsize; // velicina jednog ulaza u tabeli zaglavlja sekcija
		Elf32_Half shnum; // broj ulaza u tabeli zaglavlja sekcija
		Elf32_Half shstrndx; // index ulaza u tabeli sekcija koji odgovara section name string table 	
				
	} header;

	struct SectionTableEntry {
		Elf32_Word name; // indeks naziva sekicje u tabeli stringova sekicije zaglavlja
		Elf32_Word type;
		Elf32_Word flags;
		Elf32_Addr addr; // virtuelna adresa prvog bajta ukoliko sekcija treba da bude u memoriji u toku izvrsavanja
		Elf32_Off offset; // offset prvog bajta sekcije u odnosu na pocetak fajla (B); 
				//ako sekcija nema sadrzaj u fajlu onda SHT_NOBITS
		Elf32_Word size;
		Elf32_Word link; // posebno znacenje
		Elf32_Word info; // posebno znacenje
		Elf32_Word addralign;
		Elf32_Word entsize; // ne razumem	
	} *sectionTable;

	struct SymbolTableEntry {
		Elf32_Word name; // indeks u tabeli stringova
		Elf32_Addr value; // offset u odnosu na pocetak sekcija
		Elf32_Word size; // velicina simbola ako je poznata ?
		unsigned char info; // visa 4 bita vezivanje, niza 4 bita tip
		unsigned char other; // fiksno 0
		Elf32_Half shndx; // indeks sekcije u tabeli zaglavlja sekcija za koji je posmatrani simbol vezan
	} *symbolTable;

	struct RelocationEntry {
		Elf32_Addr offset;
		Elf32_Word info;	
	}; 
	
	ofstream output;

	OutputBuffer *headerOutput, 
			*symbolOutput,
			**sectionTableOutput,
			**sectionContentOutput;
	
	StringTableBuilder *sectionNameTable, *symbolNameTable;

	int sectionsNumber;

	int relocationSectionsNumber;

	int sectionOffset;

	int startAddress;
		
	void initHeader();

	void initSectionTable();

	void prepareHeaderOutput();

	void createElfSymbolTable(SymbolTable*, map<char*, Section*>*);

	void createElfSectionTable(map<char*, Section*>*);

	void bufferedSectionTable(map<char*, Section*>*);

	void bufferedSectionContent(map<char*, Section*>*);

public:
	ELFWriter(int);

	~ELFWriter();

	void openOutputStream(char* outputFileName);

	void prepareForWrite(SymbolTable*, map<char*, Section*>*);

	void write();
};

#endif
