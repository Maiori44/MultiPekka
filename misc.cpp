#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

std::string path;

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

std::string getfullinput(const char *toprint) {
	printf("%s\n\n", toprint);
	std::string inputstring;
	std::getline(std::cin >> std::ws, inputstring);
	return inputstring;
}

void senderror(const char* errormsg, int errornum) {
	printf("An error accoured!\nError: %s\nError code: %d\n", errormsg, errornum);
}

void iteratefiles(DIR *directory, void (*f)(char *)) {
	struct dirent *entry;
	while((entry=readdir(directory))) {
		(*f)(entry->d_name);
	}
}
