#pragma once
#pragma once
#ifndef _BST_h
#define _BST_h
#include <iostream>
#include <stdio.h>
#include "String.h"
#include <assert.h>

template <typename T>
struct BST {
private:
	struct Node {
		T value;
		int32_t var_value;
		Node* left;
		Node* right;
		Node* parent;

		Node(T value, int32_t var_value) {
			this->value = value;
			this->var_value = var_value;
			left = right = parent = nullptr;
		}

		Node(const Node& rhs) {
			value = rhs.value;
			var_value = rhs.var_value;
			if (rhs.right) { 
				right = new Node{ *rhs.right }; // list initialization.  Recursive call to Node(const Node& rhs)
				right->parent = this;
			} else {
				right = nullptr;
			}
			if (rhs.left) {
				left = new Node{*rhs.left};
				left->parent = this;
			} else {
				left = nullptr;
			}
		}

		~Node(void) { 
			delete left;  // calls delete of left, which recursively calls delete on its left and right
			delete right;
		}
	}; // end of struct Node

private:

	static void recursiveDelete(Node* root) {
		if (root == nullptr) { return root;  }
		recursiveDelete(root->left);
		recursiveDelete(root->right);
		delete root; // calls Node's destructor
	}

	static Node* recursiveCopy(Node* root) {
		if (root == nullptr) { return 0; }

		Node* result = new Node(root->value, root->var_value);
		result->value = root->value;
		result->var_value = root->var_value;
		result->parent = nullptr;

		Node* left = recursiveCopy(root->left);
		Node* right = recursiveCopy(root->right);

		result->left = left;
		result->right = right;
		if (left != nullptr) { left->parent = result; }
		if (right != nullptr) { right->parent = result; }

		return result;
	}

	static Node* successor(Node* p) {
		if (p->right != 0) { // p has a right subtree
							 /* successor to p will be smallest node in right subtree */
			Node* next = p->right;
			while (next->left != 0) {
				next = next->left;
			}
			return next;
		}
		else { // p does not have a right subtree
			   /* successor to p will be closest ancestor where we go up a left branch */
			Node* parent = p->parent;
			while (parent != 0 && parent->right == p) {
				p = parent;
				parent = p->parent;
			}
			return parent;
		}
	}

	void remove(Node* p) {
		if (p == 0) { 
			return;
		}

		if (p->right == nullptr) { // easy case! 
			Node* child = p->left;
			Node* parent = p->parent;
			if (parent == nullptr) {
				if (child != nullptr) {
					child->parent = nullptr;
				}
				root = child;
				p->left = p->right = nullptr;
				delete p;
				return;
			}

			if (p == parent->left) {
				parent->left = child;
			}
			else {
				parent->right = child;
			}
			if (child != nullptr) {
				child->parent = parent;
			}
			delete p;
		}
		else { // nasty case! 
			Node* victim = successor(p);//find next largest node
			p->value = victim->value;//p = val of next largest node
			Node* parent = victim->parent;//parent = parent of next largest node
			Node* child = victim->right;//child = child of next largest node
			if (parent->left == victim) {
				parent->left = child;
			}
			else {
				parent->right = child;
			}
			if (child != nullptr) {
				child->parent = parent;
			}
			victim->right = victim->left = nullptr;
			delete victim;
		}
	}

	/* recursive version of find */
	static Node* find(Node* root, T value) {
		if (root == nullptr) { return root; }
		if (root->value == value) { return root; }
		if (value < root->value) {
			return find(root->left, value);
		}
		return find(root->right, value);
	}
	/* The iterative version of find.*/
	/*
	static Node* find(Node* root, T value) {
		while (root != 0 && root->value != value) {
			if (value < root->value) {
				root = root->left;
			}
			else {
				root = root->right;
			}
		}
		return root;
	}
	*/

	Node* root;

public:

	BST(void) { root = nullptr; }
	~BST(void) {
		delete root;
	}

	/* copy constructor */
	BST(const BST& that) {
		root = recursiveCopy(that.root);
		//if (that.root) {
		//	root = new Node{ *that.root };
		//} else {
		//	root = nullptr;
		//}
	}

	/* assignment operator */
	BST& operator=(const BST& that) {
		if (this != &that) {
			delete root;
			root = recursiveCopy(that.root);
			//if (that.root) {
			//	root = new Node{ *that.root };
			//} else {
			//	root = nullptr;
			//}
		}
		return *this;
	}

	void insert(T value, int32_t var_value) {
		Node* child = new Node(value, var_value); // create new Node which will be inserted

		if (root == nullptr) {
			root = child; // new child becomes root
			return;
		}

		Node* parent = nullptr;
		Node* p = root;			// start from p = root
		while (p != nullptr) {	// keep searching for empty spot to insert our new Node
			parent = p;
			if (value < p->value) {
				p = p->left;
			}
			else {
				p = p->right;
			}
		}	// while loop exits when p->left or p->right = null ptr

		if (value < parent->value) {
			assert(parent->left == nullptr);	// double-check to see if while loop worked correctly
			parent->left = child;	// insert our new node
		}
		else { // same as above
			assert(parent->right == nullptr); 
			parent->right = child;
		}
		child->parent = parent;
	}

	void updateVar(T value, int32_t var_val) {
		Node* n = find(root, value);
		n->var_value = var_val;
	}

	int getVar(T value) {
		Node* n = find(root, value);
		return n->var_value;
	}

	/* remove with value as parameter, not Node* */
	void remove(T value) {
		Node* p = find(root, value);
		remove(p);
	}

	bool varExists(T value) {
		if (find(root, value) == nullptr) { return false; }
		else { return true; }
	}

	/* print the tree in sorted order (in-order); left, root, right 
	 * spec is the format string to be used for values.
	 */
	void print(const char* spec) {
		if (root == nullptr) { return; } // special case for empty tree
		printf("\n");
		Node* p;

		/* set p to smallest */
		p = root;
		while (p->left != nullptr) {
			p = p->left;
		}

		const char* prefix = "";
		while (p != nullptr) {
			printf("%s", prefix);
			printf(spec, p->value);
			prefix = ", ";
			p = successor(p);
		}
		printf("\n\n");
	}

};


#endif /* _BST_h */