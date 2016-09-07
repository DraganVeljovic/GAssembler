#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>

using namespace std;

enum SymbolType { NO_TYPE = 0, SECTION = 3 };

class Symbol {
	private:

		char* label;
		char* section;
		int offset;
		bool local;
		int ordinal;
		int size;

		SymbolType type;
	
	public:

		Symbol(char* label, char* section, int offset, bool local, int ordinal, int size, SymbolType type);

		~Symbol();

		char* getLabel();
		
		char* getSection();

		int getOffset();

		bool getLocal();

		int getOrdinal();

		void setLocal(bool local);
		
		int getSize();

		SymbolType getType();
};

class SymbolTable {
	private:

		map<char*, Symbol*> *table;

		int ordinal;

	
	public:

		SymbolTable();

		~SymbolTable();
		
		bool insert(char* symbolLabel, char* section, int offset, bool local, int size, SymbolType type);

		Symbol* get(char* symbolLabel);

		bool doesExsists(char* symbolLabel);

		map<char*, Symbol*>* getTable();
};

#endif
