#include "node.h"

#include <iomanip>
#define RESIZE_STRING(word, lenght) ( (word.size() <= lenght) ? (word) : (word.substr(0, lenght)) )

#include <iostream>
using namespace std;

#define RESET   "\033[0m"
#define REDD    "\033[31m"

void Node::copy(const Node& node) {
	for (int i = 0; i < 3; i++) {
		if (node.keys[i]) keys[i] = new Key(node.keys[i]->act, node.keys[i]->val, node.keys[i]->color);
		else keys[i] = nullptr;
	}
	for (int i = 0; i < 4; i++) child[i] = node.child[i];
	leaf = node.leaf;
	n = node.n;
}

void Node::move(Node& node) {
	for (int i = 0; i < 3; i++) {
		keys[i] = node.keys[i];
		node.keys[i] = nullptr;
	}
	for (int i = 0; i < 4; i++) child[i] = node.child[i];
	leaf = node.leaf;
	n = node.n;
}

void Node::clear() {
	for (int i = 0; i < 3; i++) {
		delete keys[i];
		keys[i] = nullptr;
	}
	//for (int i = 0; i < 4; i++) child[i] = nullptr;
}

Node::Node() {
	for (int i = 0; i < 3; i++) keys[i] = nullptr;
	for (int i = 0; i < 4; i++) child[i] = nullptr;
}

Node::Node(const Node& node) { copy(node); }

Node::Node(Node&& node) { move(node); }

Node& Node::operator=(const Node& node) {
	if (this != &node) {
		clear();
		copy(node);
	}
	return *this;
}
Node& Node::operator=(Node&& node) {
	if (this != &node) {
		clear();
		move(node);
	}
	return *this;
}

Node::~Node() { clear(); }

void Node::setKey(Key& key, int i) { if (i >= 0 && i <= 2) { if (keys[i] == nullptr) n++; keys[i] = &key; } }

void Node::remKey(int i) { if (i >= 0 && i <= 2 && keys[i]) { delete keys[i]; keys[i] = nullptr; n--; } }

Key Node::getKey(int i) const { return *keys[i]; } // function returns value, it can be risky if pointer is returned

int Node::posKey(Key& key) {
	for (int i = 0; i < 3; i++) if (keys[i] != nullptr && keys[i]->val == key.val && keys[i]->act == key.act) return i;
	return -1;
}

void Node::setChild(Node* pnode, int i) { if (i >= 0 && i <= 3) child[i] = pnode; }

void Node::remChild(int i) { if (i >= 0 && i <= 3 && child[i]) child[i] = nullptr; }

Node* Node::getChild(int i) const { return child[i]; }

int Node::WhichChildAmI(){
	if (parent == nullptr) return 0;
	for (int i = 0; i <= parent->n; i++) if (parent->child[i] == this) return i;
}


void Node::setLeaf(bool value) { leaf = value; }

bool Node::isLeaf() const { return leaf; }

int Node::N() const { return n; }

void Node::setN(int val){n = val;}

void Node::setParent(Node* newparent) { parent = newparent;  }

Node* Node::getParent() const {return parent; }

bool Node::containsKey(Key& key) const {
	int i = 0;
	while (i < n && keys[i]!= nullptr && (key.val > keys[i]->val || (key.val == keys[i]->val && key.act > keys[i]->act))) i++;
	if (i < n && keys[i] != nullptr && key.val == keys[i]->val && key.act == keys[i]->act) return true;
	return false;
}

void Node::SplitChild(int i){
	Node* z = new Node(), *y = child[i];
	if (!y) return;
	z->leaf = y->leaf;
	z->n = 1;
	y->n = 1;
	z->keys[0] = y->keys[2]; y->keys[2] = nullptr;
	z->keys[0]->color = BLACK;
	y->keys[0]->color = BLACK;
	if (!y->leaf) {
		z->child[0] = y->child[2]; y->child[2] = nullptr;
		z->child[1] = y->child[3]; y->child[3] = nullptr;
		z->child[0]->parent = z;
		z->child[1]->parent = z;
	}
	for (int j = n+1; j > i+1; j--) child[j] = child[j - 1]; // ???
	child[i+1] = z;
	z->parent = this;

	for (int j = n; j >= i + 1; j--) keys[j] = keys[j - 1];
	keys[i] = y->keys[1]; y->keys[1] = nullptr;

	if (n == 0) keys[i]->color = BLACK;
	else if (n == 2) {
		keys[0]->color = RED;
		keys[1]->color = BLACK;
		keys[2]->color = RED;
	}
	else keys[i]->color = RED;
	n++;
}

void Node::print() const{
	cout << " ";
	for (int i = 0; i < 3; i++)
		if (i < n) {
			if (keys[i]->color == RED) {
				cout << ((i == 0) ? "(" : "|") << REDD << setw(3) << setfill(' ') << keys[i]->val << RESET;
				cout << REDD << setw(2) << setfill(' ') << RESIZE_STRING(keys[i]->act, 2) << RESET;
			}
			else {
				cout << ((i == 0) ? "(" : "|") << setw(3) << setfill(' ') << keys[i]->val;
				cout << setw(2) << setfill(' ') << RESIZE_STRING(keys[i]->act, 2);
			}
		}
		else {
			cout << "|" << "     ";
		}
	cout << ") ";
}

Node* Node::FindSuccNode(Key* key){
	if (leaf) return this;
	int i = posKey(*key); // succ is in child[i+1] subtree
	Node* succ = child[i + 1];
	while (!succ->leaf) succ = succ->child[0];
	return succ;
}

Node* Node::FindPrevNode(Key* key){
	if (leaf) return this;
	int i = posKey(*key);
	Node* prev = child[i];
	while (!prev->leaf) prev = prev->child[prev->n-1];
	return prev;
}

Node* Node::FindBrother(){
	if (parent == nullptr) return nullptr;
	if (parent->n == 1) {
		if (parent->child[0] == this) return parent->child[1];
		else return parent->child[0];
	}
	else if (parent->n == 2 && parent->keys[0]->color == BLACK) {
		if (parent->child[0] == this) return nullptr;
		else if (parent->child[1] == this) return parent->child[2];
		else if (parent->child[2] == this) return parent->child[1];
	}
	else if (parent->n == 2 && parent->keys[1]->color == BLACK) {
		if (parent->child[2] == this) return nullptr;
		else if (parent->child[1] == this) return parent->child[0];
		else if (parent->child[0] == this) return parent->child[1];
	}
	else if (parent->n == 3) {
		if (parent->child[0] == this) return parent->child[1];
		else if (parent->child[1] == this) return parent->child[0];
		else if (parent->child[2] == this) return parent->child[3];
		else if (parent->child[3] == this) return parent->child[2];
	}
	return nullptr;
}

void Node::BorrowFromNonfullBrother(Node* brother){
	bool right; // 1 - right brother, 0 - left brother
	right = (parent->child[0] == this && parent->child[1] == brother || parent->child[1] == this && parent->child[2] == brother || parent->child[2] == this && parent->child[3] == brother);

	int i = WhichChildAmI();
	Key* separator;

	if (right) {
		separator = parent->keys[i];
		parent->keys[i] = brother->keys[0];

		brother->keys[0] = brother->keys[1]; brother->keys[1] = nullptr;
		brother->keys[0]->color = BLACK;
		brother->n = 1;

		parent->keys[i]->color = separator->color;
		keys[0] = separator;
		keys[0]->color = BLACK;
		n++;

		if (!leaf) {
			child[1] = brother->child[0];
			brother->child[0] = brother->child[1];
			brother->child[1] = brother->child[2];
			brother->child[2] = nullptr;
		}
	}
	else {
		separator = parent->keys[i-1];
		parent->keys[i-1] = brother->keys[1]; brother->keys[1] = nullptr;

		brother->keys[0]->color = BLACK;
		brother->n = 1;

		parent->keys[i-1]->color = separator->color;
		keys[0] = separator;
		keys[0]->color = BLACK;
		n++;

		if (!leaf) {
			child[1] = brother->child[2];
			brother->child[2] = nullptr;
		}
	}
}

void Node::BorrowFromFullBrother(Node* brother){
	bool right; // 1 - right brother, 0 - left brother
	right = (parent->child[0] == this && parent->child[1] == brother || parent->child[1] == this && parent->child[2] == brother || parent->child[2] == this && parent->child[3] == brother);

	int i = WhichChildAmI();
	Key* separator;

	if (right) {
		separator = parent->keys[i];
		parent->keys[i] = brother->keys[1];
		parent->keys[i]->color = separator->color;

		keys[1] = brother->keys[0];
		keys[0] = separator;
		keys[0]->color = BLACK;
		n+=2;

		brother->keys[0] = brother->keys[2]; brother->keys[1] = nullptr; brother->keys[2] = nullptr;
		brother->keys[0]->color = BLACK;
		brother->n = 1;

		if (!leaf) {
			child[1] = brother->child[0];
			child[2] = brother->child[1];
			brother->child[0] = brother->child[2];
			brother->child[1] = brother->child[3];
			brother->child[2] = nullptr;
			brother->child[3] = nullptr;
		}
	}
	else {
		separator = parent->keys[i-1];
		parent->keys[i-1] = brother->keys[1];
		parent->keys[i-1]->color = separator->color;

		keys[0] = brother->keys[2];
		keys[1] = separator;
		keys[1]->color = BLACK;
		n += 2;

		brother->keys[0]->color = BLACK; brother->keys[1] = nullptr; brother->keys[2] = nullptr;
		brother->n = 1;

		if (!leaf) {
			child[1] = brother->child[2];
			child[2] = brother->child[3];
			brother->child[2] = nullptr;
			brother->child[3] = nullptr;
		}
	}
}

void Node::BorrowFromNonfullNeighbour(Node* neighbour){
	bool right = (parent->keys[0]->color == BLACK)? true : false;
	int i = (parent->keys[0]->color == BLACK) ? 0 : 2;
	Key* separator;

	if (right) {
		separator = parent->keys[0];

		parent->keys[0] = neighbour->keys[0];

		neighbour->keys[0] = neighbour->keys[1]; neighbour->keys[1] = nullptr;
		neighbour->keys[0]->color = BLACK;
		neighbour->n = 1;

		parent->keys[0]->color = RED;
		parent->keys[1]->color = BLACK;
		keys[0] = separator;
		keys[0]->color = BLACK;
		n++;

		if (!leaf) {
			child[1] = neighbour->child[0];
			neighbour->child[0] = neighbour->child[1];
			neighbour->child[1] = neighbour->child[2];
			neighbour->child[2] = nullptr;
		}
	}
	else {
		separator = parent->keys[1];

		parent->keys[1] = neighbour->keys[1];
		parent->keys[1]->color = RED;
		parent->keys[0]->color = BLACK;

		keys[0] = separator;
		keys[0]->color = BLACK;
		n ++;

		neighbour->keys[0]->color = BLACK; neighbour->keys[1] = nullptr; neighbour->keys[2] = nullptr;
		neighbour->n = 1;

		if (!leaf) {
			child[1] = neighbour->child[2];
			neighbour->child[2] = nullptr;
		}
	}

}

void Node::BorrowFromFullNeighbour(Node* neighbour){
	bool right = (parent->keys[0]->color == BLACK) ? true : false;
	int i = (parent->keys[0]->color == BLACK) ? 0 : 2;
	Key* separator;

	if (right) {
		separator = parent->keys[0];
		parent->keys[0] = neighbour->keys[1];
		parent->keys[0]->color = RED;
		parent->keys[1]->color = BLACK;

		keys[1] = neighbour->keys[0];
		keys[0] = separator;
		keys[0]->color = BLACK;
		n += 2;

		neighbour->keys[0] = neighbour->keys[2]; neighbour->keys[1] = nullptr; neighbour->keys[2] = nullptr;
		neighbour->keys[0]->color = BLACK;
		neighbour->n = 1;

		if (!leaf) {
			child[1] = neighbour->child[0];
			child[2] = neighbour->child[1];
			neighbour->child[0] = neighbour->child[2];
			neighbour->child[1] = neighbour->child[3];
			neighbour->child[2] = nullptr;
			neighbour->child[3] = nullptr;
		}
	}
	else {
		separator = parent->keys[1];
		parent->keys[1] = neighbour->keys[1];
		parent->keys[1]->color = RED;
		parent->keys[1]->color = BLACK;

		keys[0] = neighbour->keys[2];
		keys[1] = separator;
		keys[1]->color = BLACK;
		n += 2;

		neighbour->keys[0]->color = BLACK; neighbour->keys[1] = nullptr; neighbour->keys[2] = nullptr;
		neighbour->n = 1;

		if (!leaf) {
			child[1] = neighbour->child[2];
			child[2] = neighbour->child[3];
			neighbour->child[2] = nullptr;
			neighbour->child[3] = nullptr;
		}
	}
}

Node* Node::BrotherMerge(Node* brother){
	bool right; // 1 - right brother, 0 - left brother
	right = (parent->child[0] == this && parent->child[1] == brother || parent->child[1] == this && parent->child[2] == brother || parent->child[2] == this && parent->child[3] == brother);

	int i = WhichChildAmI();
	Key* separator;

	//Node* deletionnode = parent->child[i];

	if (right) {
		separator = parent->keys[i]; parent->keys[i] = nullptr;
		brother->keys[1] = brother->keys[0];
		brother->keys[0] = separator;
		brother->keys[0]->color = BLACK;
		brother->keys[1]->color = RED;

		for (int j = i; j < parent->n-1; j++) parent->keys[j] = parent->keys[j + 1];
		for (int j = i; j < parent->n; j++) parent->child[j] = parent->child[j + 1];

		if (!leaf) {
			brother->child[2] = brother->child[1];
			brother->child[1] = brother->child[0];
			brother->child[0] = child[0];
		}
	}
	else {
		separator = parent->keys[i-1]; parent->keys[i-1] = nullptr;
		brother->keys[1] = separator;
		brother->keys[1]->color = BLACK;
		brother->keys[0]->color = RED;

		for (int j = i-1; j < parent->n-1; j++) parent->keys[j] = parent->keys[j + 1];
		for (int j = i; j < parent->n; j++) parent->child[j] = parent->child[j + 1];

		if (!leaf) {
			brother->child[2] = child[0];
		}
	}
	parent->child[parent->n] = nullptr; // ???
	parent->keys[parent->n - 1] = nullptr; // ???
	parent->n = parent->n - 1;
	brother->n = brother->n + 1;

	//delete deletionnode;
	return this;
}

Node* Node::NeighbourMerge(Node* neighbour){
	bool right = (parent->keys[0]->color == BLACK) ? true : false;
	int i = (parent->keys[0]->color == BLACK) ? 0 : 2;
	Key* separator;

	//Node* deletionnode = parent->child[i];

	if (right) {
		separator = parent->keys[0]; parent->keys[0] = nullptr;
		neighbour->keys[1] = neighbour->keys[0];
		neighbour->keys[0] = separator;
		neighbour->keys[0]->color = BLACK;
		neighbour->keys[1]->color = RED;

		for (int j = i; j < parent->n - 1; j++) parent->keys[j] = parent->keys[j + 1];
		for (int j = i; j < parent->n; j++) parent->child[j] = parent->child[j + 1];
		parent->keys[0]->color = BLACK;

		if (!leaf) {
			neighbour->child[2] = neighbour->child[1];
			neighbour->child[1] = neighbour->child[0];
			neighbour->child[0] = child[0];
		}
	}
	else {
		separator = parent->keys[1]; parent->keys[1] = nullptr;
		neighbour->keys[1] = separator;
		neighbour->keys[1]->color = BLACK;
		neighbour->keys[0]->color = RED;

		for (int j = i - 1; j < parent->n - 1; j++) parent->keys[j] = parent->keys[j + 1];
		for (int j = i; j < parent->n; j++) parent->child[j] = parent->child[j + 1];
		parent->keys[0]->color = BLACK;

		if (!leaf) {
			neighbour->child[2] = child[0];
		}
	}
	parent->child[parent->n] = nullptr; // ???
	parent->keys[parent->n - 1] = nullptr; // ???
	parent->n = parent->n - 1;
	neighbour->n = neighbour->n + 1;

	//delete deletionnode;
	return this;
}

bool operator==(Key k1, Key k2) {
	return (k1.val == k2.val && k1.act == k2.act);
}

bool operator!=(Key k1, Key k2){
	return !(k1==k2);
}

bool operator<(Key k1, Key k2){
	return (k1.val < k2.val || (k1.val == k2.val && k1.act < k2.act ));
}

bool operator>(Key k1, Key k2){
	return (k1.val < k2.val || (k1.val == k2.val && k1.act < k2.act));;
}

bool operator<=(Key k1, Key k2){
	return (k1 < k2 || k1 == k2);
}

bool operator>=(Key k1, Key k2){
	return (k1 > k2 || k1 == k2);
}

void Key::print(){
	if (color == RED) cout << REDD <<"(" << val << ")"<< RESET << " Action: " << act << endl;
	else cout  << "(" << val << ") Action: " << act << endl;
}
