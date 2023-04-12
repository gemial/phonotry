//#define UNICODE

// #include <windows.h>

#include <iostream>

#include "Conf.h"
#include "Letter.h"
#include "Phonotext.h"

#include <fstream>

//char ZERO_SPACE = '\u200B';





void SPmaxProccessor(Phonotext* pt)
{
    pt->SP.emplace_front(Letter(" "));
    std::forward_list<Letter>::iterator SPlast = pt->SP.begin();

    for (auto& it : pt->basetext)
        if (!(it.isVolve) && it.origin != "\n")
            SPlast = pt->SP.emplace_after(SPlast, it);

    pt->SP.pop_front();
}

void joinProccessor(Phonotext* pt, std::map<char, std::string> asSame)
{
    std::string tmp_a;
    std::string tmp_b;
    std::string tmp_c;

    auto itPreviosLetter = pt->basetext.begin();

    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++)
    {
        if (it == pt->basetext.begin())
            tmp_b = it->origin;
        else
        {
            tmp_a = (tmp_b == "&" ? "" : tmp_b);
            tmp_b = (it->origin == "&" ? "" : it->origin);

            auto sameKey = asSame.find(tmp_a[0]);
            if (sameKey != asSame.end())
            {
                if ((tmp_a + tmp_b).size() == 1 && (tmp_a + tmp_b)[0] == sameKey->first) // rewrite
                {
                    itPreviosLetter->origin = sameKey->second;
                    itPreviosLetter->technic = sameKey->second;
                    itPreviosLetter->printable = sameKey->second;

                    it = itPreviosLetter;
                    pt->basetext.erase_after(itPreviosLetter);
                }
            }
        }
        itPreviosLetter = it;
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
        std::cout << i.origin;
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
    std::cout << "SP       : ";
    for (auto& i : pt.SP)
    {
        std::cout << i.origin;
    }
    std::cout << "\n-----------\n";
}




int main()
{
    //system("chcp 65001");
    // SetConsoleOutputCP(65001);

    Conf CONFIG("rus"); // Выбор языка

    Phonotext pt("сегодня пришёл юнга");
    print(pt);

    modifyProccessor(&pt, CONFIG.getModifications());
    print(pt);
    sameProcessor(&pt, CONFIG.getAsSame());
    print(pt);
    joinProccessor(&pt, CONFIG.getAsOne());
    print(pt);
    SPmaxProccessor(&pt);
    print(pt);

    return 0;
}