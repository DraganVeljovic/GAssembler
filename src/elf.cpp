#include <elf.h>

#include <cstring>
#include <map>

ELFWriter::ELFWriter(int startAddress) {
	sectionsNumber = 0;

	relocationSectionsNumber = 0;

	this->startAddress = startAddress;

	initHeader();
}

ELFWriter::~ELFWriter() {
	if (symbolTable != NULL) delete [] symbolTable;
	if (sectionTable != NULL) delete [] sectionTable;

	if (headerOutput != NULL) delete headerOutput;
	if (symbolOutput != NULL) delete symbolOutput;
	if (sectionTableOutput != NULL) {
		
		for (int i = 0; i < sectionsNumber; i++) {
			if (sectionTableOutput[i] != NULL) delete sectionTableOutput[i];
		}	
	
		delete [] sectionTableOutput;	
	}
	if (sectionContentOutput != NULL) {
		
		for (int i = 0; i < sectionsNumber - 4; i++) {
			if (sectionContentOutput[i] != NULL) delete sectionContentOutput[i];
		}	
	
		delete [] sectionContentOutput;	
	}

	if (sectionNameTable != NULL) delete sectionNameTable;
	if (symbolNameTable != NULL) delete symbolNameTable;

	if (output.is_open()) {
		output.close();	
	}

}

void ELFWriter::initHeader() {
	header.ident[0] = 0x7f;
	header.ident[1] = 0x45;
	header.ident[2] = 0x4c;
	header.ident[3] = 0x46;
	header.ident[4] = 0x01;
	header.ident[5] = 0x01;
	header.ident[6] = 0x01;
	header.ident[7] = 0x00;
	header.ident[8] = 0x00;
	header.ident[9] = 0x00;
	header.ident[10] = 0x00;
	header.ident[11] = 0x00;
	header.ident[12] = 0x00;
	header.ident[13] = 0x00;
	header.ident[14] = 0x00;
	header.ident[15] = 0x00;

	header.type = 0x0001;
	header.machine = 0x0003;
	header.version = 0x00000001;
	header.entry = startAddress;
	header.phoff = 0;
	header.shoff = 0; // update posle postavljanja sadrzaja - reseno
	header.flags = 0;
	header.ehsize = 0x0034;
	header.phentsize = 0;
	header.phnum = 0;
	header.shentsize = 0x0028;
	header.shnum = 0; // update nakon kreiranju sht - reseno
	header.shstrndx = 0; // update nakon kreiranja sht - reseno
}

void ELFWriter::openOutputStream(char* outputFileName) {
	output.open(outputFileName, ios::out | ios::binary);
}

void ELFWriter::prepareForWrite(SymbolTable* symbolTable, map<char*, Section*>* sections) {

	createElfSectionTable(sections);

	createElfSymbolTable(symbolTable, sections);

	bufferedSectionContent(sections);

	bufferedSectionTable(sections);

	prepareHeaderOutput();

}

void ELFWriter::prepareHeaderOutput() {
	headerOutput = new OutputBuffer(52);

	headerOutput->put((char*)&header.ident, 16);

	headerOutput->put((char*)&header.type, 2);

	headerOutput->put((char*)&header.machine, 2);

	headerOutput->put((char*)&header.version, 4);

	headerOutput->put((char*)&header.entry, 4);

	headerOutput->put((char*)&header.phoff, 4);

	headerOutput->put((char*)&header.shoff, 4);

	headerOutput->put((char*)&header.flags, 4);

	headerOutput->put((char*)&header.ehsize, 2);

	headerOutput->put((char*)&header.phentsize, 2);

	headerOutput->put((char*)&header.phnum, 2);

	headerOutput->put((char*)&header.shentsize, 2);
	
	headerOutput->put((char*)&header.shnum, 2);

	headerOutput->put((char*)&header.shstrndx, 2);
}

void ELFWriter::createElfSymbolTable(SymbolTable* symbolTable, map<char*, Section*>* sections) {
	
	this->symbolTable = new SymbolTableEntry[symbolTable->getTable()->size()];
	
	this->symbolOutput = new OutputBuffer(16 * symbolTable->getTable()->size());

	symbolNameTable = new StringTableBuilder();

	Elf32_Word i = 0;	

	for (map<char*, Symbol*>::iterator it = symbolTable->getTable()->begin(); it != symbolTable->getTable()->end(); it++) {

			this->symbolTable[i].name = symbolNameTable->addString(it->second->getLabel());
			this->symbolTable[i].value = it->second->getOffset();
			this->symbolTable[i].size = 0;

			char info = (it->second->getLocal() ? 0x00 : 0x10) << 4;
			info |= ((it->second->getType()) & 0x0f);

			this->symbolTable[i].info = info; 
			this->symbolTable[i].other = 0;
		
			if (it->second->getSection() == NULL) {
				this->symbolTable[i].shndx = 0;
			} else {
				for (map<char*, Section*>::iterator sit = sections->begin(); sit != sections->end(); sit++) {
					if (!strcmp(sit->second->getName(), it->second->getSection()))			
						this->symbolTable[i].shndx = sit->second->getSectionIndex();
				}
			}

			// put to outputBuffer
		
			symbolOutput->put((char*)&(this->symbolTable[i].name), 4);	
		
			symbolOutput->put((char*)&(this->symbolTable[i].value), 4);	

			symbolOutput->put((char*)&(this->symbolTable[i].size), 4);		

			symbolOutput->put(this->symbolTable[i].info);	

			symbolOutput->put(this->symbolTable[i].other);		
		
			symbolOutput->put((char*)&(this->symbolTable[i].shndx), 2);	

			i++;
		
	}

	// Symbol Table Update
	sectionTable[sectionsNumber-2].size = symbolOutput->getBufferSize();

	// Symbol String Table Update
	sectionTable[sectionsNumber-1].size = symbolNameTable->getBufferSize();
	
}

void ELFWriter::createElfSectionTable(map<char*, Section*>* sections) {
	
	int numberOfNonDefaultSections = 0;

	for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++) {
		int addSection = (it->second->hasRelocation()) ? 2 : 1;		

		numberOfNonDefaultSections += addSection; 
	}

	sectionsNumber = numberOfNonDefaultSections + 4;

	// Update Shnum Attribute in ELF Header
	header.shnum = sectionsNumber;

	this->sectionTable = new SectionTableEntry[sectionsNumber];
	
	this->sectionTableOutput = new OutputBuffer*[sectionsNumber];

	this->sectionContentOutput = new OutputBuffer*[numberOfNonDefaultSections];

	sectionNameTable = new StringTableBuilder();

	int i = 0;

	// NULL Section Table Enrty
	this->sectionTableOutput[0] = new OutputBuffer(40);	
	
	for (int j = 0; j < 40; j++)
		this->sectionTableOutput[0]->put(0);

	this->sectionTable[i].name = 0;
	this->sectionTable[i].type = 0;
	this->sectionTable[i].flags = 0;
	this->sectionTable[i].addr = 0;
	this->sectionTable[i].offset = 0;
	this->sectionTable[i].size = 0;
	this->sectionTable[i].link = 0;
	this->sectionTable[i].info = 0;
	this->sectionTable[i].addralign = 0;
	this->sectionTable[i].entsize = 0;

	i++;

	for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++) {

		it->second->setSectionIndex(i);

		this->sectionTableOutput[i] = new OutputBuffer(40);	

		this->sectionTable[i].name = sectionNameTable->addString(it->second->getName());
		this->sectionTable[i].type = it->second->getSectionType();
		this->sectionTable[i].flags = it->second->getSectionFlags();
		this->sectionTable[i].addr = (it->second->getStartAddress() == -1) ? 0 : it->second->getStartAddress(); 
		this->sectionTable[i].offset = 0; // postvalja se nakon upisivanja sadrzaja sekcije
						// tada znamo velicinu offseta - reseno
		
		// align 2
		int alignedSize = ((it->second->size() % 2) == 0) ? it->second->size() : (it->second->size() + 1);

		this->sectionTable[i].size = alignedSize; // velicina content dela - reseno
		this->sectionTable[i].link = 0;
		this->sectionTable[i].info = 0;
		this->sectionTable[i].addralign = (it->second->getSectionType() == UND) ? 0 : 2;
		this->sectionTable[i].entsize = 0;

		i++;

		if (it->second->hasRelocation()) {

			relocationSectionsNumber++;

			it->second->setSectionIndex(i);

			this->sectionTableOutput[i] = new OutputBuffer(40);

			char* relString = new char[4 + strlen(it->second->getName())];

			relString[0] = '.';
			relString[1] = 'r';	
			relString[2] = 'e';
			relString[3] = 'l';

			for (int k = 0; k < strlen(it->second->getName()); k++)	
				relString[4 + k] = it->second->getName()[k];

			this->sectionTable[i].name = sectionNameTable->addString(relString); 
			this->sectionTable[i].type = 9; // relocation
			this->sectionTable[i].flags = 0; // relocation
			this->sectionTable[i].addr = (it->second->getStartAddress() == -1) ? 0 : it->second->getStartAddress();
			this->sectionTable[i].offset = 0; // postvalja se nakon upisivanja sadrzaja sekcije
							// tada znamo velicinu offseta - reseno
			this->sectionTable[i].size = it->second->getRelocationTable()->size() * 8;
			this->sectionTable[i].link = sectionsNumber - 2;
			this->sectionTable[i].info = i - 1;
			this->sectionTable[i].addralign = 2;
			this->sectionTable[i].entsize = 8;
		
			i++;

		}
	}

	// add Default Sections

	// Section Header String Table
	this->sectionTableOutput[i] = new OutputBuffer(40);

	// Update Shstrndx Attribute in ELF Header
	header.shstrndx = i;	

	this->sectionTable[i].name = sectionNameTable->addString(".shstrtab");
	this->sectionTable[i].type = 3;
	this->sectionTable[i].flags = 0;
	this->sectionTable[i].addr = 0; 
	this->sectionTable[i].offset = 52;
	this->sectionTable[i].size = 0; // update on end
	this->sectionTable[i].link = 0;
	this->sectionTable[i].info = 0;
	this->sectionTable[i].addralign = 1;
	this->sectionTable[i].entsize = 0; 

	i++;

	// Symbol Table
	this->sectionTableOutput[i] = new OutputBuffer(40);	

	this->sectionTable[i].name = sectionNameTable->addString(".symtab");
	this->sectionTable[i].type = 2;
	this->sectionTable[i].flags = 0; 
	this->sectionTable[i].addr = 0; 
	this->sectionTable[i].offset = 0; // naknadno postaviti - reseno
	this->sectionTable[i].size = 0; // naknadno postaviti - reseno
	this->sectionTable[i].link = 8;
	this->sectionTable[i].info = 7;
	this->sectionTable[i].addralign = 2;
	this->sectionTable[i].entsize = 16;

	i++;

	// Symbol String Table
	this->sectionTableOutput[i] = new OutputBuffer(40);	

	this->sectionTable[i].name = sectionNameTable->addString(".strtab");
	this->sectionTable[i].type = 3;
	this->sectionTable[i].flags = 0; 
	this->sectionTable[i].addr = 0; 
	this->sectionTable[i].offset = 52 + sectionNameTable->getBufferSize();
	this->sectionTable[i].size = 0; // naknadno postaviti - reseno
	this->sectionTable[i].link = 0;
	this->sectionTable[i].info = 0;
	this->sectionTable[i].addralign = 1;
	this->sectionTable[i].entsize = 0;

	// Update Section String Table Size
	this->sectionTable[header.shstrndx].size = sectionNameTable->getBufferSize();
}

void ELFWriter::bufferedSectionTable(map<char*, Section*>* sections) {

	for (int i = 1; i < sectionsNumber; i++) {
		
		if (sectionTableOutput[i] != NULL) {
		
			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].name), 4);		

			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].type), 4);		
		
			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].flags), 4);		
		
			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].addr), 4);		
			
			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].offset), 4);	

			// align 2
			int alignedSize = ((this->sectionTable[i].size % 2) == 0) ? this->sectionTable[i].size : (this->sectionTable[i].size + 1); 	
	
			sectionTableOutput[i]->put((char*)&(alignedSize), 4);
		
			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].link), 4);	

			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].info), 4);

			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].addralign), 4);

			sectionTableOutput[i]->put((char*)&(this->sectionTable[i].entsize), 4);	
	
		} else {
			
			sectionTableOutput[i] = new OutputBuffer(40);
			for (int j = 0; j < 40; j++) 
				sectionTableOutput[i]->put(0);
				
		}

	}
}

void ELFWriter::bufferedSectionContent(map<char*, Section*>* sections) {

	int r = 0, i = 0, offset = 52 + sectionNameTable->getBufferSize() + symbolNameTable->getBufferSize();

	for (map<char*, Section*>::iterator it = sections->begin(); it != sections->end(); it++) {
		
		// Section Offset Update
		sectionTable[i + 1].offset = offset; 

		// align 2
		int alignedSize = ((it->second->size() % 2) == 0) ? it->second->size() : (it->second->size() + 1);
		sectionContentOutput[i] = new OutputBuffer(alignedSize);

		if (alignedSize != it->second->size()) 
			sectionContentOutput[alignedSize - 1] = 0;

		for (int j = 0; j < it->second->size(); j++) {
			
			sectionContentOutput[i]->put(it->second->getSectionElement(j)->getContent());
		
		}

		offset += alignedSize;

		i++;

		if (it->second->hasRelocation()) {

			// Section Offset Update
			sectionTable[i + 1].offset = offset; 

			sectionContentOutput[i] = new OutputBuffer(it->second->getRelocationTable()->size() * 8);

			for (int j = 0; j < it->second->getRelocationTable()->size(); j++) {
			
				int contentOffset = it->second->getRelocationTable()->get(j)->getOffset();
				sectionContentOutput[i]->put((char*)(&contentOffset), 4);

				sectionContentOutput[i]->put((char)it->second->getRelocationTable()->get(j)->getRelocationType());

				int contentOrdinal = (it->second->getRelocationTable()->get(j)->getOrdinal()) << 8;
				char* writeableContentOrdinal = (char*)(&contentOrdinal);
				writeableContentOrdinal++; 
				sectionContentOutput[i]->put(writeableContentOrdinal, 3);		

			}						
			
			offset += it->second->getRelocationTable()->size() * 8;

			i++;
		}
	}

	// Update Shoff Attribute in ELF Header
	header.shoff = offset; 

	// Update Symbol Table Offset
	sectionTable[sectionsNumber-2].offset = offset + 40 * sectionsNumber;
}

void ELFWriter::write() {

	if (output.is_open()) {

		output.write(headerOutput->getBuffer(), headerOutput->getBufferSize());

		output.write(sectionNameTable->getBuffer(), sectionNameTable->getBufferSize());

		output.write(symbolNameTable->getBuffer(), symbolNameTable->getBufferSize());

		if (sectionContentOutput != NULL) 
			for (int i = 0; i < sectionsNumber - 4; i++)
				output.write(sectionContentOutput[i]->getBuffer(), sectionContentOutput[i]->getBufferSize());
		
		if (sectionTableOutput != NULL)		
			for (int i = 0; i < sectionsNumber; i++)
				output.write(sectionTableOutput[i]->getBuffer(), sectionTableOutput[i]->getBufferSize());

		if (symbolOutput != NULL)		
			output.write(symbolOutput->getBuffer(), symbolOutput->getBufferSize());
		
		output.close();	
	}

}
