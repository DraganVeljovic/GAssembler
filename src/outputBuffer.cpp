#include <outputBuffer.h>

#include <cstdio>
#include <iostream>

using namespace std;

OutputBuffer::OutputBuffer() {
}

OutputBuffer::OutputBuffer(int bufferSize) {
	buffer = new char[bufferSize];

	this->bufferSize = bufferSize;

	cursor = 0;
} 

OutputBuffer::~OutputBuffer() {
	if (buffer != NULL) delete [] buffer;
}

void OutputBuffer::put(char* data, int amount) {
	if (cursor + amount > bufferSize) {
		// greska
		
	} else {
		for (int i = 0; i < amount; i++)	
			buffer[cursor + i] = data[i];
		
		cursor += amount;

	}
}

void OutputBuffer::put(char data) {
	put(&data, 1);
} 

char* OutputBuffer::getBuffer() {
	return buffer;
}

int OutputBuffer::getBufferSize() {
	return bufferSize;
}

int OutputBuffer::getCursor() {
	return cursor;
}
