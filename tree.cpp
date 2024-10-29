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
	while(t != NULL && memcmp(lex, t->node->lex, std::max(strlen(lex), strlen(t->node->lex))) != 0) {
		t = t->parent;
	}
	return(t);
}

Tree* Tree::find_up_at_level(Tree* tree, type_lex lex) {
	Tree* t = tree;
	while (t != NULL && t->parent->right != t) {
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

void Tree::semantic_set_type(Tree* tree, type_data type) {
	tree->node->type = type;
}

Tree* Tree::semantic_get_type(type_lex lex) {
	return(find_up(this, lex));
}

void Tree::semantic_set_init(Tree* tree, int init) {
	tree->node->init = init;
}

Tree* Tree::semantic_get_init(type_lex lex) {
	return(find_up(this, lex));
}

void Tree::semantic_set_array_size(Tree* tree, int array_size) {
	tree->node->array_size = array_size;
}

Tree* Tree::semantic_get_array_size(type_lex lex) {
	return(find_up(this, lex));
}

Tree* Tree::semantic_include(type_lex lex) {
	if (is_exists(this, lex))
		scaner->print_error("Identifier already exists", lex);
	Tree* t; Node n;
	memcpy(n.lex, lex, strlen(lex) + 1);
	n.array_size = -1;
	n.init = -1;
	n.type = TYPE_UNKNOWN;
	n.object = OBJECT_UNKNOWN;
	current->set_left(&n);
	current = current->left;
	return current;
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
	if (find_up_at_level(tree, lex) != NULL)
		return 1;
	return 0;
}


Tree::~Tree() {
	if (left != NULL)
		delete left;
	if (right != NULL)
		delete right;
	delete node;
}