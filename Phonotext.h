#pragma once

#include <forward_list>
#include <string>
#include <vector>

#include <iostream>

#include "Letter.h"

struct Repeat
{
	int count;
	int power;
	std::vector<Letter> letters;
	std::vector<std::forward_list<Letter>::iterator> combs;
	bool _words;
};

class Phonotext
{
public:
	Phonotext();
	Phonotext(std::string text);
	~Phonotext();

	std::forward_list<Letter> basetext;
	std::vector<std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator>> SP;
	std::vector<std::vector<std::vector<std::forward_list<Letter>::iterator>>> syllableCombinations;
	std::vector<Repeat> repeat;


	std::pair<int, int> countLetters();
	std::string getOrigin();
	std::string getTechnic();
	std::string getPrintable();
	std::string getPhonotextRepr();
	int length();
};

