#ifndef _tree_h_
#define _tree_h_

#include "node.h"

class Tree {
	Node* root = nullptr;

	void copy(const Tree& tree);
	void move(Tree& tree);
	void clear();

public:
	Tree();
	Tree(const Tree& tree);
	Tree(Tree&& tree);

	Tree& operator=(const Tree& tree);
	Tree& operator=(Tree&& tree);

	friend std::ostream& operator <<(std::ostream& os, const Tree& tree);
	void PrintAsBinary() const;

	~Tree();

	Node* Root();
	void setRoot(Node* newroot);

	Node* Search(string action, int priority);
	void Insert(string action, int priority);
	Node* Insert_part2(Node* subtree, Key* key);
	
	void Delete(string action, int priority);
	void Delete_part2(Key key, Node* node);
	Node* FindMax();
	void Pop(); // delete the key with highest priority

	int CountPriorities(int x);
	void PrioritySearch(int x); // print all actions with priority x

	void ChangeActionPriority(string action, int old, int p); // old - old priority , p - new priority
};


#endif
