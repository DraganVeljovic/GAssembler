#include <stringTableBuilder.h>

#include <outputBuffer.h>

#include <cstring>

StringTableBuilder::StringTableBuilder() {
	bufferTable = new vector<char*>();
	
	charSize = 1;
}

StringTableBuilder::~StringTableBuilder() {
	delete bufferTable;
}

int StringTableBuilder::addString(char* string) {
	
	bufferTable->push_back(string);
	
	int oldCharSize = charSize;

	charSize += (strlen(string) + 1);

	return oldCharSize; 
}

char* StringTableBuilder::getBuffer() {

	OutputBuffer* buffer = new OutputBuffer(charSize);

	buffer->put(0);

	for (it = bufferTable->begin(); it != bufferTable->end(); it++) {
		buffer->put((*it), strlen(*it));
		buffer->put(0);
	}

	return buffer->getBuffer();
}

int StringTableBuilder::getBufferSize() {

	charSize += (charSize % 2 == 0) ? 0 : 1;

	return charSize;
}
