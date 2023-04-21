//#define UNICODE

//#include <windows.h>

#include <iostream>
#include <vector>
#include <forward_list>

#include "Conf.h"
#include "Letter.h"
#include "Phonotext.h"

#include <fstream>

//char ZERO_SPACE = '\u200B';


using namespace std;



vector<forward_list<Letter>::iterator> finderVolve(Phonotext* pt)
{
    vector<forward_list<Letter>::iterator> volveIterators;
    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++)
        if (it->isVolve)
            volveIterators.push_back(it);

    return volveIterators;
}


void SPmaxProccessor(Phonotext* pt)
{
    vector<pair<forward_list<Letter>::iterator, forward_list<Letter>::iterator>> dividedVolveIterators;

    forward_list<Letter>::iterator startVolveIt, middleVolveIt, endVolveIt;
    bool firstVolve = false;

    vector<forward_list<Letter>::iterator> volveIterators = finderVolve(pt);

    if (volveIterators.size() > 1)
    {
        startVolveIt = pt->basetext.begin();
        middleVolveIt = volveIterators[0];

        for (int i = 1; i < volveIterators.size(); i++)
        {
            endVolveIt = volveIterators[i];

            dividedVolveIterators.push_back(make_pair(startVolveIt, endVolveIt));

            middleVolveIt++;
            startVolveIt = middleVolveIt;
            middleVolveIt = endVolveIt;
            // дальше идёт ваше условие
        }
    }
}

void joinProccessor(Phonotext* pt, std::map<char, std::string> asSame)
{
    std::string tmp_a;
    std::string tmp_b;
    std::string tmp_c;

    auto itPreviosLetter = pt->basetext.begin();
    bool deleteLast = false;
    bool flag = true;

    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++);
    auto it = pt->basetext.begin();
    while (it != pt->basetext.end())
    {
        deleteLast = false;
        if (it == pt->basetext.begin())
            tmp_a = it->origin;
        else
        {
            tmp_a = itPreviosLetter->origin;
            tmp_b = it->origin;
            flag = true;

            auto sameKey = asSame.find(tmp_a[0]);
            if (sameKey != asSame.end())
            {
                if ((tmp_a + tmp_b) == sameKey->second)
                {
                    itPreviosLetter->origin = sameKey->second;
                    itPreviosLetter->printable = sameKey->second;
                    itPreviosLetter->technic = sameKey->second;
                    it++;
                    pt->basetext.erase_after(itPreviosLetter);
                    flag = false;
                    deleteLast = true;
                }
            }
            if (flag)
                itPreviosLetter = it;
        }
        it++;
    }

}

void sameProcessor(Phonotext *pt, std::map<std::string, std::string> asSame)
{
    for (auto& symb : pt->basetext)
    {
        auto it = asSame.find(symb.printable);
        if (it != asSame.end())
            symb.technic = it->second;
        else
            symb.technic = "&";

        if (symb.technic == "&" || symb.technic == "|")
            symb.printable = symb.technic;
    }
}

void modifyProccessor(Phonotext *pt, std::map<std::string, std::map<std::string, std::string>> modifications)
{
    std::string tmp_a;
    std::string tmp_b;
    std::string tmp_c;

    auto itPreviosLetter = pt->basetext.begin();

    bool needChange = false;
    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++)
    {
        if (needChange)
        {
            it->printable = "";
            needChange = false;
        }
        if (it == pt->basetext.begin())
        {
            tmp_b = it->origin;
        }
        else
        {
            tmp_a = tmp_b;
            tmp_b = it->origin;

            auto modFirstKey = modifications.find(tmp_a);
            if (modFirstKey != modifications.end())
            {
                int i, l;
                auto modSecondKey = modFirstKey->second.find(tmp_b);
                if (modSecondKey != modFirstKey->second.end())
                {
                    tmp_c = modSecondKey->second;
                    for(i = 0; tmp_c[0] & (0x80 >> i); ++i); i = (i)?i:1; // find first letter
                    for(l = 0; tmp_c[i] & (0x80 >> l); ++l); l = (l)?l:1; // find second letter

                    itPreviosLetter->origin = tmp_c.substr(0, i);
                    it->origin = tmp_c.substr(i, l);
                    pt->basetext.emplace_after(it, Letter(tmp_c.substr(i + l)));
                    needChange = true;
                }
            }
        }
        itPreviosLetter = it;
    }
}


void print(Phonotext pt)
{
    std::cout << "-----------\n";
    std::cout << "origin   : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.origin << " ";
    }
    std::cout << std::endl;
    std::cout << "technic  : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.technic;
    }
    std::cout << std::endl;
    std::cout << "printable: ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.printable;
    }
    std::cout << std::endl;
}




int main()
{
    system("chcp 65001");
    //SetConsoleOutputCP(65001);

    Conf CONFIG("rus"); // Выбор языка

    std::cout << "start\n";
    Phonotext pt("сегодня пришёл юнгатсл");
    print(pt);

    std::cout << "modify:\n";
    modifyProccessor(&pt, CONFIG.getModifications());
    print(pt);
    std::cout << "same:\n";
    sameProcessor(&pt, CONFIG.getAsSame());
    print(pt);
    std::cout << "join:\n";
    joinProccessor(&pt, CONFIG.getAsOne()); //???
    print(pt);
    std::cout << "sp:\n";
    SPmaxProccessor(&pt);
    print(pt);

    return 0;
}