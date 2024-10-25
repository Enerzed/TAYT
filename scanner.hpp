// file scanner.hpp
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifndef __SCANER
#define __SCANER
#include <iostream>
#include <vector>
#include <algorithm>
#include "defs.hpp"


class TScaner
{
private:
	type_mod text;
	int pointer;
	int line_counter = 1;
	int position_in_line = 0;
	std::vector<int> break_line_positions;
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