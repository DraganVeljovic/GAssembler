#include <register.h>

#include <cstring>

Register::Register() {
	name = NULL;
	nameUC = NULL;
}

Register::Register(char* name, char* nameUC) {
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);

	this->nameUC = new char[strlen(nameUC) + 1];
	strcpy(this->nameUC, nameUC);
}

Register::~Register() {
	if (name != NULL) delete [] name;
	if (nameUC != NULL) delete [] nameUC;
}

char* Register::getName() {
	return name;
}

char* Register::getNameUC() {
	return nameUC;
}

void Register::setName(char* name) {
	if (name != NULL) delete [] name;

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

void Register::setNameUC(char* nameUC) {
	if (nameUC != NULL) delete [] nameUC;

	this->nameUC = new char[strlen(nameUC) + 1];
	strcpy(this->nameUC, nameUC);
}

