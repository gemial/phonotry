//#define UNICODE

//#include <windows.h>

#include <iostream>
#include <vector>
#include <forward_list>
#include <set>

#include "Conf.h"
#include "Letter.h"
#include "Phonotext.h"

#include <fstream>

//char ZERO_SPACE = '\u200B';


using namespace std;

double min_pwr, max_pwr;

pair<bool, double> rusFilterComb(vector<int> indexes, vector<int> vol_pos, string txt /*positions*/)
{
    double pwr;
    set<string> tmptxt(txt.begin(), txt.end());
    if (tmptxt.size() < 3)
        return make_pair(false, 0);
    if (indexes[0] == vol_pos[0])
        pwr = 2;
    else if (indexes[2] == vol_pos[0])
        pwr = 1;
    else
        pwr = 3;
    int count = 0;
    for (auto& i : txt)
        if (i == ' ')
            count++;
    pwr += (indexes[2] - indexes[0] - count == 2 ? 5 : 0);
    pwr += (count == 0 ? 2 : 0);
    count = 0;
    for (int i = txt.size() - 3; i < txt.size(); i++)
        if (txt[i] == 'й') // ????????
            count++;
    pwr += (count == 0 ? 4 : 0);
    // pwr += 1 if positions.find(b'\x01', -3) != -1 else 0
    pwr /= 15;
    return make_pair(min_pwr <= pwr <= max_pwr, pwr);
}

double get_pwr(forward_list<Letter>::iterator a, forward_list<Letter>::iterator b)
{
    if (a->technic != b->technic)
        return 0;

    int dist = b->syll - a->syll;
    if (dist < 1)
        return 0;

    int pwr = 0;
    int mul = 0;
    int dist_w = b->word - a->word;

    pwr = 1 / dist + 1 / (dist_w + 2);
    if (a->origin == b->origin and a->isConsonant)
        mul += 1;
    mul *= 1 / (1 /* + a->w_pos[0] + b->w_pos[0] */ );

    return pwr * mul;
}

double get_pwr_combs(vector<vector<forward_list<Letter>::iterator>> a, vector<vector<forward_list<Letter>::iterator>> b)
{
    double pwr = 0;
    for (int i = 0; i < a[0].size(); i++)
        for (int j = 0; j < b[0].size(); j++)
            pwr += get_pwr(a[0][i], b[0][j]);

    int mul_1 = 1;
    int mul_2 = 1;
    for (int i = 0; i < a.size() - 1; i++)
    {
        mul_1 *= a[0][i + 1]->number - a[0][i]->number;
        mul_2 *= b[0][i + 1]->number - b[0][i]->number;
    }

    int mul = 1;
    // mul = 10 * a[1] * b[1] * (1 + a[0][-1].p_end + b[0][-1].p_end)

    pwr *= 1 / (mul_1 + 1) + 1 / (mul_2 + 1);

    return mul * pwr;
}


void repeatRecountProccessor(Phonotext* pt)
{
    for (auto& x : pt->repeats)
    {
        //x.second.repeatPower = make_pair(x.second.power, x.second.power / x.second.count);
        x.second.count = 1;
        vector<vector<forward_list<Letter>::iterator>> last;
        for (auto& y : x.second.combs)
        {
            if (last.size() == 0)
            {
                last = y;
                continue;
            }
            if (y[0][0]->number - last[0][last.size() - 1]->number > 0)
                x.second.count += 1;
        }
    }
    for (auto& rep : pt->repeats)
    {
        double pwr = 0;
        for (int i = 0; i < rep.second.combs.size()-1;i++)
            for (int j = i; j < rep.second.combs.size(); j++)
                pwr += get_pwr_combs(rep.second.combs[i], rep.second.combs[j]);
        //rep.second.repeatPower = pwr, rep.second.repeatPower[1];
    }
}


void repeatProccessor(Phonotext* pt)
{
    for (int n_syll = 0; n_syll < pt->syllableCombinations.size(); n_syll++)
    {
        for (auto& comb : pt->syllableCombinations[n_syll])
        {
            pair<vector<string>, map<int, int>> a;

            for (auto& i : comb)
            {
                if (i->isConsonant)
                    a.first.push_back(i->technic);
                a.second.insert(make_pair(i->number, n_syll));
            }

            set<string> tmpWords(a.first.begin(), a.first.end());
            string setToStr = "";
            for (auto& i : tmpWords)
                setToStr += i;

            map<string, Repeat>::iterator it = pt->repeats.find(setToStr);
            if (it == pt->repeats.end())
            {
                Repeat tmpRepeat;
                tmpRepeat._words = tmpWords;
                tmpRepeat.count = 1;
                //tmpRepeat.power ?????

                vector<Letter> tmpVector;
                for (auto& i : comb)
                    tmpVector.push_back(*i);
                for (int i = 0; i < tmpVector.size(); i++)
                {
                    bool flag = false;
                    for (int j = 0; j < tmpRepeat.letters.size(); j++)
                        if (tmpVector[i] == tmpRepeat.letters[j])
                            flag = true;
                    if (!flag)
                        tmpRepeat.letters.push_back(tmpVector[i]);
                }

                tmpRepeat.combs.push_back(pt->syllableCombinations[n_syll]);

                pt->repeats.insert(make_pair(setToStr, tmpRepeat));
            }
            else
            {
                it->second._words = tmpWords;
                it->second.count += 1;
                //it->second.power ?????

                vector<Letter> tmpVector;
                for (auto& i : comb)
                    tmpVector.push_back(*i);
                for (int i = 0; i < tmpVector.size(); i++)
                {
                    bool flag = false;
                    for (int j = 0; j < it->second.letters.size(); j++)
                        if (tmpVector[i] == it->second.letters[j])
                            flag = true;
                    if (!flag)
                        it->second.letters.push_back(tmpVector[i]);
                }

                it->second.combs.push_back(pt->syllableCombinations[n_syll]);
            }
        }
    }
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
        for (int j = 0; j < posCons.size(); j++)
            for (int k = j + 1; k < posCons.size(); k++)
                consCombs.push_back(make_pair(posCons[j], posCons[k]));

        // Сборка комбинаций из СГС в разных вариантах (в итераторах)
        vector<vector<forward_list<Letter>::iterator>> itCombs;
        for (int j = 0; j < consCombs.size(); j++)
        {
            // Создание сортированных комбинаций из пар индексов соглассных + индекс гласной
            vector<int> combs;
            combs.push_back(consCombs[j].first);
            combs.push_back(consCombs[j].second);
            combs.push_back(posVolve);
            sort(combs.begin(), combs.end());

            // Поиск итераторов элементов выбранных в пары
            forward_list<Letter>::iterator it1, it2, it3;
            it1 = it2 = it3 = pt->SP[i].first;

            for (int k = 0; k < combs[0]; k++)
                it1++;
            for (int k = 0; k < combs[1]; k++)
                it2++;
            for (int k = 0; k < combs[2]; k++)
                it3++;

            vector<forward_list<Letter>::iterator> tmpItCombs;
            tmpItCombs.push_back(it1);
            tmpItCombs.push_back(it2);
            tmpItCombs.push_back(it3);
            itCombs.push_back(tmpItCombs);
        }
        pt->syllableCombinations.push_back(itCombs);

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
            //if (startVolveIt->isVolve)
            middleVolveIt = endVolveIt;
            // дальше идёт ваше условие
        }

        endVolveIt = pt->basetext.end();
        startVolveIt++;
        dividedVolveIterators.push_back(make_pair(startVolveIt, endVolveIt));
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
        else if (symb.technic == "+")
            symb.technic = "&";

        if (symb.origin == " ")
            symb.technic = "-";

        if (symb.technic == "&" || symb.technic == "|")
            symb.printable = symb.technic;

        if (symb.technic == "&")
            symb.technic = symb.origin;
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
                    string tmp_str(tmp_c.substr(i, l));
                    tmp_str += tmp_c.substr(i + l);
                    it->technic = tmp_str;

                    needChange = true;
                }
            }
        }
        itPreviosLetter = it;
    }
}



void print(Phonotext pt)
{
    std::cout << "===========\n";
    std::cout << "-----------\n";
    std::cout << "origin      : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.origin;
    }
    std::cout << std::endl;
    std::cout << "-----------\n";
    std::cout << "technic     : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.technic;
    }
    std::cout << std::endl;
    std::cout << "-----------\n";
    std::cout << "printable   : ";
    for (auto& i : pt.basetext)
    {
        std::cout << i.printable;
    }
    std::cout << std::endl;
    std::cout << "-----------\n";
    std::cout << "isWord      : ";
    for (auto& i : pt.basetext)
    {
        if (i.isVolve)
            cout << "v";
        else if (i.isConsonant)
            cout << "c";
        else
            cout << "n";
    }
    std::cout << std::endl;
    std::cout << "-----------\n";
    std::cout << "SP          :\n";
    for (int i = 0; i < pt.SP.size(); i++)
    {
        cout << i << ": \"";
        for (auto it = pt.SP[i].first; it != pt.SP[i].second; it++)
        {
           cout << it->origin;
        }
        cout << "\"" << endl;
    }
    cout << endl;
    std::cout << "-----------\n";
    std::cout << "combinations:\n";
    for (int i = 0; i < pt.syllableCombinations.size(); i++)
    {
        cout << i << ":\n";
        for (int j = 0; j < pt.syllableCombinations[i].size(); j++)
        {
            for (int k = 0; k < 3; k++)
                cout << pt.syllableCombinations[i][j][k]->origin;
            cout << endl;
        }
        cout<< endl;
    }
    cout << endl;
    std::cout << "-----------\n";
    std::cout << "repeats:\n";
    for (auto& i : pt.repeats)
    {
        cout << "key : " << i.first << endl;
        cout << "Repeat.count : " << i.second.count << endl;
        cout << "Repeat.letters : ";
        for (int j = 0; j < i.second.letters.size(); j++)
            cout << i.second.letters[j].origin;
        cout << endl << "Repeat._words : ";
        for (auto& j : i.second._words)
            cout << j << "$";
        cout << endl << "Repeat.combs : ";
        for (int j = 0; j < i.second.combs.size(); j++)
            for (int k = 0; k < i.second.combs[j].size(); k++)
            {
                for (int l = 0; l < i.second.combs[j][k].size(); l++)
                    cout << i.second.combs[j][k][l]->origin;
                cout << " ";
            }
        cout << endl << endl;
    }
    cout << endl;
    std::cout << "-----------\n";
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

    std::cout << "\ncombinations\n";
    combinationsProccessor(pt);
    print(*pt);

    std::cout << "\nrepeat\n";
    repeatProccessor(pt);
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
    Phonotext pt("сегодня пришёл юнгатслтс");
    print(pt);

    proccessor(&pt, CONFIG);


    return 0;
}


// в repeatProccessor в третьем for в python файле идёт перебор comb[0], а здесь comb, потому что в comb[0] идёт перебор итераторов
// скорее всего причина в несходствах в глубине массива из-за того, что я по своему переписал функцию combinationsProccessor
// repeats.letters и repeats.combs сильно совпадают
// перепроверить получаемые результаты
// repeats.repeatPower и repeats.power ?????
// переписать / написать / проверить все моменты, где код закоментирован
// перепроверить и всунуть rus_filter_comb
// надо указывать min_pwr и max_pwr