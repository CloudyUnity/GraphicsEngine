#ifndef _MODELPARSER_H_
#define _MODELPARSER_H_

#include <fstream>
#include <ostream>
#include <iostream>
#include <vector>
using std::ifstream;
using std::ostream;

class ModelParser {
public:
	static bool ParseFile(const char*);
};

#endif