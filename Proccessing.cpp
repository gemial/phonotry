#include "Proccessing.h"

Proccessing::Proccessing(Phonotext pt, std::string lng, double min_pwr, double max_pwr)
{
    std::cout << "proccess start\n";
    this->pt = pt;
    this->min_pwr = min_pwr;
    this->max_pwr = max_pwr;

    std::cout << "Add config\n";
    Conf CONF(lng);
    this->CONFIG = CONF;
    std::cout << "Config added\n";

    proccess();
}

// Ïîëíàÿ îáðàáîòêà òåêñòà
void Proccessing::proccess()
{

    std::cout << "modify\n";
    modifyProccessor(); // Çàìåíà
    std::cout << "same\n";
    sameProccessor(); // Çàìåíà è çàïèñü
    std::cout << "join\n";
    joinProccessor(); // Îáúåäèíåíèå
    std::cout << "number\n";
    numberProccessor(); // Ïîäñ÷¸ò
    std::cout << "finder\n";
    finderVolve(); // Ïîèñê
    std::cout << "SP\n";
    SPmaxProccessor(); // Âûáîðêà
    std::cout << "combinations\n";
    combinationsProccessor(); // Êîìáèíèðîâàíèå
    std::cout << "repeat\n";
    repeatProccessor(); // Ïîäñ÷¸ò
    std::cout << "proccessing end\n";
    createJson();
    std::cout << "creating end\n";
}


// Èçìåíåíèå ñèìâîëîâ ïî json
// Åñëè èçìåíÿòü, òî òîëüêî íàïðÿìóþ ñ ïðåïîäàâàòåëåì
void Proccessing::modifyProccessor()
{
    std::string tmp_a;
    std::string tmp_b;
    std::string tmp_c;

    auto itPreviosLetter = pt.basetext.begin();

    bool needChange = false;
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
    {
        if (needChange)
        {
            // it->printable = "`";
            needChange = false;
        }
        if (it == pt.basetext.begin())
        {
            tmp_b = it->origin;
        }
        else
        {
            tmp_a = tmp_b;
            tmp_b = it->origin;

            auto modFirstKey = CONFIG.getModifications()->find(tmp_a);
            auto modFirstKeyEnd = CONFIG.getModifications()->end();
            if (modFirstKey != modFirstKeyEnd)
            {
                int i, l;
                auto modSecondKey = modFirstKey->second.find(tmp_b);
                auto modSecondKeyEnd = modFirstKey->second.end();
                if (modSecondKey != modSecondKeyEnd)
                {
                    tmp_c = modSecondKey->second;
                    for (i = 0; tmp_c[0] & (0x80 >> i); ++i); i = (i) ? i : 1; // find first letter
                    for (l = 0; tmp_c[i] & (0x80 >> l); ++l); l = (l) ? l : 1; // find second letter

                    itPreviosLetter->origin = tmp_c.substr(0, i);
                    it->origin = tmp_c.substr(i, l);
                    pt.basetext.emplace_after(it, Letter(tmp_c.substr(i + l)));
                    auto b = it;
                    (++b)->printable = it->printable;
                    it->printable = "`";
                    needChange = true;
                }

            }
        }
        itPreviosLetter = it;
    }
}


// Çàïèñü technic è printable, à òàê æå çàìåíà ñèìâîëîâ â ïåðâîì, åñëè îíè ÿâëÿþòñÿ îäèíàêîâûìè ïî json
// P.S. ñòîèò âîïðîñ ïî ïåðåïèñè
void Proccessing::sameProccessor()
{
    for (auto& symb : pt.basetext)
    {
        std::map<std::string, std::string> asSame = CONFIG.getAsSame();
        auto sameKey = asSame.find(symb.printable);
        auto sameKeyEnd = asSame.end();
        if (sameKey != sameKeyEnd)
            symb.technic = sameKey->second;
        else if (symb.technic == "+")
            symb.technic = "&";

        if (symb.origin == " ")
            symb.technic = "|";

        // if (symb.technic == "&" || symb.technic == "|")
            // symb.printable = symb.technic;

        if (symb.technic == "&")
            symb.technic = symb.origin;
    }
}

// Îáúåäèíåíèå îáúåêòîâ, åñëè îíè ÿâëÿþòñÿ îäíèì ñèìâîëîì
void Proccessing::joinProccessor()
{
    std::string tmp_a; // Ïåðâûé ñèìâîë ñîâìåùàþùåéñÿ â êîìáèíàöèè
    std::string tmp_b; // Âòîðîé ñèìâîë ñîâìåùàþùåéñÿ â êîìáèíàöèè

    // Ïîèñê èòåðàòîðà íà ïîñëåäíèé îáúåêò â Phonotext
    auto itLast = pt.basetext.begin();
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        itLast = it;

    auto it = pt.basetext.begin(); // Äàííàÿ ïåðåìåííàÿ áóäåò èòåðàòîðîì íà îáúåêò, ñ êîòîðûì ïðîèñõîäèò ðàáîòà â äàííûé ìîìåíò
    auto itPreviosLetter = pt.basetext.begin(); // Äàííàÿ ïåðåìåííàÿ áóäåò óêàçàòåëåì íà îáúåêò, ñ êîòîðûì ïðîèñõîäèëà ðàáîòà â ïðîøëûé ðàç

    while (it != pt.basetext.end()) // Ïðîõîäêà äî êîíöà
    {
        if (it == pt.basetext.begin()) // Â ïåðâûé ðàç óêàçûâàåòñÿ òîëüêî ïåðâûé ñèìâîë èùóùåéñÿ êîìáèíàöèè
            tmp_a = it->origin;
        else
        {
            tmp_a = itPreviosLetter->origin; // Çàïèñü ïåðâîãî ñèìâîëà (origin èç ïðåäûäóùåãî èòåðàòîðà)
            tmp_b = it->origin; // Çàïèñü âòîðîãî ñèìâîëà (origin èç íàñòîÿùåãî ñèìâîëà)

            std::map<char, std::string> asOne = CONFIG.getAsOne();
            auto oneKey = asOne.find(tmp_a[0]); // Ïîèñê êîìáèíàöèè ïî ïåðâîìó ñèìâîëó
            auto oneKeyEnd = asOne.end();
            if (oneKey != oneKeyEnd) // Åñëè êîìáèíàöèÿ íàéäåíà
            {
                if ((tmp_a + tmp_b) == oneKey->second) // Åñëè íàøà ïàðà ñîâïàäàåò ñ êîìáèíàöèåé
                {
                    // Ïåðåçàïèñü ïðîøëîãî èòåðàòîðà
                    itPreviosLetter->origin = oneKey->second;
                    itPreviosLetter->printable = oneKey->second;
                    itPreviosLetter->technic = oneKey->second;
                    if (it == itLast) // Åñëè ýòî ïîñëåäíèé ýëåìåíò, òî óäàëåíèå è âûõîä
                    {
                        pt.basetext.erase_after(itPreviosLetter);
                        break;
                    }
                    // Åñëè ýòî íå ïîñëåäíèé ýëåìåíò, òî ïåðåñêîê íà ñëåäóþùèé èòåðàòîð, âî èçáåæàíèå îøèáêè è óäàëåíèå íàñòîÿùåãî ñèìâîëà
                    it++;
                    pt.basetext.erase_after(itPreviosLetter);
                }
            }
            itPreviosLetter = it; // Ñìåíà ïðîøëîãî èòåðàòîðà íà íàñòîÿùèé
        }
        it++; // Ñëåäóþùèé èòåðàòîð
    }
}

// Ôóíêöèÿ äëÿ ïîäñ÷¸òà ðàçëè÷íîé èíôîðìàöèè ïî òåêñòó
void Proccessing::numberProccessor()
{
    int i = 0; // Äëÿ ïîäñ÷¸òà íîìåðà áóêâû â òåêñòå
    int j = 1; // Äëÿ ïîäñ÷¸òà ñëîãà â òåêñòå
    int k = 1; // Äëÿ ïîäñ÷¸òà ñëîâà â òåêñòå
    int num = 1; // Íå èñïîëüçóåòñÿ, íî â îðèãèíàëå áûëî
    bool space = false; // Íåñ¸ò èíôîðìàöèþ, ÿâëÿåòñÿ ëè äàííûé èòåðèðóåìûé îáúåêò ïðîáåëîì
    int space_pos = 0; // Äëÿ ïîäñ÷¸òà íàõîæäåíèÿ îáúåêòà â ñëîâå ïîñëå ïðîáåëà

    int l_number = 0; // Íå èñïîëüçóåòñÿ, íî â îðèãèíàëå áûëî
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++, l_number++)
    {
        it->number = i;
        it->word = k;

        if (it->technic == " " || it->technic == "|" || it->technic == "\n")
        {
            space_pos = 0;
            space = true;
        }
        else
        {
            std::vector<std::string> words = CONFIG.getWords();
            space_pos++;
            // Äëÿ ÷åãî âñ¸ äåëàåòñÿ äàëüøå íå ñîâñåì ïîìíþ, íî ïî-ôàêòó ýòî íè÷åãî íå äåëàåò
            bool flag = false;
            for (int t = 0; t < words.size(); t++)
                if (it->technic == words[t])
                    flag = true;
            space = (space && flag);
            // Äî ýòîãî ìîìåíòà
        }
        if (it->technic == "\n")
        {
            // îáðàáîòêà
            // P.S. õç ïî÷åìó ÿ íå ðåðàéòíóë îáðàáîòêó äëÿ ýòîãî, ìá êàêàÿ òî ïðîáëåìà
        }
        // Îïðåäåëåíèå ãëàñíûõ îáúåêòîâ
        std::vector<std::string> volves = CONFIG.getVolves();
        for (int t = 0; t < volves.size(); t++)
            if (it->printable == volves[t])
            {
                it->isVolve = true;
                j++;
                break;
            }
        // Îïðåäåëåíèå ñîãëàñíûõ îáúåêòîâ
        std::vector<std::string> consonants = CONFIG.getConsonants();
        for (int t = 0; t < consonants.size(); t++)
            if (it->printable == consonants[t])
            {
                it->isConsonant = true;
                break;
            }
        it->syll = j;
        i++;
        num++;
        it->w_pos = space_pos; // Âîçíèêàëà îùèáêà ñ ýòèì, à èìåííî, çíà÷åíèÿ íå ñîõðàíÿëèñü
    }
}

// Íàõîæäåíèå èòåðàòîðîâ ãëàñíûõ è äîáàâëåíèå âîçâðàò ìàññèâà ñî âñåìè ýòèìè èòåðàòîðàìè
void Proccessing::finderVolve()
{
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        if (it->isVolve)
            volveIterators.push_back(it);
}

// Ñîçäàíèå ìàññèâà ýëåìåíòîâ, êîòîðûå ïðåäñòàâëÿþò ñîáîé 
// èòåðàòîðû íà ñëåäóþùþþ ñîãëàñíóþ ïîñëå ïðîøëîé ãëàñîé è ïîñëåäíþþ ñîãëàñíóþ ïåðåä ñëåäóþùåé ãëàñíîé
void Proccessing::SPmaxProccessor()
{
    std::vector<std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator>> dividedVolveIterators;

    std::forward_list<Letter>::iterator startVolveIt, middleVolveIt, endVolveIt; // èòåðàòîðû íà ïðîøëóþ, íàñòîÿùóþ è ñëåäóþùóþ ãëàñíóþ
    bool firstVolve = false;

    if (volveIterators.size() > 1) // åñëè ãëàñíûõ áîëüøå îäíîé
    {
        // ïåðâîíà÷àëüíîå ðàñïîëîæåíèå èòåðàòîðà â SP - âòîðîé ñèìâîë, ò.ê. ïåðâûé - ïðîáåë ïî óìîë÷àíèþ
        startVolveIt = pt.basetext.begin();
        startVolveIt++;
        // èòåðàòîð íà ïåðâûé öåíòð = èòåðàòîð íà ïåðâóþ ãëàñíóþ
        middleVolveIt = volveIterators[0];

        for (int i = 1; i < volveIterators.size(); i++) // ïðîõîä ïî âñåì èòåðàòîðàì ãëàñíûõ, íà÷èíàÿ ñî âòîðîãî
        {
            endVolveIt = volveIterators[i]; // êîíå÷íûé èòåðàòîð ýëåìåíòà ìàññèâà, óêàçûâàþùèé íà ïîñëåäíþþ ñîãëàñíóþ ïåðåä ñëåäóþùåé ãëàñíîé

            if (middleVolveIt->origin != "\n")
                dividedVolveIterators.push_back(std::make_pair(startVolveIt, endVolveIt)); // Ñîçäàíèå ïàðû íà ïîñëåäíþþ ñîãëàñíóþ ïîñëå ïîñëåäíåé ãëàñíîé è ïîñëåäíåé ñîãëàñíîé ïåðåä ñëåäóþùåé ãëàñíîé

            // ñëåäóþùåå íà÷àëî - ñëåäóþùàÿ ãëàñíàÿ ïîñëå òîé, êîòîðàÿ ÿâëÿåòñÿ öåíòðàëüíîé íà äàííûé ìîìåíò
            // ñëåäîâàòåëüíî ñìåùàåì è ïåðåíàçíà÷àåì
            middleVolveIt++;
            startVolveIt = middleVolveIt;
            // ñëåäóþùàÿ öåíòðàëüíàÿ - òà, êîòîðàÿ ÿâëÿëàñü ñëåäóþùåé íà äàííîì öèêëå
            middleVolveIt = endVolveIt;
            // äàëüøå èä¸ò óñëîâèå èç îðèãèíàëà íà óäàðåíèå

            int counter = 0;
            for (auto it = startVolveIt; it != endVolveIt; it++)
                if (it->technic == "\n")
                    counter++;
        }

        // äîáàâëåíèå ýëåìåíòå ìàññèâà, êîòîðûé âêëþ÷àåò ñîãëàñíûå ïîñëå ïîñëåäíåé ãëàñíîé
        endVolveIt = pt.basetext.end();
        startVolveIt++;

        if (middleVolveIt->origin != "\n")
            dividedVolveIterators.push_back(std::make_pair(startVolveIt, endVolveIt));
    }
    else // åñëè ãëàñíàÿ îäíà - ýëåìåíò â ìàññèâå òîëüêî îäèí: âåñü Phonotext
        dividedVolveIterators.push_back(std::make_pair(pt.basetext.begin(), pt.basetext.end()));

    pt.SP = dividedVolveIterators; // ïåðåíîñ âñåõ ýëåìåíòîâ â Phonotext
}

// Íàõîæäåíèå èíäåêñîâ ãëàñíîé è ñîãëàñíûõ
std::pair<int, std::vector<int>> Proccessing::findLocalWordsInds(std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator> localSP)
{
    int indVolve;
    std::vector<int> indCons;

    int i = 0;
    for (auto it = localSP.first; it != localSP.second; it++, i++)
        if (it->isVolve)
            indVolve = i;
        else
            indCons.push_back(i);

    return std::make_pair(indVolve, indCons);
}

// Ñîçäàíèå êîìáèíàöèé
void Proccessing::combinationsProccessor(int N)
{
    N++;
    for (int i = 0; i < pt.SP.size(); i++)
    {
        // Èç êàæäîãî ó÷àñòêà âûáèðàåòñÿ ãëàñíàÿ è âñå ñîãëàñíûå â îòäåëüíûé ìàññèâ
        std::pair<int, std::vector<int>> localInds = findLocalWordsInds(pt.SP[i]);
        int posVolve = localInds.first;
        std::vector<int> posCons = localInds.second;

        // Âñåâîçìîæíûå ïàðû êîìáèíàöèé èç èíäåêñîâ ìàññèâà ñîãëàñíûõ
        std::vector<std::pair<int, int>> consCombs;
        for (int j = 0; j < posCons.size(); j++)
            for (int k = j + 1; k < posCons.size(); k++)
                consCombs.push_back(std::make_pair(posCons[j], posCons[k]));

        // Ñáîðêà êîìáèíàöèé èç ÑÃÑ â ðàçíûõ âàðèàíòàõ (â èòåðàòîðàõ)
        std::vector<std::vector<std::forward_list<Letter>::iterator>> itCombs;
        for (int j = 0; j < consCombs.size(); j++)
        {
            // Ñîçäàíèå ñîðòèðîâàííûõ êîìáèíàöèé èç ïàð èíäåêñîâ ñîãëàññíûõ + èíäåêñ ãëàñíîé
            std::vector<int> combs;
            combs.push_back(consCombs[j].first);
            combs.push_back(consCombs[j].second);
            combs.push_back(posVolve);
            sort(combs.begin(), combs.end());

            // Ïîèñê èòåðàòîðîâ ýëåìåíòîâ âûáðàííûõ â ïàðû
            std::forward_list<Letter>::iterator it1, it2, it3;
            it1 = it2 = it3 = pt.SP[i].first;

            for (int k = 0; k < combs[0]; k++)
                it1++;
            for (int k = 0; k < combs[1]; k++)
                it2++;
            for (int k = 0; k < combs[2]; k++)
                it3++;
            
            int counter = 0;
            if (it1->origin == "\n")
                counter++;
            if (it2->origin == "\n")
                counter++;
            if (it3->origin == "\n")
                counter++;
            
            bool isCorrect = true;
            if (it1->origin == "\n" || it2->origin == "\n" || it3->origin == "\n")
                isCorrect = false;
            if (it1->origin == "-" || it2->origin == "-" || it3->origin == "-")
                isCorrect = false;
            if (it1->origin == "," || it2->origin == "," || it3->origin == ",")
                isCorrect = false;
            if (it1->origin == "!" || it2->origin == "!" || it3->origin == "!")
                isCorrect = false;
            if (it1->origin == "?" || it2->origin == "?" || it3->origin == "?")
                isCorrect = false;
            if (it1->origin == "j" || it2->origin == "j" || it3->origin == "j")
                isCorrect = false;

            if (isCorrect)
            {
                std::vector<std::forward_list<Letter>::iterator> tmpItCombs;
                tmpItCombs.push_back(it1);
                tmpItCombs.push_back(it2);
                tmpItCombs.push_back(it3);
                itCombs.push_back(tmpItCombs);
            }
        }
        if (itCombs.size())
            pt.syllableCombinations.push_back(itCombs);

        // Äîáàâèòü òî, ÷òî âûòàñêèâàåòñÿ èç èòåðàòîðîâ è îáðàáîòêó èç filter_combination
    }
}

// Ôóíêöèÿ, êîòîðàÿ ïîäñ÷èòûâàåò êîëè÷åñòâî ïîâòîðåíèé êîìáèíàöèé è èõ ñèëû
void Proccessing::repeatProccessor()
{
    for (int n_syll = 0; n_syll < pt.syllableCombinations.size(); n_syll++)
    {
        for (auto& comb : pt.syllableCombinations[n_syll])
        {
            std::pair<std::vector<std::string>, std::map<int, int>> a;

            // Ñîçäàíèå ñòðóêòóðû Repeat, â êîòîðîé ðàñïîëîãàþòñÿ êîìáèíàöèè è èíôà î íèõ (êîìáèíàöèè ñîñòîÿò èç ÃÃ: ñîãëàñíàÿ + ñîãëàñíàÿ)
            for (auto& i : comb)
            {
                if (i->isConsonant)
                    a.first.push_back(i->technic);
                a.second.insert(std::make_pair(i->number, n_syll)); // Äîáàâëåíèå ñîãëàñíûõ èç ïîïàäàþùèõñÿ â êîìáèíàöèè
            }

            // Óäàëåíèå ïîâòîðÿþùèõñÿ ñèìâîëîâ
            std::set<std::string> tmpWords(a.first.begin(), a.first.end());
            std::string setToStr = "";
            for (auto& i : tmpWords)
                setToStr += i;

            // Îòáðàñûâàíèå íåâàëèäíûõ êîìáèíàöèé
            std::pair<bool, double> filter = rusFilterComb(comb, CONFIG.getWords());
            if (filter.first)
            {
                std::map<std::string, Repeat>::iterator it = pt.repeats.find(setToStr); // Ïðîâåðêà íà ïðèñóòñòâèå äàííîé êîìáèíàöèè â óæå äîáàâëåííûõ
                if (it == pt.repeats.end()) // Ñîçäàíèå ñòðóêòóðû Repeat, åñëè ñòðóêòóðû ñ òàêîé êîìáèíàöèè åù¸ íåáûëî
                {
                    Repeat tmpRepeat;
                    tmpRepeat._words = tmpWords; // Óíèêàëüíûå ñèìâîëû ñòðóêòóðû
                    tmpRepeat.count = 1; // Ñòðóêòóðà òîëüêî ñîçäà¸òñÿ, ñëåäîâàòåëüíî ýòî òîëüêî ïåðâàÿ äîáàâëåííàÿ
                    tmpRepeat.power = filter.second; // Äîáàâëåíèå ñèëû ïåðâîé äîáàâëåííîé êîìáèíàöèè

                    std::vector<Letter> tmpvector;
                    for (auto& i : comb) // Äîáàâëåíèå âñåõ îáúåêòîâ êëàññà Letter, êîòîðûå ïðèñóòñòâóþò â êîìáèíàöèè âî âðåìåííûé ìàññèâ
                        tmpvector.push_back(*i);
                    for (int i = 0; i < tmpvector.size(); i++) // Äîáàâëåíèå óíèêàëüíûõ îáúåêòîâ êëàññà Letter èç âðåìåííîãî ìàññèâà â ñòðóêòóðó
                    {
                        bool flag = false;
                        for (int j = 0; j < tmpRepeat.letters.size(); j++)
                            if (tmpvector[i] == tmpRepeat.letters[j])
                                flag = true;
                        if (!flag)
                            tmpRepeat.letters.push_back(tmpvector[i]);
                    }

                    tmpRepeat.combs.push_back(comb); // Äîáàâëåíèå âñåõ êîìáèíàöèè â ñòðóêòóðó

                    if (tmpRepeat._words.size() != 1)
                        pt.repeats.insert(std::make_pair(setToStr, tmpRepeat)); // Ïåðåíîñ ñòðóêòóðû â îáúåêò Phonotext
                }
                else
                {
                    it->second._words = tmpWords; // Óíèêàëüíûå ñèìâîëû ñòðóêòóðû
                    it->second.count += 1; // Òàê êàê íàøëàñü åù¸ îäíà êîìáèíàöèÿ, ñ÷¸ò÷èê óâåëè÷èâàåòñÿ
                    it->second.power = filter.second; // Äîáàâëåíèå ñèëû ñëåäóþùåé äîáàâëåííîé êîìáèíàöèè

                    std::vector<Letter> tmpvector; // Óíèêàëüíûå ñèìâîëû ñòðóêòóðû
                    for (auto& i : comb)
                        tmpvector.push_back(*i);
                    for (int i = 0; i < tmpvector.size(); i++) // Äîáàâëåíèå óíèêàëüíûõ îáúåêòîâ êëàññà Letter èç âðåìåííîãî ìàññèâà â ñòðóêòóðó
                    {
                        bool flag = false;
                        for (int j = 0; j < it->second.letters.size(); j++)
                            if (tmpvector[i] == it->second.letters[j])
                                flag = true;
                        if (!flag)
                            it->second.letters.push_back(tmpvector[i]);
                    }

                    it->second.combs.push_back(comb); // Äîáàâëåíèå âñåõ êîìáèíàöèè â ñòðóêòóðó

                }
            }
        }
    }
    pt.repeats.erase(pt.repeats.begin());
}

// Ôóíêöèÿ ñîçäàíà äëÿ ôèëüòðàöèè êîìáèíàöèé, âîçâðàùàåò ïåðâûì ïàðàìåòðîì <false, 0>, åñëè êîìáèíàöèÿ ÿâëÿåòñÿ îòáðàñûâàåìîé,
// âîçâðàùàåò <true, pwr>, åñëè êîìáèíàöèÿ âàëèäíà, ãäå pwr - ñèëà êîìáèíàöèè â òåêñòå
// Äîðàáîòàòü: ñäåëàòü ïðîâåðêó íà òî, ÷òî òåêñò, ïîëó÷àåìûé èç êîìáèíàöèè ÿâëÿþòñÿ äâóêîíñîíàíòíûì
std::pair<bool, double> Proccessing::rusFilterComb(std::vector<std::forward_list<Letter>::iterator> comb, std::vector<std::string> words)
{
    // Çàïèñü technic ñèìâîëîâ êîìáèíàöèè
    std::string tmptxt = "";
    for (auto it = comb[0]; it != comb[2]; it++)
        tmptxt += it->technic;

    // Îòáðàñûâàíèå ïîâòîðÿþùèõñÿ ñèìâîëîâ
    std::string txt = "";
    for (int i = 0; i < tmptxt.size(); i++)
    {
        bool notUnique = false;
        for (int j = 0; j < txt.size(); j++)
            if (tmptxt[i] == txt[j])
                notUnique = true;

        if (!notUnique)
            txt += tmptxt[i];
    }

    // Êîìáèíàöèÿ íå âàëèäíà, åñëè ïîëó÷àåìûé òåêñò ìåíüøå òð¸õ ñèìâîëîâ
    if (txt.size() < 3)
        return std::make_pair(false, 0);

    // Ïîäñ÷¸ò ñèëû êîìáèíàöèè
    double pwr;
    if (comb[0]->isVolve) // Åñëè ãëàñíàÿ ïåðâàÿ â êîìáèíàöèè
        pwr = 2;
    else if (comb[2]->isVolve) // Åñëè ãëàñíàÿ ïîñëåäíÿÿ â êîìáèíàöèè
        pwr = 1;
    else // Åñëè ãëàñíàÿ â ñåðåäèíå êîìáèíàöèè
        pwr = 3;

    int count = 0;
    for (auto& i : txt) // Ïîäñ÷¸ò êîëè÷åñòâà ïðîáåëîâ
        if (i == '|')
            count++;
    pwr += (comb[2]->number - comb[0]->number - count == 2 ? 5 : 0); // Ðàñïîëîæåíèå ãëàñíûõ áåç ïðîáåëîâ
    pwr += (count == 0 ? 2 : 0); // êîëè÷åñòâî ïðîáåëîâ
    count = 0;
    for (int i = txt.size() - 3; i < txt.size(); i++) // ïîäñ÷¸ò êîëè÷åñòâà áóêâ "é" â ïîñëåäíèõ òð¸õ ñèìâîëàõ ïîëó÷àåìîãî òåêñòà
        if (txt[i] == 'é')
            count++;
    pwr += (count == 0 ? 4 : 0);
    pwr += (!comb[0]->w_pos || !comb[1]->w_pos || !comb[2]->w_pos ? 1 : 0); // Ïðîâåðêà íà òî, ÿâëÿåòñÿ ëè ñèìâîëû èç êîìáèíàöèè ïåðâûì â ñëîâå
    pwr /= 15;

    return std::make_pair(min_pwr <= pwr <= max_pwr, pwr);
}

// Âûâîä
void Proccessing::print()
{
    std::ofstream fout;
    fout.open("out.txt");
    fout << "===========\n";

    fout << "-----------\n";
    fout << "origin      : ";
    for (auto& i : pt.basetext)
    {
        fout << i.origin;
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "technic     : ";
    for (auto& i : pt.basetext)
    {
        fout << i.technic;
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "printable   : ";
    for (auto& i : pt.basetext)
    {
        fout << i.printable;
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "isWord      : ";
    for (auto& i : pt.basetext)
    {
        if (i.isVolve)
            fout << "v";
        else if (i.isConsonant)
            fout << "c";
        else
            fout << "n";
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "w_pos      : ";
    for (auto& i : pt.basetext)
    {
        fout << i.w_pos;
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "SP          :\n";
    for (int i = 0; i < pt.SP.size(); i++)
    {
        fout << i << ": \"";
        for (auto it = pt.SP[i].first; it != pt.SP[i].second; it++)
        {
            fout << it->technic;
        }
        fout << "\"" << std::endl;
    }
    fout << std::endl;
    for (auto& i : pt.basetext)
    {
        fout << i.origin << i.technic << i.printable << i.isVolve << i.isConsonant << i.number << '|';
    }

    fout << "-----------\n";

    fout << "combinations:\n";
    for (int i = 0; i < pt.syllableCombinations.size(); i++)
    {
        fout << i << ":\n";
        for (int j = 0; j < pt.syllableCombinations[i].size(); j++)
        {
            for (int k = 0; k < 3; k++)
                fout << pt.syllableCombinations[i][j][k]->origin;
            fout << std::endl;
        }
        fout << std::endl;
    }
    fout << std::endl;

    fout << "-----------\n";
    fout << "repeats:\n";
    for (auto& i : pt.repeats)
    {
        fout << "key : " << i.first << std::endl;
        fout << "Repeat.power : " << i.second.power << std::endl;
        fout << "Repeat.count : " << i.second.count << std::endl;
        fout << "Repeat.letters : ";
        for (int j = 0; j < i.second.letters.size(); j++)
            fout << i.second.letters[j].origin;
        fout << std::endl << "Repeat._words : <";
        for (auto& j : i.second._words)
            fout << j << "><";
        fout << ">\n";
        fout << "Repeat.combs : ";
        for (int j = 0; j < i.second.combs.size(); j++)
        {
            for (int k = 0; k < i.second.combs[j].size(); k++)
                fout << i.second.combs[j][k]->origin;
            fout << " ";
        }
        fout << std::endl << std::endl;
    }
    fout << std::endl;
    fout << "-----------\n";
    fout.close();
}

void Proccessing::createJson()
{
    std::string printable = "";
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        printable += it->printable;


    std::string str = "{\n";
    for (auto& i : pt.repeats)
    {
        std::string key = "";
        std::string power = "";
        std::string count = "";
        std::string letters = ""; // удалить повторяющиеся
        std::string words = "";
        std::vector<std::string> combs;

        key = i.first;
        power = std::to_string(i.second.power);
        count = std::to_string(i.second.count);
        for (int j = 0; j < i.second.letters.size(); j++)
            letters += i.second.letters[j].origin;
        for (auto& j : i.second._words)
            words += j;
        for (int j = 0; j < i.second.combs.size(); j++)
        {
            std::string tCombs = "";
            for (int k = 0; k < i.second.combs[j].size(); k++)
                tCombs += i.second.combs[j][k]->origin;
            combs.push_back(tCombs);
        }

        nlohmann::json tmpOutJson = {
            {"printable", printable},
            {"repeat", {
                {"key", key},
                {"power", power},
                {"count", count},
                {"letters", letters},
                {"words", words}
            }}
        };

        str += "\t\"" + key + "\": {\n";
        str += "\t\t\"key\": \"" + key + "\",\n";
        str += "\t\t\"power\": \"" + power + "\",\n";
        str += "\t\t\"count\": \"" + count + "\",\n";
        str += "\t\t\"letters\": \"" + letters + "\",\n";
        str += "\t\t\"words\": \"" + words + "\",\n";
        str += "\t\t\"combs\": [";
        for (int j = 0; j < combs.size(); j++)
        {
            str += "\"" + combs[j] + "\"" + (j + 1 < combs.size() ? ", " : "");
        }
        str += "]\n";
        str += "\t},\n";
        outJson.push_back(tmpOutJson);
    }
    str += "}\n";

    std::ofstream fout;
    fout.open("outJson.json");
    fout << str;
    fout.close();
}