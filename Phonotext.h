#pragma once

#include <forward_list>
#include <string>
#include <vector>

#include <iostream>

#include "Letter.h"

class Phonotext
{
public:
	Phonotext();
	Phonotext(std::string text);
	~Phonotext();

	std::forward_list<Letter> basetext;
	std::vector<std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator>> SP;

	std::pair<int, int> countLetters();
	std::string getOrigin();
	std::string getTechnic();
	std::string getPrintable();
	std::string getPhonotextRepr();
	int length();
};

