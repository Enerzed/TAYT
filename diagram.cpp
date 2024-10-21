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

void TDiagram::program() {
	type_lex lex;
	int type, pointer;
	type = look_forward(1);
	while (type == TVoid || type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar) {
		description();
		type = look_forward(1);
	}
}

void TDiagram::description() {
	type_lex lex;
	int type, pointer;
	type = look_forward(1);
	if (type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar)
		data();
	else function();
}

void TDiagram::data() {
	type_lex lex;
	int type_, pointer;
	type();
	list();
	type_ = scaner->scaner(lex);
	if (type_ != TSemicolon)
		scaner->print_error("Expected ;", lex);
}

void TDiagram::function() {
	type_lex lex;
	int type, pointer;

	type = scaner->scaner(lex);
	if (type != TVoid)
		scaner->print_error("Expected void", lex);

	type = scaner->scaner(lex);
	if (type != TIdent && type != TMain)
		scaner->print_error("Expected identificator", lex);

	type = scaner->scaner(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected (", lex);

	type = scaner->scaner(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected )", lex);

	type = scaner->scaner(lex);
	if (type != TLeftBrace)
		scaner->print_error("Expected {", lex);

	operators_and_descriptions();

	type = scaner->scaner(lex);
	if (type != TRightBrace)
		scaner->print_error("Expected }", lex);
}

void TDiagram::type() {
	type_lex lex;
	int type, pointer;
	type = scaner->scaner(lex);
	if (type != TInt && type != TShort && type != TLong && type != T__Int64 && type != TChar)
		scaner->print_error("Expected type (int, short, long, __int64, char)", lex);
}

void TDiagram::list() {
	type_lex lex;
	int type, pointer;

	element();

	type = look_forward(1);

	while (type == TComma) {
		type = scaner->scaner(lex);
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
}

void TDiagram::variable() {
	type_lex lex;
	int type, pointer;

	pointer = scaner->get_pointer();
	type = scaner->scaner(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator", lex);

	type = look_forward(1);
	if (type == TEval)
	{
		scaner->set_pointer(pointer);
		assignment();
	}
}

void TDiagram::array() {
	type_lex lex;
	int type = scaner->scaner(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator", lex);

	type = scaner->scaner(lex);
	if (type != TLeftSquareBracket) {
		scaner->print_error("Expected [", lex);
	}

	expression(); 

	type = scaner->scaner(lex);
	if (type != TRightSquareBracket) {
		scaner->print_error("Expected ]", lex);
	}

	type = look_forward(1);
	if (type == TEval) {
		type = scaner->scaner(lex);
		type = scaner->scaner(lex);
		if (type != TLeftBrace)
			scaner->print_error("Expected {", lex);
		array_expression();
		type = scaner->scaner(lex);
		if (type != TRightBrace)
			scaner->print_error("Expected }", lex);
	}
}

void TDiagram::array_expression() {
	type_lex lex;
	int type, pointer;
	type = look_forward(1);
	if (type != TRightBrace) {
		expression();
		type = look_forward(1);
		while (type == TComma) {
			type = scaner->scaner(lex);
			expression();
			type = look_forward(1);
		}
	}

}

void TDiagram::assignment() {
	type_lex lex;
	int type;

	type = scaner->scaner(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator", lex);

	type = scaner->scaner(lex);
	if (type != TEval)
		scaner->print_error("Expected =", lex);

	expression();
}

void TDiagram::expression() {
	type_lex lex;
	int type, pointer;
	comparison();
	type = look_forward(1);
	while (type == TEq || type == TNe) {
		type = scaner->scaner(lex);
		comparison();
		type = look_forward(1);
	}
}

void TDiagram::composite_operator() {
	type_lex lex;
	int type, pointer;
	type = scaner->scaner(lex);
	if (type != TLeftBrace)
		scaner->print_error("Expected {", lex);
	operators_and_descriptions();
	if (type != TRightBrace)
		scaner->print_error("Expected }", lex);
}

void TDiagram::operators_and_descriptions() {
	type_lex lex;
	int type, pointer;
	type = look_forward(1);
	while (type != TRightBrace) {
		type = look_forward(1);
		if (type == TInt || type == TShort || type == TLong || type == T__Int64 || type == TChar) {
			data();
		}
		else operator_();
	}
}

void TDiagram::operator_() {
	type_lex lex;
	int type, pointer;
	type = look_forward(1);
	if (type == TSemicolon)
		type = scaner->scaner(lex);
		return;
	if (type == TIf) {
		condition();
		return;
	}
	if (type == TLeftBrace) {
		composite_operator();
		return;
	}

	type = look_forward(1);
	int type2 = look_forward(2);
	if (type == TIdent && type2 == TLeftBracket) {
		function_call();
		return;
	}
	if (type == TIdent && type == TEval) {
		assignment();
		return;
	}
	scaner->print_error("Expected operator", lex);
}

void TDiagram::function_call() {
	type_lex lex;
	int type, pointer;

	type = scaner->scaner(lex);
	if (type != TIdent)
		scaner->print_error("Expected identificator", lex);

	type = scaner->scaner(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected (", lex);

	type = scaner->scaner(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected )", lex);

	type = scaner->scaner(lex);
	if (type != TSemicolon)
		scaner->print_error("Expected ;", lex);
}

void TDiagram::condition() {
	type_lex lex;
	int type, pointer;

	type = scaner->scaner(lex);
	if (type != TIf)
		scaner->print_error("Expected if", lex);

	type = scaner->scaner(lex);
	if (type != TLeftBracket)
		scaner->print_error("Expected (", lex);

	expression();
	type = scaner->scaner(lex);
	if (type != TRightBracket)
		scaner->print_error("Expected )", lex);

	operator_();

	type = look_forward(1);

	if (type == TElse)
	{
		type = scaner->scaner(lex);
		operator_();
	}
}

void TDiagram::comparison() {
	type_lex lex;
	int type, pointer;
	addendum();
	type = look_forward(1);
	while (type == TLt || type == TLe || type == TGt || type == TGe) {
		type = scaner->scaner(lex);
		addendum();
		type = look_forward(1);
	}
}

void TDiagram::addendum() {
	type_lex lex;
	int type, pointer;
	multiplier();
	type = look_forward(1);
	while (type == TAdd || type == TSub) {
		type = scaner->scaner(lex);
		multiplier();
		type = look_forward(1);
	}
}

void TDiagram::multiplier() {
	type_lex lex;
	int type, pointer;
	unary_operation();
	type = look_forward(1);
	while (type == TMul || type == TDiv || type == TMod) {
		type = scaner->scaner(lex);
		unary_operation();
		type = look_forward(1);
	}
}

void TDiagram::unary_operation() {
	type_lex lex;
	int type = look_forward(1);

	if (type == TAdd || type == TSub)
	{
		type = scaner->scaner(lex);
		elementary_expression();
	}
	else
		elementary_expression();
}


void TDiagram::elementary_expression() {
	type_lex lex;
	int type = look_forward(1);
	if (type == TIdent) {
		type = scaner->scaner(lex);
		int type = look_forward(1);
		if (type == TLeftSquareBracket) {
			type = scaner->scaner(lex);
			expression();
			type = scaner->scaner(lex);
			if (type != TRightSquareBracket)
				scaner->print_error("Expected ]", lex);
		}
	}
	else if (type == TConst10 || type == TConst16)
		type = scaner->scaner(lex);
	else if (type == TLeftBracket) {
		type = scaner->scaner(lex);
		expression();
		type = scaner->scaner(lex);
		if (type != TRightBracket)
			scaner->print_error("Expected )", lex);
	}
	else
		scaner->print_error("Expected expression", lex);
}

