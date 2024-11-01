// file diagram.hpp

#pragma once

#include "tree.hpp"

class TDiagram
{
private:
	type_data last_type_data;
	type_object last_type_object;
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
	int evaluate_expression();			// ��������� ���������
	int look_forward(int pointer);
	int scan(type_lex lex);
public:
	Tree* root;
	TDiagram(TScaner* scaner) {
		this->scaner = scaner;
		this->root = new Tree(scaner);
		this->root->set_current(this->root);
	}
	~TDiagram() {}
	void program();						// ���������
};