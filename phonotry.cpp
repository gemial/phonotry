#include <fstream>

#include "engine/phonotext.h"
#include "engine/proccessing.h"

int main()
{

#ifdef _WIN32
    system("chcp 65001");
#endif

    std::ifstream fin;
    fin.open("../data/in.txt", std::ios_base::in);
    if (!fin.is_open()){return 0;};
    std::cout << "start\n";

    std::string data;

    while (!fin.eof())
    {
        std::string line;
        std::getline(fin, line);
        data += line + '\n';
    }
    
    fin.close();
    std::cout << "Read End";
    Phonotext pt(data);
    Proccessing proc(pt, "rus", 0., 100.);

    proc.createJson("../data/outJson.json");
    proc.print("../data/out.txt");
    return 0;
}