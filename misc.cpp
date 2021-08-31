#include <stdint.h>
#include <iostream>
#include <string>
#include <algorithm>
#define OPTION_SETUP -666713357
#define OPTION_START -1730724660
#define OPTION_INFO -237985200
#define OPTION_EXIT -248500869

char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

uint64_t mix(char m, uint64_t s) {
	return ((s<<7) + ~(s>>3)) + ~m;
}

uint64_t hash(const char * m) {
	return (*m) ? mix(*m,hash(m+1)) : 0;
}

int getinput() {
	std::string action;
	std::cin >> action;
	transform(action.begin(), action.end(), action.begin(), asciitolower);
	return hash(action.c_str());
}
