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

	Tree* t = root->semantic_include(lex, OBJECT_FUNCTION, TYPE_VOID);

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

	root->set_current(t);
}

void TDiagram::type() {
	type_lex lex;
	int type;
	type = scan(lex);
	if (type != TInt && type != TShort && type != TLong && type != T__Int64 && type != TChar)
		scaner->print_error("Expected type (int, short, long, __int64, char) got", lex);
	// set last type for semantic analyzer
	if (type == TInt)
		last_type_data = TYPE_INT;
	else if (type == TShort)
		last_type_data = TYPE_SHORT;
	else if (type == TLong)
		last_type_data = TYPE_LONG;
	else if (type == T__Int64)
		last_type_data = TYPE__INT64;
	else if (type == TChar)
		last_type_data = TYPE_CHAR;
	else
		last_type_data = TYPE_UNKNOWN;
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

	int pointer = scaner->get_pointer();
	type = scan(lex);
	Tree* t = root->semantic_include(lex, OBJECT_VARIABLE, last_type_data);
	scaner->set_pointer(pointer);

	type = look_forward(2);
	if (type == TEval) {
		assignment();
		root->semantic_set_init(t, 1);
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

	Tree* t = root->semantic_include(lex, OBJECT_ARRAY, last_type_data);
	TData* data = &root->get_current_node()->data;

	type = scan(lex);
	if (type != TLeftSquareBracket) {
		scaner->print_error("Expected [ got", lex);
	}

	TData* expression_data = new TData();
	expression(expression_data);

	switch (data->type) {
	case TYPE_INT:
		switch (expression_data->type) {
		case TYPE_INT: if (expression_data->value.data_as_int > 0) data->value.array_as_int = new int[expression_data->value.data_as_int]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_int; break;
		case TYPE_SHORT: if (expression_data->value.data_as_short > 0) data->value.array_as_int = new int[expression_data->value.data_as_short]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_short; break;
		case TYPE_LONG: if (expression_data->value.data_as_long > 0) data->value.array_as_int = new int[expression_data->value.data_as_long]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_long; break;
		case TYPE__INT64: if (expression_data->value.data_as__int64 > 0) data->value.array_as_int = new int[expression_data->value.data_as__int64]; else scaner->print_error("Array size must be greater than 0", lex); 
			t->get_node()->array_size = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: if (expression_data->value.data_as_char > 0) data->value.array_as_int = new int[expression_data->value.data_as_char]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_SHORT:
		switch (expression_data->type) {
		case TYPE_INT: if (expression_data->value.data_as_int > 0) data->value.array_as_short = new short[expression_data->value.data_as_int]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_int; break;
		case TYPE_SHORT: if (expression_data->value.data_as_short > 0) data->value.array_as_short = new short[expression_data->value.data_as_short]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_short; break;
		case TYPE_LONG: if (expression_data->value.data_as_long > 0) data->value.array_as_short = new short[expression_data->value.data_as_long]; else scaner->print_error("Array size must be greater than 0", lex); 
			t->get_node()->array_size = expression_data->value.data_as_long; break;
		case TYPE__INT64: if (expression_data->value.data_as__int64 > 0) data->value.array_as_short = new short[expression_data->value.data_as__int64]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: if (expression_data->value.data_as_char > 0) data->value.array_as_short = new short[expression_data->value.data_as_char]; else scaner->print_error("Array size must be greater than 0", lex); 
			t->get_node()->array_size = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_LONG:
		switch (expression_data->type) {
		case TYPE_INT: if (expression_data->value.data_as_int > 0) data->value.array_as_long = new long[expression_data->value.data_as_int]; else scaner->print_error("Array size must be greater than 0", lex); 
			t->get_node()->array_size = expression_data->value.data_as_int; break;
		case TYPE_SHORT: if (expression_data->value.data_as_short > 0) data->value.array_as_long = new long[expression_data->value.data_as_short]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_short; break;
		case TYPE_LONG: if (expression_data->value.data_as_long > 0) data->value.array_as_long = new long[expression_data->value.data_as_long]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_long; break;
		case TYPE__INT64: if (expression_data->value.data_as__int64 > 0) data->value.array_as_long = new long[expression_data->value.data_as__int64]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: if (expression_data->value.data_as_char > 0) data->value.array_as_long = new long[expression_data->value.data_as_char]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE__INT64:
		switch (expression_data->type) {
		case TYPE_INT: if (expression_data->value.data_as_int > 0) data->value.array_as__int64 = new __int64[expression_data->value.data_as_int]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_int; break;
		case TYPE_SHORT: if (expression_data->value.data_as_short > 0) data->value.array_as__int64 = new __int64[expression_data->value.data_as_short]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_short; break;
		case TYPE_LONG: if (expression_data->value.data_as_long > 0) data->value.array_as__int64 = new __int64[expression_data->value.data_as_long]; else scaner->print_error("Array size must be greater than 0", lex); 
			t->get_node()->array_size = expression_data->value.data_as_long; break;
		case TYPE__INT64: if (expression_data->value.data_as__int64 > 0) data->value.array_as__int64 = new __int64[expression_data->value.data_as__int64]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: if (expression_data->value.data_as_char > 0) data->value.array_as__int64 = new __int64[expression_data->value.data_as_char]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_CHAR:
		switch (expression_data->type) {
		case TYPE_INT: if (expression_data->value.data_as_int > 0) data->value.array_as_char = new char[expression_data->value.data_as_int]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_int; break;
		case TYPE_SHORT: if (expression_data->value.data_as_short > 0) data->value.array_as_char = new char[expression_data->value.data_as_short]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_short; break;
		case TYPE_LONG: if (expression_data->value.data_as_long > 0) data->value.array_as_char = new char[expression_data->value.data_as_long]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_long; break;
		case TYPE__INT64: if (expression_data->value.data_as__int64 > 0) data->value.array_as_char = new char[expression_data->value.data_as__int64]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: if (expression_data->value.data_as_char > 0) data->value.array_as_char = new char[expression_data->value.data_as_char]; else scaner->print_error("Array size must be greater than 0", lex);
			t->get_node()->array_size = expression_data->value.data_as_char; break;
		}
		break;
	}

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
		array_expression(t);
		type = scan(lex);
		if (type != TRightBrace)
			scaner->print_error("Expected } got", lex);
		root->semantic_set_init(t, 1);
	}
}

void TDiagram::array_expression(Tree* t) {
	type_lex lex;
	int type;

	for (int i = 0; i < t->get_node()->array_size; i++) {
		if (t->get_node()->data.type == TYPE_INT)
			t->get_node()->data.value.array_as_int[i] = 0;
		if (t->get_node()->data.type == TYPE_SHORT)
			t->get_node()->data.value.array_as_short[i] = 0;
		if (t->get_node()->data.type == TYPE_LONG)
			t->get_node()->data.value.array_as_long[i] = 0;
		if (t->get_node()->data.type == TYPE__INT64)
			t->get_node()->data.value.array_as__int64[i] = 0;
		if (t->get_node()->data.type == TYPE_CHAR)
			t->get_node()->data.value.array_as_char[i] = 0;
	}

	type = look_forward(1);
	if (type != TRightBrace) {
		TData* expression_data = new TData();
		expression(expression_data);
		
		switch (t->get_node()->data.type) {
		case TYPE_INT:
			switch (expression_data->type) {
			case TYPE_INT: t->get_node()->data.value.array_as_int[0] = expression_data->value.data_as_int; break;
			case TYPE_SHORT: t->get_node()->data.value.array_as_int[0] = expression_data->value.data_as_short; break;
			case TYPE_LONG: t->get_node()->data.value.array_as_int[0] = expression_data->value.data_as_long; break;
			case TYPE__INT64: t->get_node()->data.value.array_as_int[0] = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: t->get_node()->data.value.array_as_int[0] = expression_data->value.data_as_char; break;
			}
			break;
		case TYPE_SHORT:
			switch (expression_data->type) {
			case TYPE_INT: t->get_node()->data.value.array_as_short[0] = expression_data->value.data_as_int; break;
			case TYPE_SHORT: t->get_node()->data.value.array_as_short[0] = expression_data->value.data_as_short; break;
			case TYPE_LONG: t->get_node()->data.value.array_as_short[0] = expression_data->value.data_as_long; break;
			case TYPE__INT64: t->get_node()->data.value.array_as_short[0] = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: t->get_node()->data.value.array_as_short[0] = expression_data->value.data_as_char; break;
			}
			break;
		case TYPE_LONG:
			switch (expression_data->type) {
			case TYPE_INT: t->get_node()->data.value.array_as_long[0] = expression_data->value.data_as_int; break;
			case TYPE_SHORT: t->get_node()->data.value.array_as_long[0] = expression_data->value.data_as_short; break;
			case TYPE_LONG: t->get_node()->data.value.array_as_long[0] = expression_data->value.data_as_long; break;
			case TYPE__INT64: t->get_node()->data.value.array_as_long[0] = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: t->get_node()->data.value.array_as_long[0] = expression_data->value.data_as_char; break;
			}
			break;
		case TYPE__INT64:
			switch (expression_data->type) {
			case TYPE_INT: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as_int; break;
			case TYPE_SHORT: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as_short; break;
			case TYPE_LONG: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as_long; break;
			case TYPE__INT64: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as_char; break;
			}
			break;
		case TYPE_CHAR:
			switch (expression_data->type) {
			case TYPE_INT: t->get_node()->data.value.array_as_char[0] = expression_data->value.data_as_int; break;
			case TYPE_SHORT: t->get_node()->data.value.array_as_char[0] = expression_data->value.data_as_short; break;
			case TYPE_LONG: t->get_node()->data.value.array_as_char[0] = expression_data->value.data_as_long; break;
			case TYPE__INT64: t->get_node()->data.value.array_as_char[0] = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: t->get_node()->data.value.array_as_char[0] = expression_data->value.data_as_char; break;
			}
			break;
		}
		int counter = 1;
		type = look_forward(1);
		while (type == TComma) {
			type = scan(lex);
			expression(expression_data);
			switch (t->get_node()->data.type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: t->get_node()->data.value.array_as_int[counter] = expression_data->value.data_as_int; break;
				case TYPE_SHORT: t->get_node()->data.value.array_as_int[counter] = expression_data->value.data_as_short; break;
				case TYPE_LONG: t->get_node()->data.value.array_as_int[counter] = expression_data->value.data_as_long; break;
				case TYPE__INT64: t->get_node()->data.value.array_as_int[counter] = expression_data->value.data_as__int64; break;
				case TYPE_CHAR: t->get_node()->data.value.array_as_int[counter] = expression_data->value.data_as_char; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: t->get_node()->data.value.array_as_short[counter] = expression_data->value.data_as_int; break;
				case TYPE_SHORT: t->get_node()->data.value.array_as_short[counter] = expression_data->value.data_as_short; break;
				case TYPE_LONG: t->get_node()->data.value.array_as_short[counter] = expression_data->value.data_as_long; break;
				case TYPE__INT64: t->get_node()->data.value.array_as_short[counter] = expression_data->value.data_as__int64; break;
				case TYPE_CHAR: t->get_node()->data.value.array_as_short[counter] = expression_data->value.data_as_char; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: t->get_node()->data.value.array_as_long[counter] = expression_data->value.data_as_int; break;
				case TYPE_SHORT: t->get_node()->data.value.array_as_long[counter] = expression_data->value.data_as_short; break;
				case TYPE_LONG: t->get_node()->data.value.array_as_long[counter] = expression_data->value.data_as_long; break;
				case TYPE__INT64: t->get_node()->data.value.array_as_long[counter] = expression_data->value.data_as__int64; break;
				case TYPE_CHAR: t->get_node()->data.value.array_as_long[counter] = expression_data->value.data_as_char; break;
				}
				break;
			case TYPE__INT64:
				switch (expression_data->type) {
				case TYPE_INT: t->get_node()->data.value.array_as__int64[counter] = expression_data->value.data_as_int; break;
				case TYPE_SHORT: t->get_node()->data.value.array_as__int64[counter] = expression_data->value.data_as_short; break;
				case TYPE_LONG: t->get_node()->data.value.array_as__int64[counter] = expression_data->value.data_as_long; break;
				case TYPE__INT64: t->get_node()->data.value.array_as__int64[counter] = expression_data->value.data_as__int64; break;
				case TYPE_CHAR: t->get_node()->data.value.array_as__int64[0] = expression_data->value.data_as_char; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: t->get_node()->data.value.array_as_char[counter] = expression_data->value.data_as_int; break;
				case TYPE_SHORT: t->get_node()->data.value.array_as_char[counter] = expression_data->value.data_as_short; break;
				case TYPE_LONG: t->get_node()->data.value.array_as_char[counter] = expression_data->value.data_as_long; break;
				case TYPE__INT64: t->get_node()->data.value.array_as_char[counter] = expression_data->value.data_as__int64; break;
				case TYPE_CHAR: t->get_node()->data.value.array_as_char[counter] = expression_data->value.data_as_char; break;
				}
				break;
			}
			counter++;
			type = look_forward(1);
		}
	}

}

void TDiagram::array_ident(TData* data) {

	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TLeftSquareBracket) {
		scaner->print_error("Expected [ got", lex);
	}

	expression(data);

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

	Tree* t = root->semantic_get_type(lex, OBJECT_VARIABLE);
	TData* data = &root->get_current_node()->data;
	root->semantic_set_init(t, 1);

	//type = look_forward(1);
	//if (type == TLeftSquareBracket)
	//	array_ident();

	type = scan(lex);
	if (type != TEval)
		scaner->print_error("Expected = got", lex);

	TData* expression_data = new TData;
	expression(expression_data);

	switch (data->type) {
	case TYPE_INT:
		switch (expression_data->type) {
		case TYPE_INT: data->value.data_as_int = expression_data->value.data_as_int; break;
		case TYPE_SHORT: data->value.data_as_int = expression_data->value.data_as_short; break;
		case TYPE_LONG: data->value.data_as_int = expression_data->value.data_as_long; break;
		case TYPE__INT64: data->value.data_as_int = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: data->value.data_as_int = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_SHORT:
		switch (expression_data->type) {
		case TYPE_INT: data->value.data_as_short = expression_data->value.data_as_int; break;
		case TYPE_SHORT: data->value.data_as_short = expression_data->value.data_as_short; break;
		case TYPE_LONG: data->value.data_as_short = expression_data->value.data_as_long; break;
		case TYPE__INT64: data->value.data_as_short = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: data->value.data_as_short = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_LONG:
		switch (expression_data->type) {
		case TYPE_INT: data->value.data_as_long = expression_data->value.data_as_int; break;
		case TYPE_SHORT: data->value.data_as_long = expression_data->value.data_as_short; break;
		case TYPE_LONG: data->value.data_as_long = expression_data->value.data_as_long; break;
		case TYPE__INT64: data->value.data_as_long = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: data->value.data_as_long = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE__INT64:
		switch (expression_data->type) {
		case TYPE_INT: data->value.data_as__int64 = expression_data->value.data_as_int; break;
		case TYPE_SHORT: data->value.data_as__int64 = expression_data->value.data_as_short; break;
		case TYPE_LONG: data->value.data_as__int64 = expression_data->value.data_as_long; break;
		case TYPE__INT64: data->value.data_as__int64 = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: data->value.data_as__int64 = expression_data->value.data_as_char; break;
		}
		break;
	case TYPE_CHAR:
		switch (expression_data->type) {
		case TYPE_INT: data->value.data_as_char = expression_data->value.data_as_int; break;
		case TYPE_SHORT: data->value.data_as_char = expression_data->value.data_as_short; break;
		case TYPE_LONG: data->value.data_as_char = expression_data->value.data_as_long; break;
		case TYPE__INT64: data->value.data_as_char = expression_data->value.data_as__int64; break;
		case TYPE_CHAR: data->value.data_as_char = expression_data->value.data_as_char; break;
		}
		break;
	}
	print_variable(t);
}



void TDiagram::composite_operator() {
	type_lex lex;
	int type;

	type = scan(lex);
	if (type != TLeftBrace)
		scaner->print_error("Expected { got", lex);

	Tree* t = root->semantic_include(lex, OBJECT_UNKNOWN, TYPE_UNKNOWN);

	operators_and_descriptions();

	type = scan(lex);
	if (type != TRightBrace)
		scaner->print_error("Expected } got", lex);

	root->set_current(t);
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

		Tree* t = root->semantic_get_type(lex, OBJECT_ARRAY);
		TData* data = &root->get_current_node()->data;
		TData* expression_data = new TData;
		TData* assignment_data = new TData;
		array_ident(expression_data);

		type = scan(lex);
		if (type != TEval)
			scaner->print_error("Expected = got", lex);

		expression(assignment_data);

		switch (expression_data->type) {
		case TYPE_INT:
			if (expression_data->value.data_as_int >= t->get_node()->array_size || expression_data->value.data_as_int < 0)
				scaner->print_error("Array index out of range", "");
			break;
		case TYPE_SHORT:
			if (expression_data->value.data_as_short >= t->get_node()->array_size || expression_data->value.data_as_short < 0)
				scaner->print_error("Array index out of range", "");
			break;
		case TYPE_LONG:
			if (expression_data->value.data_as_long >= t->get_node()->array_size || expression_data->value.data_as_long < 0)
				scaner->print_error("Array index out of range", "");
			break;
		case TYPE__INT64:
			if (expression_data->value.data_as__int64 >= t->get_node()->array_size || expression_data->value.data_as__int64 < 0)
				scaner->print_error("Array index out of range", "");
			break;
		case TYPE_CHAR:
			if (expression_data->value.data_as_char >= t->get_node()->array_size || expression_data->value.data_as_char < 0)
				scaner->print_error("Array index out of range", "");
			break;
		}

		switch (data->type) {
		case TYPE_INT:
			switch (assignment_data->type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_int[expression_data->value.data_as_int] = assignment_data->value.data_as_int; break;
				case TYPE_SHORT: data->value.array_as_int[expression_data->value.data_as_short] = assignment_data->value.data_as_int; break;
				case TYPE_LONG:	data->value.array_as_int[expression_data->value.data_as_long] = assignment_data->value.data_as_int; scaner->print_warning("Warning: long to int conversion", ""); //"break;
				case TYPE__INT64: data->value.array_as_int[expression_data->value.data_as__int64] = assignment_data->value.data_as_int; scaner->print_warning("Warning: __int64 to int conversion", ""); //"break;
				case TYPE_CHAR: data->value.array_as_int[expression_data->value.data_as_char] = assignment_data->value.data_as_int; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_int[expression_data->value.data_as_int] = assignment_data->value.data_as_short; scaner->print_warning("Warning: int to short conversion", ""); break;
				case TYPE_SHORT: data->value.array_as_int[expression_data->value.data_as_short] = assignment_data->value.data_as_short; break;
				case TYPE_LONG: data->value.array_as_int[expression_data->value.data_as_long] = assignment_data->value.data_as_short; scaner->print_warning("Warning: long to short conversion", ""); break;
				case TYPE__INT64: data->value.array_as_int[expression_data->value.data_as__int64] = assignment_data->value.data_as_short; scaner->print_warning("Warning: __int64 to short conversion", ""); break;
				case TYPE_CHAR: data->value.array_as_int[expression_data->value.data_as_char] = assignment_data->value.data_as_short; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_int[expression_data->value.data_as_int] = assignment_data->value.data_as_long; break;
				case TYPE_SHORT: data->value.array_as_int[expression_data->value.data_as_short] = assignment_data->value.data_as_long; break;
				case TYPE_LONG: data->value.array_as_int[expression_data->value.data_as_long] = assignment_data->value.data_as_long; break;
				case TYPE__INT64: data->value.array_as_int[expression_data->value.data_as__int64] = assignment_data->value.data_as_long; scaner->print_warning("Warning: __int64 to long conversion", ""); break;
				case TYPE_CHAR: data->value.array_as_int[expression_data->value.data_as_char] = assignment_data->value.data_as_long; break;
				}
				break;
			case TYPE__INT64:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_int[expression_data->value.data_as_int] = assignment_data->value.data_as__int64; break;
				case TYPE_SHORT: data->value.array_as_int[expression_data->value.data_as_short] = assignment_data->value.data_as__int64; break;
				case TYPE_LONG: data->value.array_as_int[expression_data->value.data_as_long] = assignment_data->value.data_as__int64; break;
				case TYPE__INT64: data->value.array_as_int[expression_data->value.data_as__int64] = assignment_data->value.data_as__int64; break;
				case TYPE_CHAR: data->value.array_as_int[expression_data->value.data_as_char] = assignment_data->value.data_as__int64; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_int[expression_data->value.data_as_int] = assignment_data->value.data_as_char; scaner->print_warning("Warning: int to char conversion", ""); break;
				case TYPE_SHORT: data->value.array_as_int[expression_data->value.data_as_short] = assignment_data->value.data_as_char; scaner->print_warning("Warning: short to char conversion", ""); break;
				case TYPE_LONG: data->value.array_as_int[expression_data->value.data_as_long] = assignment_data->value.data_as_char; scaner->print_warning("Warning: long to char conversion", ""); break;
				case TYPE__INT64: data->value.array_as_int[expression_data->value.data_as__int64] = assignment_data->value.data_as_char; scaner->print_warning("Warning: __int64 to char conversion", ""); break;
				case TYPE_CHAR: data->value.array_as_int[expression_data->value.data_as_char] = assignment_data->value.data_as_char; break;
				}
				break;
			}
			break;
		case TYPE_SHORT:
			switch (assignment_data->type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_short[expression_data->value.data_as_int] = assignment_data->value.data_as_int; break;
				case TYPE_SHORT: data->value.array_as_short[expression_data->value.data_as_short] = assignment_data->value.data_as_int; break;
				case TYPE_LONG: data->value.array_as_short[expression_data->value.data_as_long] = assignment_data->value.data_as_int; break;
				case TYPE__INT64: data->value.array_as_short[expression_data->value.data_as__int64] = assignment_data->value.data_as_int; break;
				case TYPE_CHAR: data->value.array_as_short[expression_data->value.data_as_char] = assignment_data->value.data_as_int; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_short[expression_data->value.data_as_int] = assignment_data->value.data_as_short; break;
				case TYPE_SHORT: data->value.array_as_short[expression_data->value.data_as_short] = assignment_data->value.data_as_short; break;
				case TYPE_LONG: data->value.array_as_short[expression_data->value.data_as_long] = assignment_data->value.data_as_short; break;
				case TYPE__INT64: data->value.array_as_short[expression_data->value.data_as__int64] = assignment_data->value.data_as_short; break;
				case TYPE_CHAR: data->value.array_as_short[expression_data->value.data_as_char] = assignment_data->value.data_as_short; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_short[expression_data->value.data_as_int] = assignment_data->value.data_as_long; break;
				case TYPE_SHORT: data->value.array_as_short[expression_data->value.data_as_short] = assignment_data->value.data_as_long; break;
				case TYPE_LONG: data->value.array_as_short[expression_data->value.data_as_long] = assignment_data->value.data_as_long; break;
				case TYPE__INT64: data->value.array_as_short[expression_data->value.data_as__int64] = assignment_data->value.data_as_long; break;
				case TYPE_CHAR: data->value.array_as_short[expression_data->value.data_as_char] = assignment_data->value.data_as_long; break;
				}
				break;
			case TYPE__INT64:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_short[expression_data->value.data_as_int] = assignment_data->value.data_as__int64; break;
				case TYPE_SHORT: data->value.array_as_short[expression_data->value.data_as_short] = assignment_data->value.data_as__int64; break;
				case TYPE_LONG: data->value.array_as_short[expression_data->value.data_as_long] = assignment_data->value.data_as__int64; break;
				case TYPE__INT64: data->value.array_as_short[expression_data->value.data_as__int64] = assignment_data->value.data_as__int64; break;
				case TYPE_CHAR: data->value.array_as_short[expression_data->value.data_as_char] = assignment_data->value.data_as__int64; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_short[expression_data->value.data_as_int] = assignment_data->value.data_as_char; break;
				case TYPE_SHORT: data->value.array_as_short[expression_data->value.data_as_short] = assignment_data->value.data_as_char; break;
				case TYPE_LONG: data->value.array_as_short[expression_data->value.data_as_long] = assignment_data->value.data_as_char; break;
				case TYPE__INT64: data->value.array_as_short[expression_data->value.data_as__int64] = assignment_data->value.data_as_char; break;
				case TYPE_CHAR: data->value.array_as_short[expression_data->value.data_as_char] = assignment_data->value.data_as_char; break;
				}
				break;
			}
			break;
		case TYPE_LONG:
			switch (assignment_data->type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_long[expression_data->value.data_as_int] = assignment_data->value.data_as_int; break;
				case TYPE_SHORT: data->value.array_as_long[expression_data->value.data_as_short] = assignment_data->value.data_as_int; break;
				case TYPE_LONG: data->value.array_as_long[expression_data->value.data_as_long] = assignment_data->value.data_as_int; break;
				case TYPE__INT64: data->value.array_as_long[expression_data->value.data_as__int64] = assignment_data->value.data_as_int; break;
				case TYPE_CHAR: data->value.array_as_long[expression_data->value.data_as_char] = assignment_data->value.data_as_int; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_long[expression_data->value.data_as_int] = assignment_data->value.data_as_short; break;
				case TYPE_SHORT: data->value.array_as_long[expression_data->value.data_as_short] = assignment_data->value.data_as_short; break;
				case TYPE_LONG: data->value.array_as_long[expression_data->value.data_as_long] = assignment_data->value.data_as_short; break;
				case TYPE__INT64: data->value.array_as_long[expression_data->value.data_as__int64] = assignment_data->value.data_as_short; break;
				case TYPE_CHAR: data->value.array_as_long[expression_data->value.data_as_char] = assignment_data->value.data_as_short; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_long[expression_data->value.data_as_int] = assignment_data->value.data_as_long; break;
				case TYPE_SHORT: data->value.array_as_long[expression_data->value.data_as_short] = assignment_data->value.data_as_long; break;
				case TYPE_LONG: data->value.array_as_long[expression_data->value.data_as_long] = assignment_data->value.data_as_long; break;
				case TYPE__INT64: data->value.array_as_long[expression_data->value.data_as__int64] = assignment_data->value.data_as_long; break;
				case TYPE_CHAR: data->value.array_as_long[expression_data->value.data_as_char] = assignment_data->value.data_as_long; break;
				}
				break;
			case TYPE__INT64:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_long[expression_data->value.data_as_int] = assignment_data->value.data_as__int64; break;
				case TYPE_SHORT: data->value.array_as_long[expression_data->value.data_as_short] = assignment_data->value.data_as__int64; break;
				case TYPE_LONG: data->value.array_as_long[expression_data->value.data_as_long] = assignment_data->value.data_as__int64; break;
				case TYPE__INT64: data->value.array_as_long[expression_data->value.data_as__int64] = assignment_data->value.data_as__int64; break;
				case TYPE_CHAR: data->value.array_as_long[expression_data->value.data_as_char] = assignment_data->value.data_as__int64; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_long[expression_data->value.data_as_int] = assignment_data->value.data_as_char; break;
				case TYPE_SHORT: data->value.array_as_long[expression_data->value.data_as_short] = assignment_data->value.data_as_char; break;
				case TYPE_LONG: data->value.array_as_long[expression_data->value.data_as_long] = assignment_data->value.data_as_char; break;
				case TYPE__INT64: data->value.array_as_long[expression_data->value.data_as__int64] = assignment_data->value.data_as_char; break;
				case TYPE_CHAR: data->value.array_as_long[expression_data->value.data_as_char] = assignment_data->value.data_as_char; break;
				}
				break;
			}
			break;
		case TYPE__INT64:
			switch (assignment_data->type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as__int64[expression_data->value.data_as_int] = assignment_data->value.data_as_int; break;
				case TYPE_SHORT: data->value.array_as__int64[expression_data->value.data_as_short] = assignment_data->value.data_as_int; break;
				case TYPE_LONG: data->value.array_as__int64[expression_data->value.data_as_long] = assignment_data->value.data_as_int; break;
				case TYPE__INT64: data->value.array_as__int64[expression_data->value.data_as__int64] = assignment_data->value.data_as_int; break;
				case TYPE_CHAR: data->value.array_as__int64[expression_data->value.data_as_char] = assignment_data->value.data_as_int; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as__int64[expression_data->value.data_as_int] = assignment_data->value.data_as_short; break;
				case TYPE_SHORT: data->value.array_as__int64[expression_data->value.data_as_short] = assignment_data->value.data_as_short; break;
				case TYPE_LONG: data->value.array_as__int64[expression_data->value.data_as_long] = assignment_data->value.data_as_short; break;
				case TYPE__INT64: data->value.array_as__int64[expression_data->value.data_as__int64] = assignment_data->value.data_as_short; break;
				case TYPE_CHAR: data->value.array_as__int64[expression_data->value.data_as_char] = assignment_data->value.data_as_short; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as__int64[expression_data->value.data_as_int] = assignment_data->value.data_as_long; break;
				case TYPE_SHORT: data->value.array_as__int64[expression_data->value.data_as_short] = assignment_data->value.data_as_long; break;
				case TYPE_LONG: data->value.array_as__int64[expression_data->value.data_as_long] = assignment_data->value.data_as_long; break;
				case TYPE__INT64: data->value.array_as__int64[expression_data->value.data_as__int64] = assignment_data->value.data_as_long; break;
				case TYPE_CHAR: data->value.array_as__int64[expression_data->value.data_as_char] = assignment_data->value.data_as_long; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as__int64[expression_data->value.data_as_int] = assignment_data->value.data_as_char; break;
				case TYPE_SHORT: data->value.array_as__int64[expression_data->value.data_as_short] = assignment_data->value.data_as_char; break;
				case TYPE_LONG: data->value.array_as__int64[expression_data->value.data_as_long] = assignment_data->value.data_as_char; break;
				case TYPE__INT64: data->value.array_as__int64[expression_data->value.data_as__int64] = assignment_data->value.data_as_char; break;
				case TYPE_CHAR: data->value.array_as__int64[expression_data->value.data_as_char] = assignment_data->value.data_as_char; break;
				}
				break;
			}
			break;
		case TYPE_CHAR:
			switch (assignment_data->type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_char[expression_data->value.data_as_int] = assignment_data->value.data_as_int; break;
				case TYPE_SHORT: data->value.array_as_char[expression_data->value.data_as_short] = assignment_data->value.data_as_int; break;
				case TYPE_LONG: data->value.array_as_char[expression_data->value.data_as_long] = assignment_data->value.data_as_int; break;
				case TYPE__INT64: data->value.array_as_char[expression_data->value.data_as__int64] = assignment_data->value.data_as_int; break;
				case TYPE_CHAR: data->value.array_as_char[expression_data->value.data_as_char] = assignment_data->value.data_as_int; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_char[expression_data->value.data_as_int] = assignment_data->value.data_as_short; break;
				case TYPE_SHORT: data->value.array_as_char[expression_data->value.data_as_short] = assignment_data->value.data_as_short; break;
				case TYPE_LONG: data->value.array_as_char[expression_data->value.data_as_long] = assignment_data->value.data_as_short; break;
				case TYPE__INT64: data->value.array_as_char[expression_data->value.data_as__int64] = assignment_data->value.data_as_short; break;
				case TYPE_CHAR: data->value.array_as_char[expression_data->value.data_as_char] = assignment_data->value.data_as_short; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_char[expression_data->value.data_as_int] = assignment_data->value.data_as_long; break;
				case TYPE_SHORT: data->value.array_as_char[expression_data->value.data_as_short] = assignment_data->value.data_as_long; break;
				case TYPE_LONG: data->value.array_as_char[expression_data->value.data_as_long] = assignment_data->value.data_as_long; break;
				case TYPE__INT64: data->value.array_as_char[expression_data->value.data_as__int64] = assignment_data->value.data_as_long; break;
				case TYPE_CHAR: data->value.array_as_char[expression_data->value.data_as_char] = assignment_data->value.data_as_long; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.array_as_char[expression_data->value.data_as_int] = assignment_data->value.data_as_char; break;
				case TYPE_SHORT: data->value.array_as_char[expression_data->value.data_as_short] = assignment_data->value.data_as_char; break;
				case TYPE_LONG: data->value.array_as_char[expression_data->value.data_as_long] = assignment_data->value.data_as_char; break;
				case TYPE__INT64: data->value.array_as_char[expression_data->value.data_as__int64] = assignment_data->value.data_as_char; break;
				case TYPE_CHAR: data->value.array_as_char[expression_data->value.data_as_char] = assignment_data->value.data_as_char; break;
				}
				break;
			}
			break;
		}

		int index = 0;
		switch (expression_data->type) {
			case TYPE_INT: index = expression_data->value.data_as_int; break;
			case TYPE_SHORT: index = expression_data->value.data_as_short; break;
			case TYPE_LONG: index = expression_data->value.data_as_long; break;
			case TYPE__INT64: index = expression_data->value.data_as__int64; break;
			case TYPE_CHAR: index = expression_data->value.data_as_char; break;
		}
		print_array(t, index);

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

	Tree* t = root->semantic_get_type(lex, OBJECT_FUNCTION);

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

	TData* data = &root->get_current_node()->data;
	expression(data);

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

void TDiagram::expression(TData* data) {
	type_lex lex;
	int type;

	comparison(data);
	type = look_forward(1);
	while (type == TEq || type == TNe) {
		int operation = scan(lex);
		TData* expression_data = new TData;
		comparison(expression_data);
		type = look_forward(1);

		switch (data->type) {
		case TYPE_INT:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TEq)
					data->value.data_as_int == expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_int != expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TEq)
					data->value.data_as_int == expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_int != expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TEq)
					data->value.data_as_int == expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_int != expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TEq)
					data->value.data_as_int == expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_int != expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TEq)
					data->value.data_as_int == expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_int != expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		case TYPE_SHORT:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TEq)
					data->value.data_as_short == expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_short != expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TEq)
					data->value.data_as_short == expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_short != expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TEq)
					data->value.data_as_short == expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_short != expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TEq)
					data->value.data_as_short == expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_short != expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TEq)
					data->value.data_as_short == expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_short != expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		case TYPE_LONG:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TEq)
					data->value.data_as_long == expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_long != expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TEq)
					data->value.data_as_long == expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_long != expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TEq)
					data->value.data_as_long == expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_long != expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TEq)
					data->value.data_as_long == expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_long != expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TEq)
					data->value.data_as_long == expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_long != expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		case TYPE__INT64:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TEq)
					data->value.data_as__int64 == expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as__int64 != expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TEq)
					data->value.data_as__int64 == expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as__int64 != expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TEq)
					data->value.data_as__int64 == expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as__int64 != expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TEq)
					data->value.data_as__int64 == expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as__int64 != expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TEq)
					data->value.data_as__int64 == expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as__int64 != expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		case TYPE_CHAR:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TEq)
					data->value.data_as_char == expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_char != expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TEq)
					data->value.data_as_char == expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_char != expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TEq)
					data->value.data_as_char == expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_char != expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TEq)
					data->value.data_as_char == expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_char != expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TEq)
					data->value.data_as_char == expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TNe)
					data->value.data_as_char != expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		}
	}
}

void TDiagram::comparison(TData* data) {
	type_lex lex;
	int type;
	addendum(data);
	type = look_forward(1);
	while (type == TLt || type == TLe || type == TGt || type == TGe) {
		int operation = scan(lex);
		TData* expression_data = new TData;
		addendum(expression_data);
		type = look_forward(1);

		switch(data->type) {
		case TYPE_INT:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TLt)
					data->value.data_as_int < expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TLe)
					data->value.data_as_int <= expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGt)
					data->value.data_as_int > expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGe)
					data->value.data_as_int >= expression_data->value.data_as_int ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_SHORT:
				if (operation == TLt)
					data->value.data_as_int < expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TLe)
					data->value.data_as_int <= expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGt)
					data->value.data_as_int > expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGe)
					data->value.data_as_int >= expression_data->value.data_as_short ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_LONG:
				if (operation == TLt)
					data->value.data_as_int < expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TLe)
					data->value.data_as_int <= expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGt)
					data->value.data_as_int > expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGe)
					data->value.data_as_int >= expression_data->value.data_as_long ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE__INT64:
				if (operation == TLt)
					data->value.data_as_int < expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TLe)
					data->value.data_as_int <= expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGt)
					data->value.data_as_int > expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGe)
					data->value.data_as_int >= expression_data->value.data_as__int64 ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			case TYPE_CHAR:
				if (operation == TLt)
					data->value.data_as_int < expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TLe)
					data->value.data_as_int <= expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGt)
					data->value.data_as_int > expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				else if (operation == TGe)
					data->value.data_as_int >= expression_data->value.data_as_char ? data->value.data_as_int = 1 : data->value.data_as_int = 0;
				break;
			}
			break;
		case TYPE_SHORT:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TLt)
					data->value.data_as_short < expression_data->value.data_as_int ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TLe)
					data->value.data_as_short <= expression_data->value.data_as_int ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGt)
					data->value.data_as_short > expression_data->value.data_as_int ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGe)
					data->value.data_as_short >= expression_data->value.data_as_int ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				break;
			case TYPE_SHORT:
				if (operation == TLt)
					data->value.data_as_short < expression_data->value.data_as_short ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TLe)
					data->value.data_as_short <= expression_data->value.data_as_short ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGt)
					data->value.data_as_short > expression_data->value.data_as_short ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGe)
					data->value.data_as_short >= expression_data->value.data_as_short ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				break;
			case TYPE_LONG:
				if (operation == TLt)
					data->value.data_as_short < expression_data->value.data_as_long ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TLe)
					data->value.data_as_short <= expression_data->value.data_as_long ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGt)
					data->value.data_as_short > expression_data->value.data_as_long ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGe)
					data->value.data_as_short >= expression_data->value.data_as_long ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				break;
			case TYPE__INT64:
				if (operation == TLt)
					data->value.data_as_short < expression_data->value.data_as__int64 ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TLe)
					data->value.data_as_short <= expression_data->value.data_as__int64 ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGt)
					data->value.data_as_short > expression_data->value.data_as__int64 ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGe)
					data->value.data_as_short >= expression_data->value.data_as__int64 ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				break;
			case TYPE_CHAR:
				if (operation == TLt)
					data->value.data_as_short < expression_data->value.data_as_char ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TLe)
					data->value.data_as_short <= expression_data->value.data_as_char ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGt)
					data->value.data_as_short > expression_data->value.data_as_char ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				else if (operation == TGe)
					data->value.data_as_short >= expression_data->value.data_as_char ? data->value.data_as_short = 1 : data->value.data_as_short = 0;
				break;
			}
			break;
		case TYPE_LONG:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TLt)
					data->value.data_as_long < expression_data->value.data_as_int ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TLe)
					data->value.data_as_long <= expression_data->value.data_as_int ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGt)
					data->value.data_as_long > expression_data->value.data_as_int ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGe)
					data->value.data_as_long >= expression_data->value.data_as_int ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				break;
			case TYPE_SHORT:
				if (operation == TLt)
					data->value.data_as_long < expression_data->value.data_as_short ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TLe)
					data->value.data_as_long <= expression_data->value.data_as_short ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGt)
					data->value.data_as_long > expression_data->value.data_as_short ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGe)
					data->value.data_as_long >= expression_data->value.data_as_short ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				break;
			case TYPE_LONG:
				if (operation == TLt)
					data->value.data_as_long < expression_data->value.data_as_long ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TLe)
					data->value.data_as_long <= expression_data->value.data_as_long ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGt)
					data->value.data_as_long > expression_data->value.data_as_long ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGe)
					data->value.data_as_long >= expression_data->value.data_as_long ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				break;
			case TYPE__INT64:
				if (operation == TLt)
					data->value.data_as_long < expression_data->value.data_as__int64 ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TLe)
					data->value.data_as_long <= expression_data->value.data_as__int64 ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGt)
					data->value.data_as_long > expression_data->value.data_as__int64 ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGe)
					data->value.data_as_long >= expression_data->value.data_as__int64 ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				break;
			case TYPE_CHAR:
				if (operation == TLt)
					data->value.data_as_long < expression_data->value.data_as_char ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TLe)
					data->value.data_as_long <= expression_data->value.data_as_char ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGt)
					data->value.data_as_long > expression_data->value.data_as_char ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				else if (operation == TGe)
					data->value.data_as_long >= expression_data->value.data_as_char ? data->value.data_as_long = 1 : data->value.data_as_long = 0;
				break;
			}
			break;
		case TYPE__INT64:
			data->type = TYPE_CHAR;
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TLt)
					data->value.data_as__int64 < expression_data->value.data_as_int ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TLe)
					data->value.data_as__int64 <= expression_data->value.data_as_int ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGt)
					data->value.data_as__int64 > expression_data->value.data_as_int ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGe)
					data->value.data_as__int64 >= expression_data->value.data_as_int ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				break;
			case TYPE_SHORT:
				if (operation == TLt)
					data->value.data_as__int64 < expression_data->value.data_as_short ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TLe)
					data->value.data_as__int64 <= expression_data->value.data_as_short ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGt)
					data->value.data_as__int64 > expression_data->value.data_as_short ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGe)
					data->value.data_as__int64 >= expression_data->value.data_as_short ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				break;
			case TYPE_LONG:
				if (operation == TLt)
					data->value.data_as__int64 < expression_data->value.data_as_long ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TLe)
					data->value.data_as__int64 <= expression_data->value.data_as_long ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGt)
					data->value.data_as__int64 > expression_data->value.data_as_long ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGe)
					data->value.data_as__int64 >= expression_data->value.data_as_long ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				break;
			case TYPE__INT64:
				if (operation == TLt)
					data->value.data_as__int64 < expression_data->value.data_as__int64 ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TLe)
					data->value.data_as__int64 <= expression_data->value.data_as__int64 ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGt)
					data->value.data_as__int64 > expression_data->value.data_as__int64 ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGe)
					data->value.data_as__int64 >= expression_data->value.data_as__int64 ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				break;
			case TYPE_CHAR:
				if (operation == TLt)
					data->value.data_as__int64 < expression_data->value.data_as_char ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TLe)
					data->value.data_as__int64 <= expression_data->value.data_as_char ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGt)
					data->value.data_as__int64 > expression_data->value.data_as_char ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				else if (operation == TGe)
					data->value.data_as__int64 >= expression_data->value.data_as_char ? data->value.data_as__int64 = 1 : data->value.data_as__int64 = 0;
				break;
			}
			break;
		case TYPE_CHAR:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TLt)
					data->value.data_as_char < expression_data->value.data_as_int ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TLe)
					data->value.data_as_char <= expression_data->value.data_as_int ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGt)
					data->value.data_as_char > expression_data->value.data_as_int ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGe)
					data->value.data_as_char >= expression_data->value.data_as_int ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				break;
			case TYPE_SHORT:
				if (operation == TLt)
					data->value.data_as_char < expression_data->value.data_as_short ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TLe)
					data->value.data_as_char <= expression_data->value.data_as_short ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGt)
					data->value.data_as_char > expression_data->value.data_as_short ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGe)
					data->value.data_as_char >= expression_data->value.data_as_short ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				break;
			case TYPE_LONG:
				if (operation == TLt)
					data->value.data_as_char < expression_data->value.data_as_long ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TLe)
					data->value.data_as_char <= expression_data->value.data_as_long ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGt)
					data->value.data_as_char > expression_data->value.data_as_long ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGe)
					data->value.data_as_char >= expression_data->value.data_as_long ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				break;
			case TYPE__INT64:
				if (operation == TLt)
					data->value.data_as_char < expression_data->value.data_as__int64 ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TLe)
					data->value.data_as_char <= expression_data->value.data_as__int64 ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGt)
					data->value.data_as_char > expression_data->value.data_as__int64 ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				else if (operation == TGe)
					data->value.data_as_char >= expression_data->value.data_as__int64 ? data->value.data_as_char = 1 : data->value.data_as_char = 0;
				break;
			}
			break;
		}
	}
}

void TDiagram::addendum(TData* data) {
	type_lex lex;
	int type;

	multiplier(data);

	type = look_forward(1);
	while (type == TAdd || type == TSub) {
		int operation = scan(lex);
		TData* expression_data = new TData;
		multiplier(expression_data);
		type = look_forward(1);

		switch (data->type) {
		case TYPE_INT:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TAdd)
					data->value.data_as_int += expression_data->value.data_as_int; 
				else if (operation == TSub)
					data->value.data_as_int -= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TAdd)
					data->value.data_as_int += expression_data->value.data_as_short;
				else if (operation == TSub)
					data->value.data_as_int -= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TAdd)
					data->value.data_as_int += expression_data->value.data_as_long;
				else if (operation == TSub)
					data->value.data_as_int -= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TAdd)
					data->value.data_as_int += expression_data->value.data_as__int64;
				else if (operation == TSub)
					data->value.data_as_int -= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TAdd)
					data->value.data_as_int += expression_data->value.data_as_char;
				else if (operation == TSub)
					data->value.data_as_int -= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_SHORT:
			switch (expression_data->type) {
			case TYPE_INT:
				data->type = TYPE_INT;
				if (operation == TAdd)
					data->value.data_as_short += expression_data->value.data_as_int;
				else if (operation == TSub)
					data->value.data_as_short -= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TAdd)
					data->value.data_as_short += expression_data->value.data_as_short;
				else if (operation == TSub)
					data->value.data_as_short -= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TAdd)
					data->value.data_as_short += expression_data->value.data_as_long;
				else if (operation == TSub)
					data->value.data_as_short -= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TAdd)
					data->value.data_as_short += expression_data->value.data_as__int64;
				else if (operation == TSub)
					data->value.data_as_short -= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TAdd)
					data->value.data_as_short += expression_data->value.data_as_char;
				else if (operation == TSub)
					data->value.data_as_short -= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_LONG:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TAdd)
					data->value.data_as_long += expression_data->value.data_as_int;
				else if (operation == TSub)
					data->value.data_as_long -= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TAdd)
					data->value.data_as_long += expression_data->value.data_as_short;
				else if (operation == TSub)
					data->value.data_as_long -= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				if (operation == TAdd)
					data->value.data_as_long += expression_data->value.data_as_long;
				else if (operation == TSub)
					data->value.data_as_long -= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TAdd)
					data->value.data_as_long += expression_data->value.data_as__int64;
				else if (operation == TSub)
					data->value.data_as_long -= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TAdd)
					data->value.data_as_long += expression_data->value.data_as_char;
				else if (operation == TSub)
					data->value.data_as_long -= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE__INT64:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TAdd)
					data->value.data_as__int64 += expression_data->value.data_as_int;
				else if (operation == TSub)
					data->value.data_as__int64 -= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TAdd)
					data->value.data_as__int64 += expression_data->value.data_as_short;
				else if (operation == TSub)
					data->value.data_as__int64 -= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				if (operation == TAdd)
					data->value.data_as__int64 += expression_data->value.data_as_long;
				else if (operation == TSub)
					data->value.data_as__int64 -= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				if (operation == TAdd)
					data->value.data_as__int64 += expression_data->value.data_as__int64;
				else if (operation == TSub)
					data->value.data_as__int64 -= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TAdd)
					data->value.data_as__int64 += expression_data->value.data_as_char;
				else if (operation == TSub)
					data->value.data_as__int64 -= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_CHAR: 
			switch (expression_data->type) {
			case TYPE_INT:
				data->type = TYPE_INT;
				if (operation == TAdd)
					data->value.data_as_char += expression_data->value.data_as_int;
				else if (operation == TSub)
					data->value.data_as_char -= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				data->type = TYPE_SHORT;
				if (operation == TAdd)
					data->value.data_as_char += expression_data->value.data_as_short;
				else if (operation == TSub)
					data->value.data_as_char -= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TAdd)
					data->value.data_as_char += expression_data->value.data_as_long;
				else if (operation == TSub)
					data->value.data_as_char -= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TAdd)
					data->value.data_as_char += expression_data->value.data_as__int64;
				else if (operation == TSub)
					data->value.data_as_char -= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TAdd)
					data->value.data_as_char += expression_data->value.data_as_char;
				else if (operation == TSub)
					data->value.data_as_char -= expression_data->value.data_as_char;
				break;
			}
			break;
		}
	}
}

void TDiagram::multiplier(TData* data) {
	type_lex lex;
	int type;

	unary_operation(data);

	type = look_forward(1);
	while (type == TMul || type == TDiv || type == TMod) {
		int operation = scan(lex);
		TData* expression_data = new TData;
		unary_operation(expression_data);
		if (
			expression_data->value.data_as_int == 0 && (operation == TDiv || operation == TMod) || 
			expression_data->value.data_as_short == 0 && (operation == TDiv || operation == TMod) ||
			expression_data->value.data_as_long == 0 && (operation == TDiv || operation == TMod) ||
			expression_data->value.data_as__int64 == 0 && (operation == TDiv || operation == TMod) ||
			expression_data->value.data_as_char == 0 && (operation == TDiv || operation == TMod))
			scaner->print_error("Division by zero", "");
		type = look_forward(1);

		switch (data->type) {
		case TYPE_INT:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TMul)
					data->value.data_as_int *= expression_data->value.data_as_int;
				else if (operation == TDiv)
					data->value.data_as_int /= expression_data->value.data_as_int;
				else if (operation == TMod)
					data->value.data_as_int %= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TMul)
					data->value.data_as_int *= expression_data->value.data_as_short;
				else if (operation == TDiv)
					data->value.data_as_int /= expression_data->value.data_as_short;
				else if (operation == TMod)
					data->value.data_as_int %= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TMul)
					data->value.data_as_int *= expression_data->value.data_as_long;
				else if (operation == TDiv)
					data->value.data_as_int /= expression_data->value.data_as_long;
				else if (operation == TMod)
					data->value.data_as_int %= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TMul)
					data->value.data_as_int *= expression_data->value.data_as__int64;
				else if (operation == TDiv)
					data->value.data_as_int /= expression_data->value.data_as__int64;
				else if (operation == TMod)
					data->value.data_as_int %= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TMul)
					data->value.data_as_int *= expression_data->value.data_as_char;
				else if (operation == TDiv)
					data->value.data_as_int /= expression_data->value.data_as_char;
				else if (operation == TMod)
					data->value.data_as_int %= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_SHORT:
			switch (expression_data->type) {
			case TYPE_INT:
				data->type = TYPE_INT;
				if (operation == TMul)
					data->value.data_as_short *= expression_data->value.data_as_int;
				else if (operation == TDiv)
					data->value.data_as_short /= expression_data->value.data_as_int;
				else if (operation == TMod)
					data->value.data_as_short %= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TMul)
					data->value.data_as_short *= expression_data->value.data_as_short;
				else if (operation == TDiv)
					data->value.data_as_short /= expression_data->value.data_as_short;
				else if (operation == TMod)
					data->value.data_as_short %= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TMul)
					data->value.data_as_short *= expression_data->value.data_as_long;
				else if (operation == TDiv)
					data->value.data_as_short /= expression_data->value.data_as_long;
				else if (operation == TMod)
					data->value.data_as_short %= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TMul)
					data->value.data_as_short *= expression_data->value.data_as__int64;
				else if (operation == TDiv)
					data->value.data_as_short /= expression_data->value.data_as__int64;
				else if (operation == TMod)
					data->value.data_as_short %= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TMul)
					data->value.data_as_short *= expression_data->value.data_as_char;
				else if (operation == TDiv)
					data->value.data_as_short /= expression_data->value.data_as_char;
				else if (operation == TMod)
					data->value.data_as_short %= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_LONG:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TMul)
					data->value.data_as_long *= expression_data->value.data_as_int;
				else if (operation == TDiv)
					data->value.data_as_long /= expression_data->value.data_as_int;
				else if (operation == TMod)
					data->value.data_as_long %= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TMul)
					data->value.data_as_long *= expression_data->value.data_as_short;
				else if (operation == TDiv)
					data->value.data_as_long /= expression_data->value.data_as_short;
				else if (operation == TMod)
					data->value.data_as_long %= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				if (operation == TMul)
					data->value.data_as_long *= expression_data->value.data_as_long;
				else if (operation == TDiv)
					data->value.data_as_long /= expression_data->value.data_as_long;
				else if (operation == TMod)
					data->value.data_as_long %= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TMul)
					data->value.data_as_long *= expression_data->value.data_as__int64;
				else if (operation == TDiv)
					data->value.data_as_long /= expression_data->value.data_as__int64;
				else if (operation == TMod)
					data->value.data_as_long %= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TMul)
					data->value.data_as_long *= expression_data->value.data_as_char;
				else if (operation == TDiv)
					data->value.data_as_long /= expression_data->value.data_as_char;
				else if (operation == TMod)
					data->value.data_as_long %= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE__INT64:
			switch (expression_data->type) {
			case TYPE_INT:
				if (operation == TMul)
					data->value.data_as__int64 *= expression_data->value.data_as_int;
				else if (operation == TDiv)
					data->value.data_as__int64 /= expression_data->value.data_as_int;
				else if (operation == TMod)
					data->value.data_as__int64 %= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				if (operation == TMul)
					data->value.data_as__int64 *= expression_data->value.data_as_short;
				else if (operation == TDiv)
					data->value.data_as__int64 /= expression_data->value.data_as_short;
				else if (operation == TMod)
					data->value.data_as__int64 %= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				if (operation == TMul)
					data->value.data_as__int64 *= expression_data->value.data_as_long;
				else if (operation == TDiv)
					data->value.data_as__int64 /= expression_data->value.data_as_long;
				else if (operation == TMod)
					data->value.data_as__int64 %= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				if (operation == TMul)
					data->value.data_as__int64 *= expression_data->value.data_as__int64;
				else if (operation == TDiv)
					data->value.data_as__int64 /= expression_data->value.data_as__int64;
				else if (operation == TMod)
					data->value.data_as__int64 %= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TMul)
					data->value.data_as__int64 *= expression_data->value.data_as_char;
				else if (operation == TDiv)
					data->value.data_as__int64 /= expression_data->value.data_as_char;
				else if (operation == TMod)
					data->value.data_as__int64 %= expression_data->value.data_as_char;
				break;
			}
			break;
		case TYPE_CHAR:
			switch (expression_data->type) {
			case TYPE_INT:
				data->type = TYPE_INT;
				if (operation == TMul)
					data->value.data_as_char *= expression_data->value.data_as_int;
				else if (operation == TDiv)
					data->value.data_as_char /= expression_data->value.data_as_int;
				else if (operation == TMod)
					data->value.data_as_char %= expression_data->value.data_as_int;
				break;
			case TYPE_SHORT:
				data->type = TYPE_SHORT;
				if (operation == TMul)
					data->value.data_as_char *= expression_data->value.data_as_short;
				else if (operation == TDiv)
					data->value.data_as_char /= expression_data->value.data_as_short;
				else if (operation == TMod)
					data->value.data_as_char %= expression_data->value.data_as_short;
				break;
			case TYPE_LONG:
				data->type = TYPE_LONG;
				if (operation == TMul)
					data->value.data_as_char *= expression_data->value.data_as_long;
				else if (operation == TDiv)
					data->value.data_as_char /= expression_data->value.data_as_long;
				else if (operation == TMod)
					data->value.data_as_char %= expression_data->value.data_as_long;
				break;
			case TYPE__INT64:
				data->type = TYPE__INT64;
				if (operation == TMul)
					data->value.data_as_char *= expression_data->value.data_as__int64;
				else if (operation == TDiv)
					data->value.data_as_char /= expression_data->value.data_as__int64;
				else if (operation == TMod)
					data->value.data_as_char %= expression_data->value.data_as__int64;
				break;
			case TYPE_CHAR:
				if (operation == TMul)
					data->value.data_as_char *= expression_data->value.data_as_char;
				else if (operation == TDiv)
					data->value.data_as_char /= expression_data->value.data_as_char;
				else if (operation == TMod)
					data->value.data_as_char %= expression_data->value.data_as_char;
				break;
			}
			break;
		}
	}
}

void TDiagram::unary_operation(TData* data) {
	type_lex lex;
	int type = look_forward(1);

	if (type == TAdd)
	{
		type = scan(lex);
		elementary_expression(data);
	}
	else if (type == TSub)
	{
		type = scan(lex);
		elementary_expression(data);
		switch (data->type) {
			case TYPE_INT: data->value.data_as_int *= -1; break;
			case TYPE_SHORT: data->value.data_as_short *= -1; break;
			case TYPE_LONG: data->value.data_as_long *= -1; break;
			case TYPE__INT64: data->value.data_as__int64 *= -1; break;
			case TYPE_CHAR: data->value.data_as_char *= -1; break;
		}
	}
	else
		elementary_expression(data);
}


void TDiagram::elementary_expression(TData* data) {
	type_lex lex;
	int type = look_forward(1);
	if (type == TIdent) {
		type = scan(lex);
		int type = look_forward(1);
		if (type == TLeftSquareBracket) {
			Tree* t = root->semantic_get_type(lex, OBJECT_ARRAY);

			TData* expression_data = new TData;
			array_ident(expression_data);
			
			data->type = t->get_node()->data.type;

			switch (expression_data->type) {
			case TYPE_INT:
				if (expression_data->value.data_as_int >= t->get_node()->array_size || expression_data->value.data_as_int < 0)
					scaner->print_error("Array index out of range", "");
				break;
			case TYPE_SHORT:
				if (expression_data->value.data_as_short >= t->get_node()->array_size || expression_data->value.data_as_short < 0)
					scaner->print_error("Array index out of range", "");
				break;
			case TYPE_LONG:
				if (expression_data->value.data_as_long >= t->get_node()->array_size || expression_data->value.data_as_long < 0)
					scaner->print_error("Array index out of range", "");
				break;
			case TYPE__INT64:
				if (expression_data->value.data_as__int64 >= t->get_node()->array_size || expression_data->value.data_as__int64 < 0)
					scaner->print_error("Array index out of range", "");
				break;
			case TYPE_CHAR:
				if (expression_data->value.data_as_char >= t->get_node()->array_size || expression_data->value.data_as_char < 0)
					scaner->print_error("Array index out of range", "");
				break;
			}

			switch (t->get_node()->data.type) {
			case TYPE_INT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.data_as_int = t->get_node()->data.value.array_as_int[expression_data->value.data_as_int]; break;
				case TYPE_SHORT: data->value.data_as_int = t->get_node()->data.value.array_as_int[expression_data->value.data_as_short]; break;
				case TYPE_LONG: data->value.data_as_int = t->get_node()->data.value.array_as_int[expression_data->value.data_as_long]; break;
				case TYPE__INT64: data->value.data_as_int = t->get_node()->data.value.array_as_int[expression_data->value.data_as__int64]; break;
				case TYPE_CHAR: data->value.data_as_int = t->get_node()->data.value.array_as_int[expression_data->value.data_as_char]; break;
				}
				break;
			case TYPE_SHORT:
				switch (expression_data->type) {
				case TYPE_INT: data->value.data_as_short = t->get_node()->data.value.array_as_short[expression_data->value.data_as_int]; break;
				case TYPE_SHORT: data->value.data_as_short = t->get_node()->data.value.array_as_short[expression_data->value.data_as_short]; break;
				case TYPE_LONG: data->value.data_as_short = t->get_node()->data.value.array_as_short[expression_data->value.data_as_long]; break;
				case TYPE__INT64: data->value.data_as_short = t->get_node()->data.value.array_as_short[expression_data->value.data_as__int64]; break;
				case TYPE_CHAR: data->value.data_as_short = t->get_node()->data.value.array_as_short[expression_data->value.data_as_char]; break;
				}
				break;
			case TYPE_LONG:
				switch (expression_data->type) {
				case TYPE_INT: data->value.data_as_long = t->get_node()->data.value.array_as_long[expression_data->value.data_as_int]; break;
				case TYPE_SHORT: data->value.data_as_long = t->get_node()->data.value.array_as_long[expression_data->value.data_as_short]; break;
				case TYPE_LONG: data->value.data_as_long = t->get_node()->data.value.array_as_long[expression_data->value.data_as_long]; break;
				case TYPE__INT64: data->value.data_as_long = t->get_node()->data.value.array_as_long[expression_data->value.data_as__int64]; break;
				case TYPE_CHAR: data->value.data_as_long = t->get_node()->data.value.array_as_long[expression_data->value.data_as_char]; break;
				}
				break;
			case TYPE__INT64:
				switch (expression_data->type) {
				case TYPE_INT: data->value.data_as__int64 = t->get_node()->data.value.array_as__int64[expression_data->value.data_as_int]; break;
				case TYPE_SHORT: data->value.data_as__int64 = t->get_node()->data.value.array_as__int64[expression_data->value.data_as_short]; break;
				case TYPE_LONG: data->value.data_as__int64 = t->get_node()->data.value.array_as__int64[expression_data->value.data_as_long]; break;
				case TYPE__INT64: data->value.data_as__int64 = t->get_node()->data.value.array_as__int64[expression_data->value.data_as__int64]; break;
				case TYPE_CHAR: data->value.data_as__int64 = t->get_node()->data.value.array_as__int64[expression_data->value.data_as_char]; break;
				}
				break;
			case TYPE_CHAR:
				switch (expression_data->type) {
				case TYPE_INT: data->value.data_as_char = t->get_node()->data.value.array_as_char[expression_data->value.data_as_int]; break;
				case TYPE_SHORT: data->value.data_as_char = t->get_node()->data.value.array_as_char[expression_data->value.data_as_short]; break;
				case TYPE_LONG: data->value.data_as_char = t->get_node()->data.value.array_as_char[expression_data->value.data_as_long]; break;
				case TYPE__INT64: data->value.data_as_char = t->get_node()->data.value.array_as_char[expression_data->value.data_as__int64]; break;
				case TYPE_CHAR: data->value.data_as_char = t->get_node()->data.value.array_as_char[expression_data->value.data_as_char]; break;
				}
				break;
			}

			return;
		}
		Tree* t = root->semantic_get_type(lex, OBJECT_VARIABLE);
		if (t->get_node()->init != 1)
			scaner->print_error("Variable not initialized", lex);
		data->type = t->get_node()->data.type;
		data->value = t->get_node()->data.value;
		return;
	}
	else if (type == TConst10 || type == TConst16) {
		type = scan(lex);
		data->type = TYPE__INT64;
		data->value.data_as__int64 = strtoll(lex, NULL, 0);
		return;
	}
	else if (type == TLeftBracket) {
		type = scan(lex);
		expression(data);
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

void TDiagram::print_variable(Tree* t) {
	switch(t->get_node()->data.type) {
	case TYPE_INT: std::cout << t->get_node()->lex << " = " << t->get_node()->data.value.data_as_int << " type of int" << std::endl; break;
	case TYPE_SHORT: std::cout << t->get_node()->lex << " = " << t->get_node()->data.value.data_as_short << " type of short" << std::endl; break;
	case TYPE_LONG: std::cout << t->get_node()->lex << " = " << t->get_node()->data.value.data_as_long << " type of long" << std::endl; break;
	case TYPE__INT64: std::cout << t->get_node()->lex << " = " << t->get_node()->data.value.data_as__int64 << " type of __int64" << std::endl; break;
	case TYPE_CHAR: std::cout << t->get_node()->lex << " = " << t->get_node()->data.value.data_as_char << " type of char" << std::endl; break;
	}
}

void TDiagram::print_array(Tree* t, int index) {
	switch (t->get_node()->data.type) {
	case TYPE_INT: std::cout << t->get_node()->lex << "[" << index << "] = " << t->get_node()->data.value.array_as_int[index] << " type of int" << std::endl; break;
	case TYPE_SHORT: std::cout << t->get_node()->lex << "[" << index << "] = " << t->get_node()->data.value.array_as_short[index] << " type of short" << std::endl; break;
	case TYPE_LONG: std::cout << t->get_node()->lex << "[" << index << "] = " << t->get_node()->data.value.array_as_long[index] << " type of long" << std::endl; break;
	case TYPE__INT64: std::cout << t->get_node()->lex << "[" << index << "] = " << t->get_node()->data.value.array_as__int64[index] << " type of __int64" << std::endl; break;
	case TYPE_CHAR: std::cout << t->get_node()->lex << "[" << index << "] = " << t->get_node()->data.value.array_as_char[index] << " type of char" << std::endl; break;
	}
}