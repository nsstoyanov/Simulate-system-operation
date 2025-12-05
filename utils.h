#pragma once
int myStrLen(const char* str);
char myToLower(char c);
char myToUpper(char c);
bool myIsDigit(char c);
double convertToNumber(const char* ex, int& index);
bool isLetter(char c);
bool isOperator(char c);
bool isRightAssociative(char c);

double myPow(double base, int exp);
int myMin(int a, int b);
int myMax(int a, int b);