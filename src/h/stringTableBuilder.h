#ifndef STRING_TABLE_BUILDER_H
#define STRING_TABLE_BUILDER_H

#include <vector>

using namespace std;

class StringTableBuilder {
private:
	
	vector<char*>* bufferTable;
	vector<char*>::iterator it;

	int charSize;
	
public:
	StringTableBuilder();

	~StringTableBuilder();
	
	int addString(char*); 

	char* getBuffer();

	int getBufferSize();
};

#endif
