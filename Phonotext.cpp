#include "Phonotext.h"

Phonotext::Phonotext()
{
}

Phonotext::Phonotext(std::string text)
{
	if (text == "")
		return;

	// Генерация односвязного списка букв
	basetext.emplace_front(Letter(tolower(text[0])));
	std::forward_list<Letter>::iterator iter = basetext.begin();
	for (int i = 1; i < text.size(); i++)
	{
		basetext.emplace_after(iter, Letter(tolower(text[i])));
		++iter;
	}
}

Phonotext::~Phonotext()
{
}

std::pair<int, int> Phonotext::countLetters()
{
	std::pair<int, int> count(0, 0); // <countSymbols, countVolves>

	for (auto &symb : basetext)
	{
		if (symb.isConsonant)
			count.first++;
		if (symb.isVolve && symb.printable != '\n') // Вернуться к вопросу хранения данных ("--endl")
		{
			count.second++;
			count.first++;
		}
	}

	return count;
}

std::string Phonotext::getOrigin()
{
	std::string originText = "";

	for (auto& symb : basetext)
		originText += symb.origin;

	return originText;
}

std::string Phonotext::getTechnic()
{
	std::string technicText = "";

	for (auto& symb : basetext)
		technicText += symb.technic;

	return technicText;
}

std::string Phonotext::getPrintable()
{
	std::string printableText = "";

	for (auto& symb : basetext)
		printableText += symb.printable;

	return printableText;
}

std::string Phonotext::getPhonotextRepr()
{
	std::string reprText = "";

	for (auto& symb : basetext)
		reprText += symb.getLetterRepr();

	return reprText;
}

int Phonotext::length()
{
	int len = 0;

	for (auto& symb : basetext)
		len++;

	return len;
}