#include "tree.h"

#include <iostream>

using namespace std;

#include <queue>
#include <iomanip>
#define RESIZE_STRING(word, lenght) ( (word.size() <= lenght) ? (word) : (word.substr(0, lenght)) )

#define RESET   "\033[0m"
#define REDD    "\033[31m"

void Tree::copy(const Tree& tree){
	queue<Node*> queue;
	Node* next = tree.root;
	root = new Node(*tree.root);
	Node* copynext = root;
	queue.push(next);
	queue.push(copynext);
	while (!queue.empty()) {
		next = queue.front();
		queue.pop();
		copynext = queue.front();
		queue.pop();
		for (int i = 0; i < 4; i++)
			if (next->child[i] != nullptr) {
				Node* newchild = new Node(*next->child[i]);
				copynext->child[i] = newchild;
				newchild->parent = copynext;
				queue.push(next->child[i]);
				queue.push(newchild);
			}
	}
}

void Tree::move(Tree& tree){
	root = tree.root;
	tree.root = nullptr;
}

void Tree::clear(){
	if (root == nullptr) return;
	queue<Node*> queue;
	Node* next = root;
	queue.push(next);
	while (!queue.empty()) {
		next = queue.front();
		for (int i = 0; i < 4; i++)
			if (next->child[i] != nullptr) {
				queue.push(next->child[i]);
			}
		queue.pop();
		delete next;
	}
	root = nullptr;
}

Tree::Tree(){}

Tree::Tree(const Tree& tree) { copy(tree); }

Tree::Tree(Tree&& tree) { move(tree); }

Tree& Tree::operator=(const Tree& tree){
	if (this != &tree) {
		clear();
		copy(tree);
	}
	return *this;
}

Tree& Tree::operator=(Tree&& tree){
	if (this != &tree) {
		clear();
		move(tree);
	}
	return *this;
}

void Tree::PrintAsBinary() const{
	if (root == nullptr) return;
	cout << endl;
	int height = 0;
	Node* p = root;
	while (p) { height++; p = p->getChild(0); }
	height--;

	int line_len = static_cast<int>(pow(4, height)) * (13 + 6 + 2);
	int node_len = 13 + 6;

	int blank = static_cast<int>(floor((line_len - (13 + 6 + 2))) / 2);
	//int blank = static_cast<int>(floor((line_len- static_cast<int>(pow(4, currlevel)) * (13 + 2)))/2);

	queue<Node*> queue;
	Node* next = root;
	queue.push(next);

	int node_count = 1, currlevel = 0, nextnodes = 0, currnodes = 1;
	for (int t = 0; t < blank; t++) cout << " ";

	while (!queue.empty()) {
		next = queue.front();
		queue.pop();

		if (next->N() == 1)
			cout << "        ("<< setw(3) << setfill(' ') << next->getKey(0).val <<")        ";
		else if (next->N() == 2 && next->getKey(0).color == BLACK)
			cout << "     ("<< setw(3) << setfill(' ') << next->getKey(0).val <<")->" << REDD <<"(" << setw(3) << setfill(' ') << next->getKey(1).val<< ")" << RESET <<"    ";
		else if (next->N() == 2 && next->getKey(0).color == RED) 
			cout << "     "  << REDD << "(" << setw(3) << setfill(' ') << next->getKey(0).val << ")" << RESET << "<-("<< setw(3) << setfill(' ') << next->getKey(1).val <<")    ";
		else if (next->N() == 3)
			cout << REDD << " (" << setw(3) << setfill(' ') << next->getKey(0).val << ")" << RESET <<"<-("<< setw(3) << setfill(' ') << next->getKey(1).val <<")->"<< REDD << "(" << setw(3) << setfill(' ') << next->getKey(2).val << ") " << RESET;

		//os << ((i == 0) ? "(" : "|") << setw(3) << setfill(' ') << next->getKey(i).val << "[" << setw(3) << setfill(' ') << RESIZE_STRING(next->getKey(i).act, 3) << "]";

		for (int i = 0; i < 4; i++)
			if (next->getChild(i) != nullptr) {
				nextnodes++;
				queue.push(next->getChild(i));
			}

		currnodes--;
		node_count--;
		if (currnodes == 0) {
			while (node_count > 0) {
				cout << "             ";
				node_count--;
			}
			//node_count is 0

			for (int t = 0; t < blank; t++) cout << " ";
			cout << endl;
			cout << endl;

			currlevel++;
			node_count = static_cast<int>(pow(4, currlevel));
			currnodes = nextnodes;
			blank = static_cast<int>(floor((line_len - static_cast<int>(pow(4, currlevel)) * (13 + 6 + 2))) / 2);

			for (int t = 0; t < blank; t++) cout << " ";
		}
	}
	cout << endl;
}

Tree::~Tree() { clear(); }

Node* Tree::Root() { return root; }

void Tree::setRoot(Node* newroot) { root = newroot; }

Node* Tree::Search(string action, int priority){
	Node* p = root, * prev = nullptr;
	while (p) {
		prev = p;
		int i = 0;
		while (i < p->n && p->keys[i]!= nullptr && (priority > p->keys[i]->val || (priority == p->keys[i]->val && action > p->keys[i]->act)))i++;
		if (i < p->n && p->keys[i] != nullptr && priority == p->keys[i]->val && action == p->keys[i]->act) return p;
		else if (p->leaf) break;
		else p = p->child[i];
	}
	return prev;
}

void Tree::Insert(string action, int priority){
	Key *pkey = new Key(action, priority);
	if (root == nullptr) {
		root = new Node();
		Insert_part2(root, pkey);
		return;
	}
	Node* x = Search(action, priority), *y = nullptr, *s;
	if (x->containsKey(*pkey)) return; // key already excists

	if (x->n == 3) {
		int pos;
		y = x;
		while (y->parent != nullptr && y->parent->n == 3) { y = y->parent; }// from node y do the SPLIT starting with node y
		
		while (y) {
			if (y->n == 3 && y == root) {
				s = new Node();
				root = s;
				s->leaf = false;
				s->child[0] = y;
				y->parent = s;
				s->SplitChild(0);
				y = Insert_part2(s, pkey);
			}
			else if (y->n == 3 && y != root) {
				int t = 0;
				if (y->parent != nullptr) {
					for (; t <= y->parent->n; t++) if (y->parent->child[t] == y) y->parent->SplitChild(t);
					y = Insert_part2(y->parent, pkey);
				}
			}
			else y = Insert_part2(y, pkey);
		}
	}
	else Insert_part2(x, pkey);
}

Node* Tree::Insert_part2(Node* node, Key* key){
	int i = node->n - 1;
	if (node->leaf) {
		for (;i>=0 && (key->val < node->keys[i]->val || (key->val == node->keys[i]->val && key->act < node->keys[i]->act)) ;i--) node->keys[i+1] = node->keys[i]; // make space for new key
		node->keys[i+1] = key;
		if (node->n == 2) {
			node->keys[0]->color = RED;
			node->keys[1]->color = BLACK;
			node->keys[2]->color = RED;
		}
		else if (node->n == 0) key->color = BLACK;
		node->n = node->n + 1;
		return nullptr;
	}
	else {
		for (; i >= 0 && (key->val < node->keys[i]->val || (key->val == node->keys[i]->val && key->act < node->keys[i]->act)); i--);
		i++;
		if (node->child[i]->n == 3) {
			node->SplitChild(i);
			if (i >= 0 && (key->val > node->keys[i]->val || (key->val == node->keys[i]->val && key->act > node->keys[i]->act))) i++;
		}
		return node->child[i];
	}
}


std::ostream& operator<<(std::ostream& os, const Tree& tree){
	if (tree.root == nullptr) return os;
	os << endl;
	int height = 0;
	Node* p = tree.root;
	while (p) { height++; p = p->getChild(0); }
	height--;
	
	int line_len = static_cast<int>(pow(4, height))*(13+6+2);
	int node_len = 13+6;
	
	int blank = static_cast<int>(floor((line_len - (13+6+2))) / 2);
	//int blank = static_cast<int>(floor((line_len- static_cast<int>(pow(4, currlevel)) * (13 + 2)))/2);

	queue<Node*> queue;
	Node* next = tree.root;
	queue.push(next);

	int node_count = 1, currlevel = 0, nextnodes=0,currnodes=1;
	for (int t = 0; t < blank; t++) os << " ";

	while (!queue.empty()) {
		next = queue.front();
		queue.pop();

		os << " ";
		for (int i = 0; i < 3; i++)
			if (i < next->N()) {
				if (next->getKey(i).color == RED) {
					os << ((i == 0) ? "(" : "|") << REDD << setw(3) << setfill(' ') << next->getKey(i).val << RESET;
					os << REDD << setw(2) << setfill(' ')  << RESIZE_STRING(next->getKey(i).act, 2) << RESET;
				}
				else {
					os << ((i == 0) ? "(" : "|") << setw(3) << setfill(' ')  << next->getKey(i).val;
					os << setw(2) << setfill(' ') << RESIZE_STRING(next->getKey(i).act, 2);
				}
			}
			else {
				os << "|" << "     ";
			}
		os << ") ";

		//os << ((i == 0) ? "(" : "|") << setw(3) << setfill(' ') << next->getKey(i).val << "[" << setw(3) << setfill(' ') << RESIZE_STRING(next->getKey(i).act, 3) << "]";

		for (int i = 0; i < 4; i++)
			if (next->getChild(i) != nullptr) {
				nextnodes++;
				queue.push(next->getChild(i));
			}

		currnodes--;
		node_count--;
		if (currnodes == 0) {
			while (node_count > 0) {
				os << "             ";
				node_count--;
			}
			//node_count is 0

			for (int t = 0; t < blank; t++) os << " ";
			os << endl;
			os << endl;

			currlevel++;
			node_count = static_cast<int>(pow(4, currlevel));
			currnodes = nextnodes;
			blank = static_cast<int>(floor((line_len - static_cast<int>(pow(4, currlevel)) * (13+6+2))) / 2);

			for (int t = 0; t < blank; t++) os << " ";
		}
	}
	os << endl;
	return os;
}

void Tree::Delete(string action, int priority){
	Key key = { action, priority }; // the key we want to delete

	Node* deletionnode = Search(key.act, key.val), * t = nullptr;
	if (deletionnode == nullptr || !deletionnode->containsKey(key)) return;

	int pos = deletionnode->posKey(key);
	Key* pkey;

	if (deletionnode->leaf) {
		pkey = deletionnode->keys[pos];
		Delete_part2(*pkey, deletionnode);
	}
	else { // 3
		t = deletionnode->FindSuccNode(&key);
	
		Key* tmp = deletionnode->keys[pos]; 
		deletionnode->keys[pos] = t->keys[0]; 
		t->keys[0] = tmp; 
		
		Color tcol = deletionnode->keys[pos]->color;
		deletionnode->keys[pos]->color = t->keys[0]->color;
		t->keys[0]->color = tcol;

		pkey = t->keys[0];
		Delete_part2(*pkey,t);
	}
}

void Tree::Delete_part2(Key key, Node* node){
	Node* old = nullptr;
	if (node == nullptr) return;
	if (key.color == RED) { // 1.
		int i = 0, j = 0;
		int nn = node->n;
		for (; i < nn; i++) {
			if (*node->keys[i] != key) node->keys[j++] = node->keys[i];
			else node->remKey(i);
		}
		for (; j < 3; j++) node->keys[j] = nullptr;
	}
	else if (node->n == 2) { // 2.1
		int i = 0, j = 0;
		int nn = node->n;
		for (; i < nn; i++) {
			if (*node->keys[i] != key) node->keys[j++] = node->keys[i];
			else node->remKey(i);
		}
		for (; j < 3; j++) node->keys[j] = nullptr;
		node->keys[0]->color = BLACK;
	}
	else if (node->n == 3) { // 2.1
		node->remKey(1);
		node->keys[1] = node->keys[2];
		node->keys[2] = nullptr;
		node->keys[1]->color = BLACK;
	}
	else {
		node->remKey(0);
		while (node && node->n == 0) {
			if (node == root) { // doesn't have a brother nor a neighbour
				if (node->child[0] != nullptr) { root = node->child[0]; node->child[0] = nullptr; }
				else if (node->child[1] != nullptr) { root = node->child[1]; node->child[1] = nullptr; }
				else root = nullptr;
				delete node;
				return;
			}
			Node* brother = node->FindBrother();
			if (brother != nullptr) { // 2.2
				if (brother->n == 2) { node->BorrowFromNonfullBrother(brother); break; } // (1)
				else if (brother->n == 3) { node->BorrowFromFullBrother(brother); break; }// (2)
				else if (brother->n == 1) {
					old = node->BrotherMerge(brother); // (3) returns node
					node = old->parent;
					delete old;
				}
			}
			else {
				Node* neighbour = node->parent->child[1]; // 2.3
				if (neighbour->n == 2) { node->BorrowFromNonfullNeighbour(neighbour); break; } // (1)
				else if (neighbour->n == 3) { node->BorrowFromFullNeighbour(neighbour); break; }// (2)
				else if (neighbour->n == 1) {
					old = node->NeighbourMerge(neighbour); // (3) returns node
					node = old->parent;
					delete old;
				}
			}
		}
	}
}

Node* Tree::FindMax(){
	Node* p = root, *prev = nullptr;
	while (p) {
		prev = p;
		p = p->child[p->n];
	}
	return prev;
}

void Tree::Pop(){
	Node* node = FindMax();
	if (node == nullptr || root->n == 0) {
		cout << "Stablo je prazno." << endl;
		return;
	}
	cout << "Akcija najviseg prioriteta: ";
	node->keys[node->n - 1]->print();
	cout << endl;
	if (node) Delete_part2(*node->keys[node->n-1], node);
}

int Tree::CountPriorities(int x){
	int count = 0;
	Node* p = root;
	queue<Node*> queue;
	while (p) {
		int i = 0;
		while (i < p->n && x >= p->keys[i]->val) {
			if (x == p->keys[i]->val) {
				count++;
				if (!p->leaf) queue.push(p->child[i]);
			}
			i++;
		}
		if (i >= 1 && i <= p->n && p->keys[i - 1] != nullptr && x == p->keys[i - 1]->val && (!p->leaf)) p = p->child[i];
		else if (p->leaf) {
			if (queue.empty()) break;
			else { p = queue.front(); queue.pop(); }
		}
		else p = p->child[i];
	}
	return count;
}

void Tree::PrioritySearch(int x) {
	Node* p = root;
	queue<Node*> queue;
	bool found = false;
	if (p == nullptr) { cout << "Pretraga je neuspesno zavrsena" << endl; return;}
	while (p) {
		int i = 0;
		while (i < p->n && x >= p->keys[i]->val) {
			if (x == p->keys[i]->val) {
				if (!p->leaf) queue.push(p->child[i]);
				p->keys[i]->print();
				found = true;
			}
			i++;
		}
		if (i >= 1 && i <= p->n && p->keys[i - 1] != nullptr && x == p->keys[i - 1]->val && (!p->leaf)) p = p->child[i];
		else if (p->leaf) {
			if (queue.empty()) break;
			else { p = queue.front(); queue.pop(); }
		}
		else p = p->child[i];
	}
	if (!found) cout << "Pretraga je neuspesno zavrsena";
}

void Tree::ChangeActionPriority(string action, int old, int p){
	Node* currnode = Search(action, old);
	Node* excists = Search(action, p);

	Key oldkey(action, old );
	Key newkey(action, p);
	int pos = currnode->posKey(oldkey);

	if (!currnode->containsKey(oldkey)) Insert(action, p);
	else if (excists->containsKey(newkey)) return;
	else {
		Node* prev = currnode->FindPrevNode(&oldkey), * succ = currnode->FindSuccNode(&oldkey);
		if (oldkey < *succ->keys[0] && oldkey > * prev->keys[prev->n - 1] && !currnode->leaf) {
			currnode->keys[pos]->val = p;
			return;
		}
		else if (currnode->leaf && *currnode->keys[0] <= newkey && *currnode->keys[currnode->n-1] >= newkey && currnode->n!=1) {
			currnode->keys[pos]->val = p;
			for (int i = 0; i < currnode->n - 1; i++)
				for (int j = i + 1; j < currnode->n; j++)
					if (*currnode->keys[j] < *currnode->keys[i]) {
						Key* t = currnode->keys[i];
						currnode->keys[i] = currnode->keys[j];
						currnode->keys[j] = t;
					}
			if (currnode->n == 3) {
				currnode->keys[0]->color = RED;
				currnode->keys[1]->color = BLACK;
				currnode->keys[2]->color = RED;
			}
		}
		else {
			Delete(action, old);
			Insert(action, p);
		}
	}
}
