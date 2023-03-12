#include <windows.h>

#include <iostream>

#include "conf.h"
#include "Letter.h"
#include "Phonotext.h"



int main()
{
    system("chcp 65001");

    Conf CONFIG("eng"); // Выбор языка

    Letter l('l');
    std::cout << l.getLetterRepr() << std::endl;
    std::cout << l.getLetter() << std::endl;
    std::cout << std::endl;

    Phonotext p("IoSTReam");
    std::cout << p.countLetters().first << " " << p.countLetters().second << std::endl;
    std::cout << p.getOrigin() << std::endl;
    std::cout << p.getTechnic() << std::endl;
    std::cout << p.getPrintable() << std::endl;
    std::cout << p.getPhonotextRepr() << std::endl;
    std::cout << p.length() << std::endl;

    return 0;
}