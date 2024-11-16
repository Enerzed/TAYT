// file tree.cpp

#include "tree.hpp"

Tree::Tree(TScaner* scaner, Tree* parent, Tree* left, Tree* right, Node* node) {
	this->scaner = scaner;
	this->parent = parent;
	this->left = left;
	this->right = right;
	this->node = new Node();
	memcpy(this->node, node, sizeof(Node));
}

Tree::Tree(TScaner* scaner) {
	this->scaner = scaner;
	this->parent = NULL;
	this->left = NULL;
	this->right = NULL;
	this->node = new Node();
	memcpy(this->node, node, sizeof(Node));
}

void Tree::set_left(Node* node) {
	Tree* left = new Tree(this->scaner, this, NULL, NULL, node);
	this->left = left;
}

void Tree::set_right(Node* node) {
	Tree* right = new Tree(this->scaner, this, NULL, NULL, node);
	this->right = right;
}

Tree* Tree::find_up(type_lex lex) {
	return(find_up(this, lex));
}

Tree* Tree::find_up(Tree* tree, type_lex lex) {
	Tree* t = tree;
	while (t != NULL) {
			if (memcmp(lex, t->node->lex, std::max(strlen(lex), strlen(t->node->lex))) == 0)
				break;
		t = t->parent;
	}
	return(t);
}

Tree* Tree::find_up_at_level(Tree* tree, type_lex lex) {
	Tree* t = tree;
	while (t != NULL && t->parent != NULL && t->parent->right != t) {
		if (memcmp(lex, t->node->lex, std::max(strlen(lex), strlen(t->node->lex))) == 0)
			return(t);
		t = t->parent;
	}
	return NULL;
}

Tree* Tree::find_right_left(type_lex lex) {
	return(find_right_left(this, lex));
}

Tree* Tree::find_right_left(Tree* tree, type_lex lex) {
	Tree* t = tree->right;
	while(t != NULL && memcmp(lex, t->node->lex, std::max(strlen(lex), strlen(t->node->lex))) != 0) {
		t = t->left;
	}
	return(t);
}


void Tree::set_current(Tree* tree) {
	current = tree;
}

Tree* Tree::get_current() {
	return(current);
}

void Tree::set_node(Node* node) {
	this->node = node;
}

Node* Tree::get_node() {
	return(node);
}

Tree* Tree::semantic_get_type(type_lex lex, type_object object) {
	Tree* t = find_up(current, lex);
	if (t == NULL)
		scaner->print_error("Identifier not found", lex);
	if (t->node->object != object)
	{
		if (t->node->object == OBJECT_VARIABLE)
			std::cout << lex << " is a variable" << std::endl;
		if (t->node->object == OBJECT_ARRAY)
			std::cout << lex << " is an array" << std::endl;
		if (t->node->object == OBJECT_FUNCTION)
			std::cout << lex << " is a function" << std::endl;
		scaner->print_error("Identifier used incorrectly", lex);
	}
	return(t);
}

void Tree::semantic_set_object(Tree* tree, type_object object) {
	tree->node->object = object;
}

Tree* Tree::semantic_get_object(type_lex lex) {
	Tree* t = find_up(current, lex);
	if (t == NULL)
		scaner->print_error("Identifier not found", lex);
	return(t);
}

void Tree::set_current_node(Node* node) {
	current->node = node;
}

Node* Tree::get_current_node() {
	if (current == NULL)
		return NULL;
	return current->node;
}

void Tree::semantic_set_init(Tree* tree, int init) {
	tree->node->init = init;
}

Tree* Tree::semantic_get_init(type_lex lex) {
	Tree* t = find_up(current, lex);
	if (t == NULL)
		scaner->print_error("Identifier not found", lex);
	if (t->node->object != OBJECT_VARIABLE || t->node->object != OBJECT_ARRAY)
		scaner->print_error("Identifier used incorrectly", lex);
	return(t);
}

void Tree::semantic_set_array_size(Tree* tree, int array_size) {
	tree->node->array_size = array_size;
}

Tree* Tree::semantic_get_array_size(type_lex lex) {
	Tree* t = find_up(this, lex);
	if (t == NULL)
		scaner->print_error("Identifier not found", lex);
	if (t->node->object != OBJECT_ARRAY)
		scaner->print_error("Identifier is not an array", lex);
	return(t);
}

Tree* Tree::semantic_include(type_lex lex, type_object object, type_data type) {
	if (object != OBJECT_UNKNOWN)
		if (is_exists(current, lex))
			scaner->print_error("Identifier already exists", lex);
	//std::cout << "include: " << lex << std::endl;
	Tree* t; Node n;
	if (object == OBJECT_VARIABLE) {
		memcpy(n.lex, lex, strlen(lex) + 1);
		n.array_size = -1;
		n.init = -1;
		n.object = object;
		n.data.type = type;
		n.data.value.array_as__int64 = 0;
		
		current->set_left(&n);
		current = current->left;
		return current;
	}
	if (object == OBJECT_ARRAY) {
		memcpy(n.lex, lex, strlen(lex) + 1);
		n.array_size = -1;
		n.init = -1;
		n.object = object;
		n.data.type = type;
		switch (type) {
			case TYPE_INT: n.data.value.array_as_int = 0; break;
			case TYPE_SHORT: n.data.value.array_as_short = 0; break;
			case TYPE_LONG: n.data.value.array_as_long = 0; break;
			case TYPE__INT64: n.data.value.array_as__int64 = 0; break;
			case TYPE_CHAR: n.data.value.array_as_char = 0; break;
			case TYPE_VOID: scaner->print_error("Type void is not allowed", lex); break;
		}
		current->set_left(&n);
		current = current->left;
		return current;
	}
	if (object == OBJECT_FUNCTION) {
		memcpy(n.lex, lex, strlen(lex) + 1);
		n.array_size = -1;
		n.init = -1;
		n.data.type = type;
		n.object = object;
		current->set_left(&n);
		current = current->left;
		t = current;
		memcpy(&n.lex, &"", 2);
		n.array_size = -1;
		n.init = -1;
		n.data.type = type;
		n.object = object;
		current->set_right(&n);
		current = current->right;
		return t;
	}
	if (object == OBJECT_UNKNOWN) {
		memcpy(n.lex, lex, strlen(lex) + 1);
		n.array_size = -1;
		n.init = -1;
		n.data.type = type;
		n.object = object;
		current->set_left(&n);
		current = current->left;
		t = current;
		memcpy(&n.lex, &"", 2);
		n.array_size = -1;
		n.init = -1;
		n.data.type = type;
		n.object = object;
		current->set_right(&n);
		current = current->right;
		return t;
	}
	return NULL;
}

void Tree::print() {
	std::cout << "node: " << node->lex << std::endl;
	if (left != NULL)
		std::cout << "left: " << left->node->lex << std::endl;
	if (right != NULL)
		std::cout << "right: " << right->node->lex << std::endl;

	if (left != NULL)
		left->print();
	if (right != NULL)
		right->print();
}

int Tree::is_exists(Tree* tree, type_lex lex) {
	if (find_up_at_level(tree, lex) == NULL)
		return 0;
	return 1;
}


Tree::~Tree() {
	if (left != NULL)
		delete left;
	if (right != NULL)
		delete right;
	delete node;
}