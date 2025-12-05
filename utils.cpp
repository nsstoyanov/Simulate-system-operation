#pragma once
#include "utils.h"
int myStrLen(const char* str) {
	if (!str) {
		return 0;
	}
	int size = 0;
	while (*str) {
		size++;
		str++;
	}
	return size;
}
char myToLower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c + ('a' - 'A');
	}
	return c;

}
char myToUpper(char c) {
	if (c >= 'a' && c <= 'z') {
		return c - ('a' - 'A');
	}
	return c;
}
bool myIsDigit(char c) {
	return c >= '0' && c <= '9';
}
double convertToNumber(const char* ex, int& index) {
	bool flag = false;
	double j = 1.0;
	double number = 0;
	while ((myIsDigit(ex[index]) || ex[index] == '.') && ex[index] != '\0') {
		if (ex[index] == '.') {
			if (flag)
				break;
			flag = true;
		}
		else {
			number = (number * 10) + (ex[index] - '0');
			if (flag) {
				j *= 0.1;
			}
		}

		index++;
	}
	number *= j;
	return number;
}
bool isLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}
bool isOperator(char c) {
	switch (c) {
	case '#': case'~': case '^': case '*': case '/': case '+': case '-': return true;
	default: return false;
	}
}
bool isRightAssociative(char c) {
	return c == '^';
}

double myPow(double base, int exp) {
	double result = 1.0;
	bool negative = false;

	if (exp < 0) {
		negative = true;
		exp = -exp;
	}

	while (exp > 0) {
		if (exp & 1)
			result *= base;
		base *= base;
		exp >>= 1;
	}

	if (negative)
		return 1.0 / result;
	return result;
}
int myMin(int a, int b) {
	return (a < b) ? a : b;
}
int myMax(int a, int b) {
	return (a > b) ? a : b;
}