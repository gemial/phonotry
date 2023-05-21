#pragma once

#include <iostream>
#include <vector>
#include <forward_list>
#include <set>

#include "Conf.h"
#include "Letter.h"
#include "Phonotext.h"

class Proccessing
{
public:
	Proccessing(Phonotext pt, std::string lng, double min_pwr, double max_pwr);

	Phonotext getResult() { return this->pt; }
	void print();

private:
	Phonotext pt;
	Conf CONFIG;
	double min_pwr = 0;
	double max_pwr = 0;
	
	std::vector<std::forward_list<Letter>::iterator> volveIterators;

	void proccess();

	void modifyProccessor();
	void sameProccessor();
	void joinProccessor();
	void numberProccessor();
	void finderVolve();
	void SPmaxProccessor();
	std::pair<int, std::vector<int>> findLocalWordsInds(std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator> localSP);
	void combinationsProccessor(int N = 2);
	void repeatProccessor();
	std::pair<bool, double> rusFilterComb(std::vector<std::forward_list<Letter>::iterator> comb, std::vector<std::string> words);
};

