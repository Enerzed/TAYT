#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifndef __SCANER
#define __SCANER
#include <iostream>
#include "defs.hpp"


class TScaner
{
private:
	type_mod text;
	int pointer;

public:
	void set_pointer(int);
	int get_pointer();
	void print_error(const char*,const char*);
	int scaner(type_lex);
	void get_data(const char*);
	TScaner(const char*);
	~TScaner() {};
};

#endif