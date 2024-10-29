#define _CRT_SECURE_NO_WARNINGS

#include "diagram.hpp"
#include <iomanip>


int main(int argc, char* argv[])
{
    const int a = 1 == 2;
    TScaner* scaner;
    scaner = new TScaner("input.txt");
    TDiagram* diagram = new TDiagram(scaner);

	diagram->program();

    int type; type_lex lex;
    type=scaner->scaner(lex);
    if (type == TEnd)
        std::cout << "OK" << std::endl;
    else
		std::cout << "ERROR" << std::endl;
}

/*
int main(int argc, char* argv[]) 
{
    TScaner* scaner;
    int type;
    type_lex lex;

    if (argc <= 1) 
        scaner = new TScaner("input.txt");
    else 
        scaner = new TScaner(argv[1]);
    do
    {
        type = scaner->scaner(lex);
        std::cout << std::setw(32) << lex << "   type of:    " << type << std::endl;
    } while (type != TEnd);
    delete scaner;
    return 0;
}
*/