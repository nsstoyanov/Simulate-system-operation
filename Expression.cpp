#include "Expression.h"
#include <stdexcept>
#include <cmath>
#include "utils.h"
Expression::Expression(const char* text) {
    root = buildAST(text);
}

Expression::~Expression() {
    deleteNode(root);
}
int Expression::priority(char op) {
    switch (op) {
    case '~': return 4;  
    case '#': return 4;  
    case '^': return 3; 
    case '*': return 2;
    case '/': return 2;
    case '+': return 1;
    case '-': return 1;
    case '(': return 0;
    default: return 0;
    }
}

bool Expression::isRightAssoc(char op) {
    return op == '^';
}

int Expression::varIndex(char c) {
    c = myToUpper(c);
    if (c < 'A' || c > 'Z') throw std::invalid_argument("Invalid variable name");
    return c - 'A';
}

void Expression::makeOperation(std::stack<char>& ops, std::stack<Node*>& vals) {
    char op = ops.top();
    ops.pop();

    if (op == '~' || op == '#') {
        if (vals.empty()) throw std::invalid_argument("Invalid unary expression");
        Node* a = vals.top(); vals.pop();
        vals.push(new Node(op, a));
        return;
    }
    if (vals.size() < 2) 
        throw std::invalid_argument("Invalid binary expression");

    Node* right = vals.top(); vals.pop();
    Node* left = vals.top(); vals.pop();

    vals.push(new Node(op, left, right));
}


Node* Expression::buildFunction(const char* name, Node** arr, int count) {
    return new Node(name, arr, count);
}


Node* Expression::buildAST(const char* text) {
    int n = myStrLen(text);

    int eq = -1;
    for (int i = 0; i < n; i++) {
        if (text[i] == '=') 
            eq = i; break; 
    }
    if (eq == -1)
        throw std::invalid_argument("Expression must contain '='");

    int p = 0;
    while (p < eq && text[p] == ' ')
        p++;

    if (p >= eq || !isLetter(text[p]))
        throw std::invalid_argument("Left side must be a single variable");

    variable = text[p];

    return parse(text + eq + 1);
}

Node* Expression::parse(const char* ex) {
    std::stack<char> ops;
    std::stack<Node*> vals;

    char funcNames[16][4];      
    int funcPtr = 0;

    int argCountStack[16];      
    int argPtr = 0;

    int len = myStrLen(ex);
    for (int i = 0; i < len; i++) {

        char c = ex[i];

        if (c == ' ' || c == '\t') 
            continue;

        if (myIsDigit(c)) {
            double num = convertToNumber(ex, i);
            i--;
            vals.push(new Node(num));
            continue;
        }

        if (isLetter(c)) {
            char word[8];
            int w = 0;

            while (i < len && isLetter(ex[i])) {
                word[w++] = myToUpper(ex[i]);
                i++;
            }
            word[w] = '\0';

            if (i < len && ex[i] == '(' && w >= 2) {
                strcpy(funcNames[funcPtr], word);
                funcPtr++;

                ops.push('(');
                argCountStack[argPtr++] = 0;  
                continue;
            }

            if (w == 1) {
                vals.push(new Node(word[0]));
                i--;
                continue;
            }

            throw std::invalid_argument("Unknown identifier");
        }
        if (c == '(') {
            ops.push('(');
            continue;
        }

        if (c == ')') {
            while (!ops.empty() && ops.top() != '(')
                makeOperation(ops, vals);
            if (ops.empty()) 
                throw std::invalid_argument("Mismatched parentheses");

            ops.pop(); 

            if (funcPtr > 0 && argPtr > 0) {

                int argsCount = argCountStack[argPtr - 1] + 1;
                argPtr--;

                funcPtr--;
                char fname[4];
                strcpy(fname, funcNames[funcPtr]);

                Node** arr = new Node * [argsCount];
                for (int k = argsCount - 1; k >= 0; k--) {
                    if (vals.empty()) throw std::invalid_argument("Function arg missing");
                    arr[k] = vals.top();
                    vals.pop();
                }

                Node* fn = buildFunction(fname, arr, argsCount);
                vals.push(fn);
            }

            continue;
        }

        if (c == ';') {
            while (!ops.empty() && ops.top() != '(')
                makeOperation(ops, vals);

            if (argPtr == 0) 
                throw std::invalid_argument("Unexpected semicolon");

            argCountStack[argPtr - 1]++;
            continue;
        }

        if (c == '+' || c == '-') {
            int j = i - 1;
            while (j >= 0 && (ex[j] == ' ' || ex[j] == '\t')) 
                j--;

            if (j < 0 || ex[j] == '(' || ex[j] == ';' || isOperator(ex[j])) {
                char un = (c == '+') ? '#' : '~';
                ops.push(un);
                continue;
            }
        }

        if (isOperator(c)) {
            while (!ops.empty() && ops.top() != '(' && (priority(ops.top()) > priority(c) || (priority(ops.top()) == priority(c) && !isRightAssoc(c)))) {
                makeOperation(ops, vals);
            }
            ops.push(c);
            continue;
        }

        throw std::invalid_argument("Invalid character");
    }

    while (!ops.empty()) {
        if (ops.top() == '(') 
            throw std::invalid_argument("Mismatched '('");
        makeOperation(ops, vals);
    }

    if (vals.empty()) 
        throw std::invalid_argument("Empty expression");

    return vals.top();
}

void Expression::deleteNode(Node* n) {
    if (!n) 
        return;

    if (n->type == CONST || n->type == VARIABLE) {
        delete n;
        return;
    }

    if (n->type == UNAREN_OP) {
        deleteNode(n->left);
        delete n;
        return;
    }

    if (n->type == BINARY_OP) {
        deleteNode(n->left);
        deleteNode(n->right);
        delete n;
        return;
    }

    if (n->type == FUNC) {
        for (int i = 0; i < n->argCount; i++)
            deleteNode(n->args[i]);
        delete[] n->args;
        delete n;
        return;
    }
}
Node* Expression::simplify(Node* node, double* vars, bool* defined, bool& complete) {

    if (!node) 
        return nullptr;

    if (node->type == CONST)
        return node;

    if (node->type == VARIABLE) {
        int idx = varIndex(node->variable);
        if (defined[idx]) {
            double v = vars[idx];
            delete node;
            return new Node(v);
        }
        else {
            complete = false;
            return node;
        }
    }
    if (node->type == UNAREN_OP) {
        bool leftFull = true;
        Node* L = simplify(node->left, vars, defined, leftFull);
        node->left = L;

        if (leftFull) {
            double val = 0;
            if (node->op == '#') 
                val = +L->number; // unaren plus
            else if (node->op == '~') 
                val = -L->number; // uraren minus

            delete node->left;
            delete node;

            return new Node(val);
        }

        complete = false;
        return node;
    }

    if (node->type == BINARY_OP) {
        bool LF = true, RF = true;
        Node* L = simplify(node->left, vars, defined, LF);
        Node* R = simplify(node->right, vars, defined, RF);
        node->left = L;
        node->right = R;

        if (LF && RF && L->type == CONST && R->type == CONST) {
            double a = L->number;
            double b = R->number;
            double res = 0;

            switch (node->op) {
            case '+': res = a + b; break;
            case '-': res = a - b; break;
            case '*': res = a * b; break;
            case '/':
                if (b == 0) throw std::invalid_argument("Division by zero");
                res = a / b;
                break;
            case '^': res = pow(a, b); break;
            }

            delete L;
            delete R;
            delete node;

            return new Node(res);
        }

        complete = false;
        return node;
    }

    if (node->type == FUNC) {

        bool allFull = true;

        for (int i = 0; i < node->argCount; i++) {
            bool tf = true;
            node->args[i] = simplify(node->args[i], vars, defined, tf);
            if (!tf) allFull = false;
        }
        if (allFull) {
            double result = 0;

            if (strcmp(node->func, "SUM") == 0) {
                double s = 0;
                for (int i = 0; i < node->argCount; i++)
                    s += node->args[i]->number;
                result = s;
            }
            else if (strcmp(node->func, "PRD") == 0) {
                double p = 1;
                for (int i = 0; i < node->argCount; i++)
                    p *= node->args[i]->number;
                result = p;
            }
            else if (strcmp(node->func, "MIN") == 0) {
                double m = node->args[0]->number;
                for (int i = 1; i < node->argCount; i++)
                    if (node->args[i]->number < m) m = node->args[i]->number;
                result = m;
            }
            else if (strcmp(node->func, "MAX") == 0) {
                double m = node->args[0]->number;
                for (int i = 1; i < node->argCount; i++)
                    if (node->args[i]->number > m) m = node->args[i]->number;
                result = m;
            }
            else if (strcmp(node->func, "AVG") == 0) {
                double s = 0;
                for (int i = 0; i < node->argCount; i++)
                    s += node->args[i]->number;
                result = s / node->argCount;
            }

            for (int i = 0; i < node->argCount; i++)
                delete node->args[i];
            delete[] node->args;
            delete node;

            return new Node(result);
        }

        complete = false;
        return node;
    }

    return node;
}


double Expression::evaluate(Node* node, double* vars, bool* defined) {

    bool complete = true;
    Node* newRoot = simplify(node, vars, defined, complete);

    if (newRoot != node) 
        root = newRoot;
    

    if (!complete) 
        throw std::invalid_argument("Variable not defined");
    
    if (newRoot->type == CONST) 
        return newRoot->number;

    if (newRoot->type == UNAREN_OP) {
        double v = evaluate(newRoot->left, vars, defined);
        return (newRoot->op == '~') ? -v : +v;
    }

    if (newRoot->type == BINARY_OP) {
        double l = evaluate(newRoot->left, vars, defined);
        double r = evaluate(newRoot->right, vars, defined);

        switch (newRoot->op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/':
            if (r == 0) 
                throw std::invalid_argument("Division by zero");
            return l / r;
        case '^': return pow(l, r);
        }
    }

    if (newRoot->type == FUNC) {
        if (strcmp(newRoot->func, "SUM") == 0) {
            double s = 0;
            for (int i = 0; i < newRoot->argCount; i++)
                s += evaluate(newRoot->args[i], vars, defined);
            return s;
        }

        if (strcmp(newRoot->func, "PRD") == 0) {
            double p = 1;
            for (int i = 0; i < newRoot->argCount; i++)
                p *= evaluate(newRoot->args[i], vars, defined);
            return p;
        }

        if (strcmp(newRoot->func, "MIN") == 0) {
            double m = evaluate(newRoot->args[0], vars, defined);
            for (int i = 1; i < newRoot->argCount; i++) {
                double v = evaluate(newRoot->args[i], vars, defined);
                if (v < m) m = v;
            }
            return m;
        }

        if (strcmp(newRoot->func, "MAX") == 0) {
            double m = evaluate(newRoot->args[0], vars, defined);
            for (int i = 1; i < newRoot->argCount; i++) {
                double v = evaluate(newRoot->args[i], vars, defined);
                if (v > m) m = v;
            }
            return m;
        }

        if (strcmp(newRoot->func, "AVG") == 0) {
            double s = 0;
            for (int i = 0; i < newRoot->argCount; i++)
                s += evaluate(newRoot->args[i], vars, defined);
            return s / newRoot->argCount;
        }
    }

    throw std::invalid_argument("Evaluation error");
}