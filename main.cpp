/**************************************************************************
Class:			CSCI 3333.01 Data Structures and Algorithms
				Spring 2025
Project 2:		Binary Arithmetic Expression Trees
Name:			Coder Warrior
ID:             11111111
Description: Use stacks to build the binary expression tree for a given 
	input binary arithmetic expression and do syntactic and semantic analysis.
Input file: expressions.txt
Output file: outfile.txt
Example format of output file:
	Input expression 1
	List of errors, if any.
	If no errors, then give the value of the expression
	Infix expression
	Infix expression
	Postfix expression
	......
**************************************************************************/

#include <iostream>
#include <fstream>
#include <cassert>
#include <string> 
#include <cstdlib>
#include <stack>
#include <iomanip>
#include <sstream>
#include <vector>

//the header file defines the tree node and other parts needed.
#include "project2_head.h"	

using namespace std;

//turn a token into a new tree node
//in this instance, numbers are leaf nodes
treeNodePtr makeNode(const string& token)
{
	return new treeNode(token, nullptr, nullptr);
}

treeNodePtr makeOpNode(const string& op, treeNodePtr left, treeNodePtr right)
{
	return new treeNode(op, left, right);
}

//will reutrn true if right associativ
bool RightAssociative(const string& op) {
	return (op == "!");
}

//single and double character operators
bool isOperator(const string& s)
{
	static const vector<string> operators = {
		//arithmetic operators
		"+", "-", "*", "/",
		//relational operators
		"<", "<=", ">", ">=",
		//equalti operators
		"==", "!=",
		//logical operators
		"&&", "||", "!"
	};
	for (const string& op : operators)
	    if (s == op) return true;

	return false;
}

//operator precedence
//left to right, pop operators with higher or equal precedence
int precedence(const string& op)
{
	//unary operator
	if (op == "!") return 7; 
	if (op == "*" || op == "/") return 6;
	if (op == "+" || op == "-") return 5;
	if (op == "<" || op == "<=" || op == ">" || op == ">=") return 4;
	if (op == "==" || op == "!=") return 3;
	if (op == "&&") return 2;
	if (op == "||") return 1;
	//unknown operator
	return 0; 
}

//apply operator op to the top one or two tree nodes in treeStack
//returns true on success, false on failure
bool applyOp(const string& op, stack<treeNodePtr>& treeStack, vector<string> &error_message)
{
	treeNodePtr t1, t2, newNode;
	//unary operator
	//one operandd
	//use right child to hold operand
	if (op == "!")
	{
		//error check
		if (treeStack.empty()) {
			error_message.push_back("Missing operand for '!'.");
			return false;
		}
	
		treeNodePtr operand = treeStack.top(); treeStack.pop();
		//rep unary node as op node w right child
		treeNodePtr newNode = makeOpNode("!", nullptr, operand);
		treeStack.push(newNode);
		return true;
	}
	else {
		//binary operatore therefore need two operands
		if (treeStack.size() < 2) {
			error_message.push_back("Missing operands for binary operator '" + op + "'.");
			return false;
		}
		treeNodePtr right = treeStack.top(); treeStack.pop();
		treeNodePtr left = treeStack.top(); treeStack.pop();
		treeNodePtr newNode = makeOpNode(op, left, right);
		treeStack.push(newNode);
		return true;
	}
}

//convert raw input string line to tokens
//tokens are numbers, operators, and parentheses
//keep whitespace
vector<string> tokenize(const string& line) {
	vector<string> tokens;
	size_t i = 0;
	while (i < line.size()) {
		char c = line[i];
		if (isspace((unsigned char)c)) {
			i++;
			continue;
		}

		//integer and float nums
		if (isdigit((unsigned char)c) || c == '.') {
			size_t j = i + 1;
			bool dotSeen = false;
				while (j < line.size() && (isdigit((unsigned char)line[j]) || line[j] == '.')) {
					if (line[j] == '.') {
						//second dot, stop
						if (dotSeen) break;
						dotSeen = true;
					}
					j++;
				}
			tokens.push_back(line.substr(i, j - i));
			i = j;
			continue;
		}

		//parentheses
		if (c == '(' || c == ')') {
			tokens.push_back(string(1, c));
			i++;
			continue;
		}

		//operators
		//check operators w two characters first
		if (i + 1 < line.size()) {
			string twoCharOp = line.substr(i, 2);
			if (twoCharOp == "<=" || twoCharOp == ">=" || twoCharOp == "==" | twoCharOp == "!=" || twoCharOp == "&&" || twoCharOp == "||") {
				tokens.push_back(twoCharOp);
				i += 2;
				continue;
			}
		}

		//single character operators
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '!') {
			tokens.push_back(string(1, c));
			i++;
			continue;
		}

		//unknown character
		//push it as a token so it can raise an error later
		tokens.push_back(string(1, c));
		i++;
	}
	return tokens;
}

//traversal functions
void printInfix(treeNodePtr root, ostream& out)
{
	if (!root) return;
	//if leaf operand
	if (!root->left && !root->right) {
		out << root->value;
		return;
	}
	//operator node
	//for ! print
	if (root->value == "!") {
		out << "(!";
		printInfix(root->right, out);
		out << ")";
		return;
	}
	out << "(";
	printInfix(root->left, out);
	out << " " << root->value << " ";
	printInfix(root->right, out);
	out << ")";
}

void printPrefix(treeNodePtr root, ostream& out)
{
	if (!root) return;
	out << root->value << " ";
	printPrefix(root->left, out);
	printPrefix(root->right, out);
}

void printPostfix(treeNodePtr root, ostream& out)
{
	if (!root) return;
	printPostfix(root->left, out);
	printPostfix(root->right, out);
	out << root->value << " ";
}

///recursively delete tree nodes
void deleteTree(treeNodePtr root)
{
	if (!root) return;
	deleteTree(root->left);
	deleteTree(root->right);
	delete root;
}


int main()
{
	ifstream inFile;
	ofstream outFile;

	//char str[100];
	//int i, j;
	//string numStr;
	//double x;
	//char p;
	//stack<char> opStack; //a stack of operators
	//stack<treeNodePtr> treeStack; //a stack of tree node pointers
	//treeNodePtr tmpNode, lc, rc;
	//bool tag;

	// open input file of expressions
	inFile.open("expressions.txt");
	// open output file
	outFile.open("expressions_output.txt");

	string line;
	int line_num = 0;

	//use a loop to read input expressions, one at a time
	//i = 0;
	//while (!inFile.eof())
	//{
	//	i++;
	//	inFile.getline(str, 100);
	//	outFile << "Input expression "<<i<<": " << str << endl;
	//	//use the stacks to build the binary expression tree for expression str
	//	//do syntactic and semantic analysis.
	//	// if no errors, then calculate the value and 
	//	// list prefix, infix and post fix expressions 
	//	//.....................

	//}
    while (getline(inFile, line)) {
    	++line_num;
    		//input expression into outfile
    		outFile << "Input expression " << line_num << ": " << line << endl;
    
    	//tokenize the line
    	vector<string> tokens = tokenize(line);
    
    	////quick check for empty expression
    	if (tokens.empty()) {
    		outFile << "Error: Empty expression." << endl;
            continue;
    	}
    
    	//stacks used by algorithm
    	//operator stack, stores operators and "("
    	stack<string> opStack;
    	//operand stack, stores tree nodes
    	stack<treeNodePtr> treeStack;
    
    	//error messages:
    	vector<string> error_message;
    
    	//we'll scan token left to right
    	for (size_t i = 0; i < tokens.size(); ++i) {
    		const string& token = tokens[i];
    
    		//if token is a number, make a leaf node and push onto tree stack
    		bool isNumber = false;
    
    		//try to parse token as a number
    		try {
    			size_t pos = 0;
    			stod(token, &pos);
    			if (pos == token.size()) {
    				isNumber = true;
    			}
    		}
    		catch (const invalid_argument&) {
    			//not a number
    		}
    		catch (const out_of_range&) {
    			error_message.push_back("Error: Number out of range: " + token);
    			continue;
    		}
    
    		if (isNumber) {
    			treeStack.push(makeNode(token));
    			continue;
    		}
    
    		//if token is "(", push onto op stack
    		if (token == "(") {
    			opStack.push(token);
    			continue;
    		}
    
    		//if token is ")", pop op stack until "(" is found
    		if (token == ")") {
    			//pop operators until "(" is found
    			bool leftParenFound = false;
    			while (!opStack.empty()) {
    				string topOp = opStack.top(); opStack.pop();
    				if (topOp == "(") {
    					leftParenFound = true;
    					break;
    				}
    				//apply operator
    				if (!applyOp(topOp, treeStack, error_message)) {
    					//error applying operator
    					break;
    				}
    			}
    			if (!leftParenFound) {
    				error_message.push_back("Error: Mismatched parentheses, no matching '(' found for ')'.");
    			}
    			continue;
    		}
    
    		//if token is an operator
    		//multi or single char
    		if (isOperator(token)) {
    			//pop operators from op stack while they have higher or equal precedence
    			while (!opStack.empty()) {
    				string topOp = opStack.top();
    				if (topOp == "(") break;
    				int topPrecedence = precedence(topOp);
    				int tokenPrecedence = precedence(token);
    				if ((RightAssociative(token) && topPrecedence > tokenPrecedence) || (!RightAssociative(token) && topPrecedence >= tokenPrecedence)) {
    					//pop and apply topOp
    					opStack.pop();
    					if (!applyOp(topOp, treeStack, error_message)) {
    						//error applying operator
    						break;
    					}
    					continue;
    				}
    				else {
    					break;
    				}
    			}
    			//push current operator onto op stack
    			opStack.push(token);
    			continue;
    		}
    
    		//unknown token
    		error_message.push_back("Error: Unknown token '" + token + "'.");
    	} //end for each token
    
    	//after all tokens processed, apply remaining operators
    	while (!opStack.empty()) {
    		string topOp = opStack.top(); opStack.pop();
    		if (topOp == "(" || topOp == ")") {
    			error_message.push_back("Error: Mismatched parentheses, no matching ')' found for '('.");
    			continue;
    		}
    		if (!applyOp(topOp, treeStack, error_message)) {
    			//error applying operator
    			break;
    		}
    	}
    
    	//extra checks
    	if (treeStack.empty()) {
    		error_message.push_back("Error: No operands in expression.");
    	}
    	else if (treeStack.size() > 1) {
    		error_message.push_back("Error: Too many operands, missing operators.");
    	}
    
    	//if any syntax erros found, report them and cleanup the nodes
    	if (!error_message.empty()) {
    		for (const string& msg : error_message) {
    			outFile << msg << endl;
    		}
    		//cleanup tree nodes
    		while (!treeStack.empty()) {
    			treeNodePtr t = treeStack.top(); treeStack.pop();
    			deleteTree(t);
    		}
    		continue; //next expression
    	}
    
    	//root
    	treeNodePtr root = treeStack.top(); treeStack.pop();
    
    	//evaluate the expression tree using calculator from header function
    	//catch runtime erros like zerodivision
    	double val = 0.0;
     	bool calcError = false;
    	string calcErrorMessage;
    	try {
    		//assumes calc returns double value
    		val = calculator(root);
    	}
    	catch (const invalid_argument& e) {
    		calcError = true;
    		calcErrorMessage = e.what();
    	}
    
    	if (calcError) {
    		outFile << calcErrorMessage << endl;
    		//cleanup tree
    		deleteTree(root);
    // 		continue; //next expression
    	}
    
    	//print value
    	outFile << fixed << setprecision(6);
    	outFile << "Value: " << val << endl;
    
    	//traversals
    	{
    		ostringstream oss;
    		printInfix(root, oss);
    		outFile << "Infix: " << oss.str() << endl;
    	}
    
    	{
    		ostringstream oss;
    		printPrefix(root, oss);
    		outFile << "Prefix: " << oss.str() << endl;
    	}
    
    	{
    		ostringstream oss;
    		printPostfix(root, oss);
    		outFile << "Postfix: " << oss.str() << endl;
    	}
    
    	outFile << "\n";
    
    	//cleanup tree
    	deleteTree(root);
}




//complete the program
cout << " Okay, all are done!" << endl;
inFile.close();
outFile.close(); 
return 0;
}
