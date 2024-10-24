// file diagram.hpp

#pragma once

#include "scanner.hpp"

class TDiagram
{
private:
	TScaner* scaner;
	void description();					// ��������
	void data();						// ������
	void function();					// �������
	void type();						// ���
	void list();						// ������
	void element();						// �������
	void variable();					// ����������
	void array();						// ������
	void array_expression();			// ��������� ����������� �������
	void array_ident();					// ������������� �������
	void assignment();					// ������������
	void expression();					// ���������
	void composite_operator();			// ��������� ��������
	void operators_and_descriptions();	// ��������� � ��������
	void operator_();					// ��������
	void function_call();				// ����� �������
	void condition();					// �������
	void comparison();					// ���������
	void addendum();					// ���������
	void multiplier();					// ���������
	void unary_operation();				// ������� ��������
	void elementary_expression();		// ������������ ���������
	int look_forward(int pointer);
public:
	TDiagram(TScaner* scaner) {
		this->scaner = scaner;
	}
	~TDiagram() {}
	void program();						// ���������
};