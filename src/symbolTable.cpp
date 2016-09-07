#include <symbolTable.h>

#include <cstring>
#include <iostream>

using namespace std;

Symbol::Symbol(char* label, char* section, int offset, bool local, int ordinal, int size, SymbolType type) {
	this->label = new char[strlen(label) + 1];
	strcpy(this->label, label);

	if (section != NULL) {
		this->section = new char[strlen(section) + 1];
		strcpy(this->section, section);
	}
	
	this->offset = offset;
	this->local = local;
	this->ordinal = ordinal;
	this->size = size;

	this->type = type;
}

Symbol::~Symbol() {
	delete [] label;
	
	if (section != NULL)	
		delete [] section; 
}

char* Symbol::getLabel() {
	return label;		
}
		
char* Symbol::getSection() {
	return section;		
}

int Symbol::getOffset() {
	return offset;		
}

bool Symbol::getLocal() {
	return local;		
}

int Symbol::getOrdinal() {
	return ordinal;		
}

void Symbol::setLocal(bool local) {
	this->local = local;		
}

int Symbol::getSize() {
	return size;
}

SymbolType Symbol::getType() {
	return type;
}

SymbolTable::SymbolTable() {
	table = new map<char*, Symbol*>();

	ordinal = 0;

}

SymbolTable::~SymbolTable() {
	if (table->size() > 0) 
		for (map<char*, Symbol*>::iterator it = table->begin(); it != table->end(); it++)
			table->erase(it);

	delete table;	
}

bool SymbolTable::insert(char* symbolLabel, char* section, int offset, bool local, int size, SymbolType type) {
	if (doesExsists(symbolLabel)) {
		return false;	
	}

	table->insert(std::pair<char*, Symbol*>(symbolLabel, new Symbol(symbolLabel, section, offset, local, ordinal, size, type)));

	ordinal++;

	return true;
}

Symbol* SymbolTable::get(char* symbolLabel) {
	for (map<char*, Symbol*>::iterator it = table->begin(); it != table->end(); it++) {
		if (!strcmp(it->second->getLabel(), symbolLabel))
			return it->second;	
	}
	
	return NULL;
}

bool SymbolTable::doesExsists(char* symbolLabel) {
	if (get(symbolLabel) != NULL) {
		return true;	
	}

	return false;
}

map<char*, Symbol*>* SymbolTable::getTable() {
	return table;				
}

