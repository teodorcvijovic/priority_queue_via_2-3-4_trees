#ifndef _node_h_
#define _node_h_

#include <iostream>
#include <string>

using namespace std;

enum Color { RED, BLACK };

struct Key {
	string act; // action
	int val; // priority
	Color color;
	Key(string action, int priority, Color color = RED) : act(action), val(priority), color(color) {};

	friend bool operator==(Key k1, Key k2);
	friend bool operator!=(Key k1, Key k2);
	friend bool operator<(Key k1, Key k2);
	friend bool operator>(Key k1, Key k2);
	friend bool operator<=(Key k1, Key k2);
	friend bool operator>=(Key k1, Key k2);
	void print();
};

class Node {
	Key* keys[3];
	Node* child[4]; // child[i] contains values between keys[i-1] and keys[i]
	Node* parent = nullptr;
	bool leaf = true;
	int n = 0; // number of keys in node

	void copy(const Node& node);
	void move(Node& node);
	void clear();

public:
	friend class Tree;

	Node();
	Node(const Node& node);
	Node(Node&& node);

	Node& operator=(const Node& node);
	Node& operator=(Node&& node);

	~Node();

	void setKey(Key& key, int i);
	void remKey(int i);
	Key getKey(int i) const;
	int posKey(Key& key);

	void setChild(Node* pnode, int i);
	void remChild(int i);
	Node* getChild(int i) const;
	int WhichChildAmI();

	void setLeaf(bool value);
	bool isLeaf() const;
	int N() const;
	void setN(int val);
	void setParent(Node* newparent);
	Node* getParent() const;

	bool containsKey(Key& key) const;

	void SplitChild(int pos);
	
	void print() const;

	Node* FindSuccNode(Key* key);
	Node* FindPrevNode(Key* key);

	Node* FindBrother();
	void BorrowFromNonfullBrother(Node* brother);
	void BorrowFromFullBrother(Node* brother);
	void BorrowFromNonfullNeighbour(Node* neighbour);
	void BorrowFromFullNeighbour(Node* neighbour);
	Node* BrotherMerge(Node* brother);
	Node* NeighbourMerge(Node* neighbour);

};

#endif
