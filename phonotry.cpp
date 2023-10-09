#include <fstream>

#include "Phonotext.h"
#include "Proccessing.h"

int main()
{
    system("chcp 65001");

    std::ifstream fin;
    fin.open("in.txt");

    std::cout << "start\n";

    std::string data;

    while (!fin.eof())
    {
        std::string line;
        fin >> line;
        data += line + '\n';
    }
    fin.close();

    Phonotext pt(data);
    Proccessing proc(pt, "rus", 0., 100.);

    proc.print();
    return 0;
}