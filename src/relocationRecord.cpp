#include "relocationRecord.h"
#include <section.h>

#include <cstring>

RelocationRecord::RelocationRecord(int offset, RelocationType type, int ordinal) {
	
	this->offset = offset;

	this->type = type;

	this->ordinal = ordinal;

}

int RelocationRecord::getOffset() {
	return offset;
}

RelocationType RelocationRecord::getRelocationType() {
	return type;
}

int RelocationRecord::getOrdinal() {
	return ordinal;
}

RelocationTable::RelocationTable(Section* section) {
	
	records = new vector<RelocationRecord*>();
	it = records->begin(); 

	ordinal = 0;

	this->section = section;
}

RelocationTable::~RelocationTable() {
	if (records->size() > 0)
		for (vector<RelocationRecord*>::iterator it = records->begin(); it != records->end(); it++)
			records->erase(it);

	delete records;
}

void RelocationTable::insert(int offset, RelocationType type, int ordinal) {
	it = records->insert(it, new RelocationRecord(offset, type, ordinal));
}


RelocationRecord* RelocationTable::get(int position) {
	return (*records)[records->size() - 1 - position];
}

int RelocationTable::size() {
	return records->size();
}
