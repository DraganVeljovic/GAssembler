#ifndef SECTION_H
#define SECTION_H

#include <vector>

using namespace std;

class RelocationTable;

enum SectionType { UND = 0, TEXT = 1, DATA = 1, BSS = 8};

class SectionElement {
private:
	
	int offset;
	char content;

public:

	SectionElement(int offset, char content);

	int getOffset();

	char getContent();

};

class Section {
private:

	vector<SectionElement*>* elements;
	vector<SectionElement*>::iterator it;

	char* name;

	int localCounter;

	int startAddress;

	RelocationTable* relocationTable;

	SectionType type;

	int sectionIndex;

public:

	Section(char* name);

	~Section();

	char* getName();

	int getLocalCounter();

	void setLocalCounter(int localCounter);

	int getStartAddress();

	void setStartAddress(int startAddress);

	void incrementLocalCounter(int increment);

	void insert(char content);

	SectionElement* getSectionElement(int offset);

	int size();

	RelocationTable* getRelocationTable();

	bool hasRelocation();

	SectionType getSectionType();

	int getSectionIndex();

	void setSectionIndex(int);

	int getSectionFlags();
};
	
#endif
