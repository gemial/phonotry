#include "Phonotext.h"
#include "Proccessing.h"

int main()
{
    system("chcp 65001");

    std::cout << "start\n";
    std::string data;
    std::getline(std::cin, data);
    std::string line;
    while (std::getline(std::cin, line))
    {
        data += "\n" + line; 
    }
    Phonotext pt(data);

    Proccessing proc(pt, "rus", 0., 100.);
    pt = proc.getResult();
    proc.print();

    return 0;
}
