/**************************************************************************
Class:			CSCI 3333.01 Data Structures and Algorithms
				Spring 2025
Project 2:		Binary Arithmetic Expression Trees
Name:			Coder Warrior
ID:             11111111
Description:	This is the header file for Project 2. We define treeNode, 
				treeNodePtr, and calculator to evaluate the value of
				an arithmetic binary expression tree. 
				You can choose to add more parts into this file
**************************************************************************/

#ifndef PROJECT02
#define PROJECT02

#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>
#include <cmath>
#include <stdexcept>

using namespace std;


//define tree node pointer type
class treeNode; //forward declaration to help define treeNodePtr
typedef treeNode* treeNodePtr;

//i will be using three components: value: string/char, left child node, and right child node
//i will also use a an operator FLAG to distinguish between operators and operands
class treeNode
{
public:
	//do the work
	//operator or operand stores as a string
	string value;
	//pointer to the left child
	treeNodePtr left;
	//pointer to the right chidl
	treeNodePtr right;

	//constructor that willl initialize a node with given values
	///and optional left and right children
	treeNode(string val, treeNodePtr l = nullptr, treeNodePtr r = nullptr)
	{
		value = val;
		left = l;
		right = r;
	}
};

//calculate value of a binary expression tree
//t: pointer to root node of expression tree
double calculator(treeNodePtr t)
{
	//do the work
	//no tree
	if (t == nullptr)
	{
		throw invalid_argument("Error: Null tree node encountered during evaluation.");
	}

	//base case: leaf node (operand)
	if (t->left == nullptr && t->right == nullptr) {
		//convert string operand to double
		return stod(t->value);
	}

	//handle ! not operator
	if (t->value == "!") {
		//the result of calculator(t->right) is passed to the logical
		//not operator
		return !calculator(t->right);
	}

	//recursively evaluate left&right subtrees
	double leftVal = calculator(t->left);	
	double rightVal = calculator(t->right);

	//arithmetic operators
	if (t->value == "+") return leftVal + rightVal;
	if (t->value == "-") return leftVal - rightVal;
	if (t->value == "*") return leftVal * rightVal;
	if (t->value == "/") {
		if (rightVal == 0) {
			throw invalid_argument("Error: Division by zero encountered during evaluation.");
		}
		return leftVal / rightVal;
	}

	//handdle relational operators
	//will return 1/true or 0/false
	if (t->value == "<") return leftVal < rightVal;
	if (t->value == "<=") return leftVal <= rightVal;
	if (t->value == ">") return leftVal > rightVal;
	if (t->value == ">=") return leftVal >= rightVal;
	if (t->value == "==") return leftVal == rightVal;
	if (t->value == "!=") return leftVal != rightVal;

	//handle logical operators
	//convert nonzero to true
	bool leftBool = (leftVal != 0);
	bool rightBool = (rightVal != 0);
	if (t->value == "&&") return leftBool && rightBool;
	if (t->value == "||") return leftBool || rightBool;

	//unknown operator
	throw invalid_argument("Error: Unknown operator '" + t->value + "' encountered during evaluation.");

}
//traversal functions
//preorder traversal
void preorder(treeNodePtr t, ostream& out)
{
	if (!t) return;
	out << t->value << " ";
	preorder(t->left, out);
	preorder(t->right, out);
}

//inorder traversal
void inorder(treeNodePtr t, ostream& out)
{
	if (!t) return;
	//add parentheses for clarity
	if (t->left && t->right) out << "( ";
	inorder(t->left, out);
	out << t->value << " ";
	inorder(t->right, out);
	if (t->left && t->right) out << ") ";
}

//postorder traversal
void postorder(treeNodePtr t, ostream& out)
{
	if (!t) return;
	postorder(t->left, out);
	postorder(t->right, out);
	out << t->value << " ";
}
#endif
