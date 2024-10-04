#define _CRT_SECURE_NO_WARNINGS

#include "scanner.hpp"
#include <iomanip>

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