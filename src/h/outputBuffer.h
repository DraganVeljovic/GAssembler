#ifndef OUTPUT_BUFFER_H
#define OUTPUT_BUFFER_H

class OutputBuffer {
private:
	char* buffer;
	int bufferSize;
	
	int cursor;
public:
	OutputBuffer();
	
	OutputBuffer(int bufferSize);

	~OutputBuffer();

	void put(char*, int);

	void put(char);

	char* getBuffer();
	
	int getBufferSize();

	int getCursor();
	
};

#endif
