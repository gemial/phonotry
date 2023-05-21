#include "Phonotext.h"
#include "Proccessing.h"

int main()
{
    system("chcp 65001");

    std::cout << "start\n";
    Phonotext pt("всегодня пришёл юнгатслтс");

    Proccessing proc(pt, "rus", 0, 100);
    pt = proc.getResult();
    proc.print();

    return 0;
}
