#ifndef REGISTER_H
#define REGISTER_H

class Register {
private: 
	char* name;
	char* nameUC;
	int index;
public:
	Register();
	
	Register(char* name, char* nameUC);

	~Register();
	
	char* getName();

	char* getNameUC();

	void setName(char*);

	void setNameUC(char*);
};

#endif
