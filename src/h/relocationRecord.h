#ifndef RELOCATION_RECORD_H
#define RELOCATION_RECORD_H

#include <vector>

using namespace std;

enum RelocationType { NONE, R_16, R_PC16 };

class Section;

class RelocationRecord {

private:

	int offset;
	RelocationType type;
	int ordinal;

public:

	RelocationRecord(int offset, RelocationType type, int ordinal);

	int getOffset();

	RelocationType getRelocationType();

	int getOrdinal();
};

class RelocationTable {

private:

	vector<RelocationRecord*>* records;
	vector<RelocationRecord*>::iterator it;

	int ordinal;

	Section* section;	

public:

	RelocationTable(Section* section);

	~RelocationTable();

	void insert(int offset, RelocationType type, int ordinal);

	RelocationRecord* get(int position);

	int size();
};

#endif
