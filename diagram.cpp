// file diagram.cpp

#include "diagram.hpp"

int TDiagram::look_forward(int pos) {
	type_lex lex;
	int saved_pointer = scaner->get_pointer();
	int next_type;
	for (int i = 0; i < pos; i++)
		next_type = scaner->scaner(lex);
	scaner->set_pointer(saved_pointer);
	return next_type;
}

int TDiagram::scan(type_lex lex) {
	return scaner->scaner(lex);
}

void TDiagram::program() {
	type_lex lex;
	int type;
	type = look_forward(1);
	while (type == TVoid || type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar) {
		description();
		type = look_forward(1);
	}
	if (type != TEnd) {
		type = scan(lex);
		scaner->print_error("Expected end got", lex);
	}
}

void TDiagram::description() {
	type_lex lex;
	int type;
	type = look_forward(1);
	if (type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar) {
		data();
		return;
	}
	if (type == TVoid) {
		function();
		return;
	}
	type = scan(lex);
	scaner->print_error("Expected void or type got", lex);
}

void TDiagram::data() {
	type_lex lex;
	int type_;

	type();

	list();

	type_ = scan(lex);
	if (type_ != TSemicolon)
		scaner->print_error("Expected ; got", lex);
}

void TDiagram::function() {
	type_lex lex;
	int type, pointer;

	type = scan(lex);
	if (type != TVoid)
		scaner->print_error("Expected void got", lex);

	type = scan(lex);
	if (type != TIdent && type != TMain)
		scaner->print_error("Expected identificator got", lex);

	type = scan(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected ( got", lex);

	type = scan(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected ) got", lex);

	type = scan(lex);
	if (type != TLeftBrace)
		scaner->print_error("Expected { got", lex);

	operators_and_descriptions();

	type = scan(lex);
	if (type != TRightBrace)
		scaner->print_error("Expected } got", lex);
}

void TDiagram::type() {
	type_lex lex;
	int type;
	type = scan(lex);
	if (type != TInt && type != TShort && type != TLong && type != T__Int64 && type != TChar)
		scaner->print_error("Expected type (int, short, long, __int64, char) got", lex);
}

void TDiagram::list() {
	type_lex lex;
	int type, pointer;

	element();

	type = look_forward(1);

	while (type == TComma) {
		type = scan(lex);
		element();
		type = look_forward(1);
	}
}

void TDiagram::element() {

	type_lex lex;
	int type;
	type = look_forward(1);
	int type2 = look_forward(2);
	if (type == TIdent && type2 != TLeftSquareBracket) {
		variable();
		return;
	}
	if (type == TIdent && type2 == TLeftSquareBracket) {
		array();
		return;
	}
	else {
		type = scaner->scaner(lex);
		scaner->print_error("Expected element got", lex);
	}
}

void TDiagram::variable() {
	type_lex lex;
	int type;

	type = look_forward(1);

	if (type != TIdent) {
		type = scan(lex);
		scaner->print_error("Expected identificator got", lex);
	}

	type = look_forward(2);
	if (type == TEval) {
		assignment();
		return;
	}
	type = scan(lex);
}

void TDiagram::array() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator got", lex);

	type = scan(lex);
	if (type != TLeftSquareBracket) {
		scaner->print_error("Expected [ got", lex);
	}

	expression(); 

	type = scan(lex);
	if (type != TRightSquareBracket) {
		scaner->print_error("Expected ] got", lex);
	}

	type = look_forward(1);
	if (type == TEval) {
		type = scan(lex);
		type = scan(lex);
		if (type != TLeftBrace)
			scaner->print_error("Expected { got", lex);
		array_expression();
		type = scan(lex);
		if (type != TRightBrace)
			scaner->print_error("Expected } got", lex);
	}
}

void TDiagram::array_expression() {
	type_lex lex;
	int type;

	type = look_forward(1);
	if (type != TRightBrace) {
		expression();
		type = look_forward(1);
		while (type == TComma) {
			type = scan(lex);
			expression();
			type = look_forward(1);
		}
	}

}

void TDiagram::array_ident() {

	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TLeftSquareBracket) {
		scaner->print_error("Expected [ got", lex);
	}

	expression();

	type = scan(lex);
	if (type != TRightSquareBracket) {
		scaner->print_error("Expected ] got", lex);
	}
}

void TDiagram::assignment() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TIdent) {
		scaner->print_error("Expected identificator got", lex);
	}

	type = look_forward(1);
	if (type == TLeftSquareBracket)
		array_ident();

	type = scan(lex);
	if (type != TEval)
		scaner->print_error("Expected = got", lex);

	expression();
}

void TDiagram::expression() {
	type_lex lex;
	int type;

	comparison();
	type = look_forward(1);
	while (type == TEq || type == TNe) {
		type = scan(lex);
		comparison();
		type = look_forward(1);
	}
}

void TDiagram::composite_operator() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TLeftBrace)
		scaner->print_error("Expected { got", lex);

	operators_and_descriptions();

	type = scan(lex);
	if (type != TRightBrace)
		scaner->print_error("Expected } got", lex);
}

void TDiagram::operators_and_descriptions() {
	type_lex lex;
	int type;

	type = look_forward(1);
	while (type != TRightBrace) {
		if (type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar) {
			data();
		}
		else operator_();
		type = look_forward(1);

	}
}

void TDiagram::operator_() {
	type_lex lex;
	int type;

	type = look_forward(1);
	if (type == TSemicolon) {
		type = scan(lex);
		return;
	}

	if (type == TIf) {
		condition();
		return;
	}

	if (type == TLeftBrace) {
		composite_operator();
		return;
	}

	int type2 = look_forward(2);
	if (type == TIdent && type2 == TLeftBracket) {
		function_call();
		type = scan(lex);
		if (type != TSemicolon)
			scaner->print_error("Expected ; got", lex);
		return;
	}
	if (type == TIdent && type2 == TEval) {
		assignment();
		type = scan(lex);
		if (type != TSemicolon)
			scaner->print_error("Expected ; got", lex);
		return;
	}
	if (type == TIdent && type2 == TLeftSquareBracket) {
		type = scan(lex);
		array_ident();
		type = look_forward(1);
		if (type == TEval) {
			type = scan(lex);
			expression();
		}
		type = scan(lex);
		if (type != TSemicolon)
			scaner->print_error("Expected ; got", lex);
		return;
	}

	type = scan(lex);
	scaner->print_error("Expected operator got", lex);
}

void TDiagram::function_call() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator got", lex);

	type = scan(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected ( got", lex);

	type = scan(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected ) got", lex);

	type = scan(lex);
	if (type != TSemicolon)
		scaner->print_error("Expected ; got", lex);
}

void TDiagram::condition() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TIf)
		scaner->print_error("Expected if got", lex);

	type = scan(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected ( got", lex);

	expression();

	type = scan(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected ) got", lex);

	operator_();

	type = look_forward(1);
	if (type == TElse)
	{
		type = scan(lex);
		operator_();
	}
}

void TDiagram::comparison() {
	type_lex lex;
	int type;
	addendum();
	type = look_forward(1);
	while (type == TLt || type == TLe || type == TGt || type == TGe) {
		type = scan(lex);
		addendum();
		type = look_forward(1);
	}
}

void TDiagram::addendum() {
	type_lex lex;
	int type;

	multiplier();

	type = look_forward(1);
	while (type == TAdd || type == TSub) {
		type = scan(lex);
		multiplier();
		type = look_forward(1);
	}
}

void TDiagram::multiplier() {
	type_lex lex;
	int type;

	unary_operation();

	type = look_forward(1);
	while (type == TMul || type == TDiv || type == TMod) {
		type = scan(lex);
		unary_operation();
		type = look_forward(1);
	}
}

void TDiagram::unary_operation() {
	type_lex lex;
	int type = look_forward(1);

	if (type == TAdd || type == TSub)
	{
		type = scan(lex);
		elementary_expression();
	}
	else
		elementary_expression();
}


void TDiagram::elementary_expression() {
	type_lex lex;
	int type = look_forward(1);
	if (type == TIdent) {
		type = scan(lex);
		int type = look_forward(1);
		if (type == TLeftSquareBracket) {
			array_ident();
			return;
		}
		return;
	}
	else if (type == TConst10 || type == TConst16) {
		type = scan(lex);
		return;
	}
	else if (type == TLeftBracket) {
		type = scan(lex);
		expression();
		type = scan(lex);
		if (type != TRightBracket)
			scaner->print_error("Expected ) got ", lex);
		return;
	}
	else {
		type = scan(lex);
		scaner->print_error("Expected expression got", lex);
	}
}

