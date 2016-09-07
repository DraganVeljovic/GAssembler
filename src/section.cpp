#include <section.h>
#include <relocationRecord.h>

#include <cstring>

SectionElement::SectionElement(int offset, char content) {
	
	this->offset = offset;

	this->content = content;

}


int SectionElement::getOffset() {
	return offset;
}

char SectionElement::getContent() {
	return content;
}

Section::Section(char* name) {

	elements = new vector<SectionElement*>();
	it = elements->begin();

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);

	if (strstr(name, "text")) {
		type = TEXT;
	} else if (strstr(name, "data")) {
			type = DATA;		
		} else if (strstr(name, "bss")) {
				type = BSS;
			} else {
				type = UND;			
			}

	localCounter = 0;

	startAddress = -1;

	relocationTable = new RelocationTable(this);

	sectionIndex = -1;
}

Section::~Section() {
	delete [] name;

	if (elements->size() > 0)
		for (vector<SectionElement*>::iterator it = elements->begin(); it != elements->end(); it++)
			elements->erase(it);

	delete elements; 

	delete relocationTable;

}

char* Section::getName() {
	return name;
}

int Section::getLocalCounter() {
	return localCounter;
}

void Section::setLocalCounter(int localCounter) {
	this->localCounter = localCounter;
}

void Section::incrementLocalCounter(int increment) {
	this->localCounter += increment;
}

void Section::insert(char content) {
	
	it = elements->insert(it, new SectionElement(localCounter, content));

	localCounter++;

}

SectionElement* Section::getSectionElement(int offset) {
	return (*elements)[elements->size() - 1 - offset];
}

int Section::size() {
	return elements->size();
}

int Section::getStartAddress() {
	return startAddress;
}

void Section::setStartAddress(int startAddress) {
	this->startAddress = startAddress;
}

RelocationTable* Section::getRelocationTable() {
	return relocationTable;
}

bool Section::hasRelocation() {
	return ((relocationTable->size() == 0) ? false : true); 
}

SectionType Section::getSectionType() {
	return type;
}

int Section::getSectionIndex() {
	return sectionIndex;
}

void Section::setSectionIndex(int index) {
	sectionIndex = index;
}

int Section::getSectionFlags() {
	if (strstr(name, "text"))
		return 0x06;

	if (strstr(name, "data") || strstr(name, "bss"))
		return 0x03;

	return 0x00;
}
