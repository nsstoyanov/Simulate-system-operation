#pragma once
#include <iostream>
#include <stack>
#include <cstring>


#pragma warning(disable : 4996)

enum NodeType {
    CONST,
    VARIABLE,
    UNAREN_OP,
    BINARY_OP,
    FUNC
};

struct Node {
    NodeType type;

    double number;
    char variable;
    char op;
    char func[4];

    Node* left;
    Node* right;

    Node** args;     
    int argCount;
    Node(double val)
        : type(CONST), number(val), variable(0), op(0),
        left(nullptr), right(nullptr), args(nullptr), argCount(0)
    {
        func[0] = '\0';
    }

    Node(char var)
        : type(VARIABLE), number(0), variable(var), op(0),
        left(nullptr), right(nullptr), args(nullptr), argCount(0)
    {
        func[0] = '\0';
    }
    Node(char oper, Node* child)
        : type(UNAREN_OP), number(0), variable(0), op(oper),
        left(child), right(nullptr), args(nullptr), argCount(0)
    {
        func[0] = '\0';
    }
    Node(char oper, Node* L, Node* R)
        : type(BINARY_OP), number(0), variable(0), op(oper),
        left(L), right(R), args(nullptr), argCount(0)
    {
        func[0] = '\0';
    }

    Node(const char* name, Node** arr, int cnt)
        : type(FUNC), number(0), variable(0), op(0),
        left(nullptr), right(nullptr), args(arr), argCount(cnt)
    {
        func[0] = toupper(name[0]);
        func[1] = toupper(name[1]);
        func[2] = toupper(name[2]);
        func[3] = '\0';
    }
};

class Expression {
public:
    Expression(const char* text);
    ~Expression();

    double evaluate(Node* node, double* vars, bool* defined);
    Node* getRoot() const { return root; }
    char getVariable() const { return variable; }

    void setId(int i) { id = i; }
    int getId() const { return id; }

private:
    char variable;
    Node* root;
    int id;

    Node* buildAST(const char* text);
    Node* parse(const char* ex);

    void makeOperation(std::stack<char>& ops, std::stack<Node*>& vals);
    int priority(char op);
    bool isRightAssoc(char op);

    Node* buildFunction(const char* name, Node** arr, int count);

    int varIndex(char c);

    Node* simplify(Node* node, double* vars, bool* defined, bool& complete);

    void deleteNode(Node* n);
};
