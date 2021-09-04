#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <stdarg.h>
#include <stdarg.h>
#define CHOICE_YES -1926432
#define CHOICE_NO -14561

std::string path;
FILE *tmplog;

void consolelog(const char *text, ...) {
	va_list args;
	va_start(args, text);
	vfprintf(stdout, text, args);
	vfprintf(tmplog, text, args);
}

struct error {
  int code;
  const char *msg;
};

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

uint64_t getinput() {
	std::string action;
	std::cin >> action;
	transform(action.begin(), action.end(), action.begin(), asciitolower);
	fprintf(tmplog, "%s\n",action.c_str());
	return hash(action.c_str());
}

std::string getfullinput(const char *toprint) {
	printf("%s\n\n", toprint);
	std::string inputstring;
	std::getline(std::cin >> std::ws, inputstring);
	fprintf(tmplog, "%s\n", inputstring.c_str());
	return inputstring;
}
