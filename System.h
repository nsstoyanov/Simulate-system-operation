#pragma once
#include "Expression.h"
#include "utils.h"
#include <queue>
#include <stdexcept>
#include <iostream>
using namespace std;

class System {
public:
	System(const char* entry);
	void run();
	void read();
	void calc();
	~System();
private:
	char entry[256];
	std::queue<Expression*> expressions;
	unsigned expCount = 0;
	double variables[26]{};
	bool defined[26]{};
	int indexOf(char c) const {
		c = myToUpper(c);   
		if (c >= 'A' && c <= 'Z') return c - 'A';
		throw std::runtime_error("Invalid variable");
	}

};

System::System(const char* entry) {
    unsigned size = myStrLen(entry);

    for (size_t i = 0; i < size; i++) {
        char c = myToUpper(entry[i]);
        if (c != 'R' && c != 'C')
            throw std::invalid_argument("Entry string can contain only R and C.");
		this->entry[i] = c;
    }
	this->entry[size] = '\0';
   
}
void System::run() {
	unsigned size = myStrLen(entry);
	for (size_t i = 0; i < size; i++)
	{
		if (entry[i] == 'C')
			calc();
		else
			read();
	}
}
void System::read() {
	cout << "Enter expression:";
	char exp[256];
	cin.getline(exp,256);
	Expression* ex = new Expression(exp);
	ex->setId(++expCount);
	expressions.push(ex);
	cout << "Expression (" << ex->getId() << ") was read." << endl;
}
void System::calc() {
	if (expressions.empty()) {
		cout << "Queue is empty" << endl;
		return;
	}

	size_t queueSize = expressions.size();
	bool anyCalculated = false;

	for (size_t i = 0; i < queueSize; i++) {
		Expression* expr = expressions.front();
		expressions.pop();

		try {
			double val = expr->evaluate(expr->getRoot(), variables, defined);
			char var = expr->getVariable();
			int idx = indexOf(var);
			variables[idx] = val;
			defined[idx] = true;

			cout << "Expression (" << expr->getId() << ") was calculated. The value of "
				<< var << " is now " << val << endl;

			delete expr;
			anyCalculated = true;
		}
		catch (std::exception& e) {
			cout << "Expression (" << expr->getId() << ") was postponed: " << e.what() << endl;
			expressions.push(expr); 
		}
	}

	if (!anyCalculated && !expressions.empty()) {
		cout << "No expressions could be calculated at this step." << endl;
	}
}
System::~System() {
	while (!expressions.empty()) {
		delete expressions.front();
		expressions.pop();
	}
}