#include <iostream>
#include <fstream>
#include <atlstr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <map>
#include <locale>
#include <windows.h>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

vector<char> makeAlphabetConfig(string jAlphabet) // Преобразования алфавита из строкового типа в массив букв
{
    vector<char> alphabet;
    for (auto& i : jAlphabet)
        alphabet.push_back(i);
    return alphabet;
}

map<char, string> makeAsOneConfig(vector<string> jAsOne) // Создание словаря: Буква с которой начинается комбинация букв - Сочетание символов (букв)
{
    map<char, string> asOne;

    for (auto& i : jAsOne)
        if (asOne.find((char)i[0]) == asOne.end())
            asOne.emplace((char)i[0], i);

    return asOne;
}

map<string, string> makeAsSameConfig(vector<vector<string>> jAsSame, string jAlphabet)  // Создание словаря: Уникальный символ - Как символ слышится ( {'a':'a', 'p':'b', 'ph':'f'} )
{
    map<string, string> asSame;

    for (int i = 0; i < jAlphabet.size(); i++) // Добавление всех букв алфавита в словарь: Буква - Буква
    {
        string tempStr(1, jAlphabet[i]);
        asSame.emplace(tempStr, tempStr);
    }
    for (int i = 0; i < jAsSame.size(); i++) // Добавление недостающих символов и сочетаний букв, замена значений словаря на "как слышится"
    {
        for (auto& j : jAsSame[i])
        {
            // Добавление уникальных сиволов и сочетаний букв
            if (asSame.find(j) == asSame.end())
                asSame.emplace(j, j);

            // Поиск и замена значений словаря на "как слышится"
            map<string, string> ::iterator it = asSame.find(j);
            it->second = jAsSame[i][0];
        }
    }

    return asSame;
}

map<char, map<char, string>> makeModificationsConfig(map<string, string> jDictionary) // Создания словаря: (Первая буква - (вторая буква - значение))
{
    map<char, map<char, string>> modifications;

    for (auto& i : jDictionary)
    {
        if (modifications.find(i.first[0]) == modifications.end()) // Добавление в словарь, если первая буква отсутствует
        {
            map<char, string> tmp;
            tmp.emplace(i.first[1], i.second);
            modifications.emplace(i.first[0], tmp);
        }
        else // Добавление в подсловарь, если первая буква присутствует
        {
            map<char, map<char, string>> ::iterator it = modifications.find(i.first[0]);
            it->second.emplace(i.first[1], i.second);
        }
    }

    return modifications;
}


void makeConfig(string lngPath)
{
    ifstream fin(lngPath); // Подгрзка файла json
    json data = json::parse(fin); // Выделение json файла

    vector<char> alphabet = makeAlphabetConfig(data["alphabet"]); // Преобразования алфавита из строкового типа в массив букв
    
    vector<string> consonants = data["consonants"]; // Выборка символов из consonants

    vector<string> volves = data["volves"]; // Выборка символов из volves

    // Соединение consonants и volves в один массив
    vector<string> words;
    for (auto& i : consonants)
        words.push_back(i);
    for (auto& i : volves)
        words.push_back(i);

    // Создание словаря: Буква с которой начинается комбинация букв - Сочетание символов (букв)
    vector<string> jAsOne;
    for (auto& i : data["as_one"])
        jAsOne.push_back(i);
    map<char, string> asOne = makeAsOneConfig(jAsOne);

    map<string, string> asSame = makeAsSameConfig(data["as_same"], data["alphabet"]); // Создание словаря: Уникальный символ - Как символ слышится ( {'a':'a', 'p':'b', 'ph':'f'} )

    // Создания словаря: (Первая буква - (вторая буква - значение))
    map<string, string> jDictionary;
    for (json::iterator it = data["modifications"].begin(); it != data["modifications"].end(); it++)
        jDictionary.emplace(it.key(), it.value());
    map<char, map<char, string>> modifications = makeModificationsConfig(jDictionary);
}

void defineLng(string lng)
{
    if (lng == "rus")
        makeConfig("./lng json/russian.json");
    else if (lng == "eng")
        makeConfig("./lng json/english.json");
    else if (lng == "lat")
        makeConfig("./lng json/latin.json");
}


int main()
{
    system("chcp 65001");

    defineLng("rus"); // Выбор языка

    return 0;
}