#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <map>
#include <algorithm>


class Conf
{
public:
    Conf();
    Conf(std::string lng);
    ~Conf();

    std::map<char, std::map<char, std::string>> getModifications() { return modifications; }
    std::map<std::string, std::string> getAsSame() { return asSame; }
    std::map<char, std::string> getAsOne() { return asOne; }

private:
    std::map<char, std::map<char, std::string>> modifications;
    std::map<char, std::string> asOne;
    std::vector<std::string> consonants;
    std::vector<std::string> volves;
    std::vector<std::string> words;
    std::map<std::string, std::string> asSame;
    std::vector<char> alphabet;

    void makeConfig(std::string lngPath);

    void makeAlphabetConfig(std::string jAlphabet);
    void makeAsOneConfig(std::vector<std::string> jAsOne);
    void makeAsSameConfig(std::vector<std::vector<std::string>> jAsSame, std::string jAlphabet);
    void makeModificationsConfig(std::map<std::string, std::string> jDictionary);
};

