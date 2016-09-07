#include <parser.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	Parser* parser = new Parser();

	parser->parse(argv[1], argv[2]);

	delete parser;
}
