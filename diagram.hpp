// file diagram.hpp

#pragma once

#include "tree.hpp"

class TDiagram
{
private:
	type_data last_type_data;
	type_object last_type_object;
	TScaner* scaner;
	void description();					// Описание
	void data();						// Данные
	void function();					// Функция
	void type();						// Тип
	void list();						// Список
	void element();						// Элемент
	void variable();					// Переменная
	void array();						// Массив
	void array_expression(Tree*);		// Выражение одномерного массива
	void array_ident(TData*);			// Идентификатор массива
	void assignment();					// Присваивание
	void expression(TData*);			// Выражение
	void composite_operator();			// Составной оператор
	void operators_and_descriptions();	// Операторы и описания
	void operator_();					// Оператор
	void function_call();				// Вызов функции
	void condition();					// Условие
	void comparison(TData*);			// Сравнение
	void addendum(TData*);				// Слагаемое
	void multiplier(TData*);			// Множитель
	void unary_operation(TData*);		// Унарная операция
	void elementary_expression(TData*);	// Элементарное выражение
	int look_forward(int pointer);
	int scan(type_lex lex);
	void print_variable(Tree*);
	void print_array(Tree*, int);
public:
	Tree* root;
	TDiagram(TScaner* scaner) {
		this->scaner = scaner;
		this->root = new Tree(scaner);
		this->root->set_current(this->root);
	}
	~TDiagram() {}
	void program();						// Программа
};