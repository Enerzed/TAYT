// file diagram.hpp

#pragma once

#include "scanner.hpp"

class TDiagram
{
private:
	TScaner* scaner;
	void description();					// Описание
	void data();						// Данные
	void function();					// Функция
	void type();						// Тип
	void list();						// Список
	void element();						// Элемент
	void variable();					// Переменная
	void array();						// Массив
	void array_expression();			// Выражение одномерного массива
	void array_ident();					// Идентификатор массива
	void assignment();					// Присваивание
	void expression();					// Выражение
	void composite_operator();			// Составной оператор
	void operators_and_descriptions();	// Операторы и описания
	void operator_();					// Оператор
	void function_call();				// Вызов функции
	void condition();					// Условие
	void comparison();					// Сравнение
	void addendum();					// Слагаемое
	void multiplier();					// Множитель
	void unary_operation();				// Унарная операция
	void elementary_expression();		// Элементарное выражение
	int look_forward(int pointer);
public:
	TDiagram(TScaner* scaner) {
		this->scaner = scaner;
	}
	~TDiagram() {}
	void program();						// Программа
};