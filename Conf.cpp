#include "conf.h"

Conf::Conf()
{
}

Conf::Conf(std::string lng)
{
    if (lng == "rus")
        makeConfig("./lng json/russian.json");
    else if (lng == "eng")
        makeConfig("./lng json/english.json");
    else if (lng == "lat")
        makeConfig("./lng json/latin.json");
}

Conf::~Conf()
{
}

//string utf8_to_string(const char* utf8str, const locale& loc)
//{
//    // UTF-8 to wstring
//    wstring_convert<codecvt_utf8<wchar_t>> wconv;
//    wstring wstr = wconv.from_bytes(utf8str);
//    // wstring to string
//    vector<char> buf(wstr.size());
//    use_facet<ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
//    return string(buf.data(), buf.size());
//}


void Conf::makeConfig(std::string lngPath)
{
    std::ifstream fin(lngPath); // Ïîäãðçêà ôàéëà json
    nlohmann::json data = nlohmann::json::parse(fin); // Âûäåëåíèå json ôàéëà

    makeAlphabetConfig(data["alphabet"]); // Ïðåîáðàçîâàíèÿ àëôàâèòà èç ñòðîêîâîãî òèïà â ìàññèâ áóêâ

    //string d = data["alphabet"];
    //d = utf8_to_string(d.c_str(), locale(".1252"));

    consonants = data["consonants"]; // Âûáîðêà ñèìâîëîâ èç consonants

    volves = data["volves"]; // Âûáîðêà ñèìâîëîâ èç volves

    // Ñîåäèíåíèå consonants è volves â îäèí ìàññèâ
    for (auto& i : consonants)
        words.push_back(i);
    for (auto& i : volves)
        words.push_back(i);

    // Ñîçäàíèå ñëîâàðÿ: Áóêâà ñ êîòîðîé íà÷èíàåòñÿ êîìáèíàöèÿ áóêâ - Ñî÷åòàíèå ñèìâîëîâ (áóêâ)
    std::vector<std::string> jAsOne;
    for (auto& i : data["as_one"])
        jAsOne.push_back(i);
    makeAsOneConfig(jAsOne);

    makeAsSameConfig(data["as_same"], data["alphabet"]); // Ñîçäàíèå ñëîâàðÿ: Óíèêàëüíûé ñèìâîë - Êàê ñèìâîë ñëûøèòñÿ ( {'a':'a', 'p':'b', 'ph':'f'} )

    // Ñîçäàíèÿ ñëîâàðÿ: (Ïåðâàÿ áóêâà - (âòîðàÿ áóêâà - çíà÷åíèå))
    std::map<std::string, std::string> jDictionary;
    for (nlohmann::json::iterator it = data["modifications"].begin(); it != data["modifications"].end(); it++)
        jDictionary.emplace(it.key(), it.value());
    makeModificationsConfig(jDictionary);
}

void Conf::makeAlphabetConfig(std::string jAlphabet)
{
    for (auto& i : jAlphabet)
        alphabet.push_back(i);
}

void Conf::makeAsOneConfig(std::vector<std::string> jAsOne)
{
    for (auto& i : jAsOne)
    {
        auto it = asOne.find((char)i[0]);
        if (it == asOne.end())
            asOne.emplace((char)i[0], i);
        else
            it->second = i;
    }
        
}

void Conf::makeAsSameConfig(std::vector<std::vector<std::string>> jAsSame, std::string jAlphabet)
{
    for (int i = 0; i < jAlphabet.size(); i++) // Äîáàâëåíèå âñåõ áóêâ àëôàâèòà â ñëîâàðü: Áóêâà - Áóêâà
    {
        std::string tempStr(1, jAlphabet[i]);
        asSame.emplace(tempStr, tempStr);
    }
    for (int i = 0; i < jAsSame.size(); i++) // Äîáàâëåíèå íåäîñòàþùèõ ñèìâîëîâ è ñî÷åòàíèé áóêâ, çàìåíà çíà÷åíèé ñëîâàðÿ íà "êàê ñëûøèòñÿ"
    {
        for (auto& j : jAsSame[i])
        {
            // Äîáàâëåíèå óíèêàëüíûõ ñèâîëîâ è ñî÷åòàíèé áóêâ
            if (asSame.find(j) == asSame.end())
                asSame.emplace(j, j);

            // Ïîèñê è çàìåíà çíà÷åíèé ñëîâàðÿ íà "êàê ñëûøèòñÿ"
            std::map<std::string, std::string> ::iterator it = asSame.find(j);
            it->second = jAsSame[i][0];
        }
    }
}

void Conf::makeModificationsConfig(std::map<std::string, std::string> jDictionary)
{
    for (auto& i : jDictionary)
    {
        if (modifications.find(i.first[0]) == modifications.end()) // Äîáàâëåíèå â ñëîâàðü, åñëè ïåðâàÿ áóêâà îòñóòñòâóåò
        {
            std::map<char, std::string> tmp;
            tmp.emplace(i.first[1], i.second);
            modifications.emplace(i.first[0], tmp);
        }
        else // Äîáàâëåíèå â ïîäñëîâàðü, åñëè ïåðâàÿ áóêâà ïðèñóòñòâóåò
        {
            std::map<char, std::map<char, std::string>> ::iterator it = modifications.find(i.first[0]);
            it->second.emplace(i.first[1], i.second);
        }
    }
}
