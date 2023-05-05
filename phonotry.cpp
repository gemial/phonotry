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

double get_pwr(Letter a, Letter b)
{
    if (a.technic != b.technic)
        return 0;

    int dist = b.syll - a.syll;
    if (dist < 1)
        return 0;

    int pwr = 0;
    int mul = 0;
    int dist_w = b.word - a.word;

    pwr = 1 / dist + 1 / (dist_w + 2);
    if (a.origin == b.origin and a.isConsonant)
        mul += 1;
    mul *= 1 / (1 /* + a.w_pos[0] + b.w_pos[0] */ );

    return pwr * mul;
}

void numberProccessor(Phonotext* pt, vector<string> words, vector<string> volves, vector<string> consonants)
{
    int i = 0;
    int j = 1;
    int k = 1;
    int num = 1;
    bool space = false;

    int l_number = 0;
    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++, l_number++)
    {
        it->number = i;
        it->word = k;
        //w_pos

        if (it->technic == " " || it->technic == "|" || it->technic == "\n")
        {
            if (!space)
            {
                k++;
                if (num < 3)
                {
                    while (num > 1)
                    {
                        num--;
                        // w_pos skip
                        num--;
                    }
                }
                while (num > 1)
                {
                    num--;
                    // w_pos skip
                    num--;
                }
            }
            space = true;
        }
        else
        {
            bool flag = false;
            for (int t = 0; t < words.size(); t++)
                if (it->technic == words[t])
                    flag = true;
            space = (space && flag);
        }
        if (it->technic == "\n")
        {
            // обработка
        }
        for (int t = 0; t < volves.size(); t++)
            if (it->printable == volves[t])
            {
                it->isVolve = true;
                j++;
                break;
            }
        for (int t = 0; t < consonants.size(); t++)
            if (it->printable == consonants[t])
            {
                it->isConsonant = true;
                break;
            }
        it->syll = j;
        i++;
        num++;
    }
}

pair<int, vector<int>> findLocalWordInds(pair<forward_list<Letter>::iterator, forward_list<Letter>::iterator> localSP)
{
    int indVolve;
    vector<int> indCons;

    int i = 0;
    for (auto it = localSP.first; it != localSP.second; it++, i++)
        if (it->isVolve)
            indVolve = i;
        else
            indCons.push_back(i);

    return make_pair(indVolve, indCons);
}

void combinationsProccessor(Phonotext* pt, int N = 2)
{
    N++;
    for (int i = 0; i < pt->SP.size(); i++)
    {
        // Из каждого участка выбирается гласная и все согласные в отдельный массив
        pair<int, vector<int>> localInds = findLocalWordInds(pt->SP[i]);
        int posVolve = localInds.first;
        vector<int> posCons = localInds.second;

        // Всевозможные пары комбинаций из индексов массива согласных
        vector<pair<int, int>> consCombs;
        for (int j = 0; j < 5; j++)
            for (int k = j + 1; k < 5; k++)
                consCombs.push_back(make_pair(j, k));

        // Сборка комбинаций из СГС в разных вариантах (в итераторах)
        vector<forward_list<Letter>::iterator> tmpItCombs;
        for (int j = 0; j < consCombs.size(); j++)
        {
            // Создание сортированных комбинаций из пар индексов соглассных + индекс гласной
            vector<int> tmpCombs;
            tmpCombs.push_back(posCons[consCombs[j].first]);
            tmpCombs.push_back(posCons[consCombs[j].second]);
            tmpCombs.push_back(posVolve);
            sort(tmpCombs.begin(), tmpCombs.end());

            // Поиск итераторов элементов выбранных в пары
            forward_list<Letter>::iterator it1, it2, it3;
            it1 = it2 = it3 = pt->SP[i].first;

            for (int k = 0; k < posCons[2]; k++)
            {
                if (k <= tmpCombs[0])
                    it1++;
                if (k <= tmpCombs[1])
                    it2++;
                if (k <= tmpCombs[2])
                    it3++;
            }
            tmpItCombs.push_back(it1);
            tmpItCombs.push_back(it2);
            tmpItCombs.push_back(it3);
        }
        pt->syllableCombinations.push_back(tmpItCombs);

        // Добавить то, что вытаскивается из итераторов и обработку из filter_combination
    }
}

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
        // возможно не берётся последний слог
    }
    pt->SP = dividedVolveIterators;
}

void joinProccessor(Phonotext* pt, std::map<char, std::string> asOne)
{
    std::string tmp_a;
    std::string tmp_b;

    auto itLast = pt->basetext.begin();
    for (auto it = pt->basetext.begin(); it != pt->basetext.end(); it++)
        itLast = it;

    auto it = pt->basetext.begin();
    auto itPreviosLetter = pt->basetext.begin();

    while (it != pt->basetext.end())
    {
        if (it == pt->basetext.begin())
            tmp_a = it->origin;
        else
        {
            tmp_a = itPreviosLetter->origin;
            tmp_b = it->origin;

            auto sameKey = asOne.find(tmp_a[0]);
            if (sameKey != asOne.end())
            {
                if ((tmp_a + tmp_b) == sameKey->second)
                {
                    itPreviosLetter->origin = sameKey->second;
                    itPreviosLetter->printable = sameKey->second;
                    itPreviosLetter->technic = sameKey->second;
                    if (it == itLast)
                    {
                        pt->basetext.erase_after(itPreviosLetter);
                        break;
                    }
                    it++;
                    pt->basetext.erase_after(itPreviosLetter);
                }
            }
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
    std::cout << "origin      : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.origin;
    }
    std::cout << std::endl;
    std::cout << "technic     : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.technic;
    }
    std::cout << std::endl;
    std::cout << "printable   : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.printable;
    }
    std::cout << std::endl;
    std::cout << "isWord      : ";
    for (auto& i : pt.basetext)
    {
        if (i.isVolve)
            cout << "v";
        else if (i.isConsonant)
            cout << "c";
        else
            cout << " ";
    }
    std::cout << std::endl;
}

void proccessor(Phonotext* pt, Conf CONFIG)
{
    std::cout << "\nnumber\n";
    numberProccessor(pt, CONFIG.getWords(), CONFIG.getVolves(), CONFIG.getConsonants());
    print(*pt);

    std::cout << "\nmodify\n";
    modifyProccessor(pt, CONFIG.getModifications());
    print(*pt);

    std::cout << "\nsame\n";
    sameProcessor(pt, CONFIG.getAsSame());
    print(*pt);

    std::cout << "\njoin\n";
    joinProccessor(pt, CONFIG.getAsOne());
    print(*pt);

    std::cout << "\nSP\n";
    SPmaxProccessor(pt);
    print(*pt);
}

/*string toLower(string str)
{
    for (int i = 0; i < str.size(); i++)
    switch (str[i])
    {
    case 'А':
        str[i] = 'a';
        break;
    case 'Б':
        str[i] = 'б';
        break;
    case 'В':
        str[i] = 'в';
        break;
    case 'Г':
        str[i] = 'г';
        break;
    case 'Д':
        str[i] = 'д';
        break;
    case 'Е':
        str[i] = 'е';
        break;
    case 'Ё':
        str[i] = 'ё';
        break;
    case 'Ж':
        str[i] = 'ж';
        break;
    case 'З':
        str[i] = 'з';
        break;
    case 'И':
        str[i] = 'и';
        break;
    case 'Й':
        str[i] = 'й';
        break;
    case 'К':
        str[i] = 'к';
        break;
    case 'Л':
        str[i] = 'л';
        break;
    case 'М':
        str[i] = 'м';
        break;
    case 'Н':
        str[i] = 'н';
        break;
    case 'О':
        str[i] = 'о';
        break;
    case 'П':
        str[i] = 'п';
        break;
    case 'Р':
        str[i] = 'р';
        break;
    case 'С':
        str[i] = 'с';
        break;
    case 'Т':
        str[i] = 'т';
        break;
    case 'У':
        str[i] = 'у';
        break;
    case 'Ф':
        str[i] = 'ф';
        break;
    case 'Х':
        str[i] = 'х';
        break;
    case 'Ц':
        str[i] = 'ц';
        break;
    case 'Ч':
        str[i] = 'ч';
        break;
    case 'Ш':
        str[i] = 'ш';
        break;
    case 'Щ':
        str[i] = 'щ';
        break;
    case 'Ъ':
        str[i] = 'ъ';
        break;
    case 'Ы':
        str[i] = 'ы';
        break;
    case 'Ь':
        str[i] = 'Ь';
        break;
    case 'Э':
        str[i] = 'э';
        break;
    case 'Ю':
        str[i] = 'ю';
        break;
    case 'Я':
        str[i] = 'я';
        break;
    }
    return str;
}*/

int main()
{
    system("chcp 65001");

    Conf CONFIG("rus"); // Выбор языка

    std::cout << "start\n";
    Phonotext pt("҄сегодня пришёл юнгатслтс");
    print(pt);

    proccessor(&pt, CONFIG);

    return 0;
}

// первая буква скипается