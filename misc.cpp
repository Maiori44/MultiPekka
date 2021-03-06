#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <stdarg.h>
#include <dirent.h>
#define CHOICE_YES -1926432
#define CHOICE_NO -14561
#define ARROW_RIGHT 77
#define ARROW_LEFT 75
#define CHAR_ARROW 224
#define CHAR_SEARCH 115
#define CHAR_FINDBROKEN 102
#define CHAR_LEVELSEARCH 108
#define CHAR_ESC 27

std::string path;
FILE *templog;
int vectorpos;

void consolelog(const char *text, ...) {
	va_list args;
	va_start(args, text);
	vfprintf(stdout, text, args);
	vfprintf(templog, text, args);
}

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
	fprintf(templog, "%s\n",action.c_str());
	return hash(action.c_str());
}

std::string getfullinput(const char *toprint) {
	consolelog("%s\n\n", toprint);
	std::string inputstring;
	std::getline(std::cin >> std::ws, inputstring);
	fprintf(templog, "%s\n", inputstring.c_str());
	return inputstring;
}

std::string cpkread(int offset, int length, int end, FILE *file) {
	fseek(file, offset, SEEK_SET);
	std::string result;
	for (int i = 1; i <= length; i++) {
		int byte = fgetc(file);
		if (byte == end or byte == EOF or byte == 0)
			break;
		char crt[2];
		sprintf(crt, "%c", byte);
		result.append(crt);
	}
	return result;
}

std::string pkread(int offset, int length, FILE *file) {
	fseek(file, offset, SEEK_SET);
	std::string result;
	for (int i = 1; i <= length; i++) {
		int byte = fgetc(file);
		if (byte == 0 or byte == EOF)
			break;
		char crt[2];
		sprintf(crt, "%c", byte);
		result.append(crt);
	}
	return result;
}

DIR *openpkdir(const char *path) {
	DIR *directory = opendir(path);
	if (directory == NULL) {
		std::string errormsg;
		errormsg.append("The folder \"");
		errormsg.append(path);
		errormsg.append("\" was not found");
		throw errormsg.c_str();
	}
	return directory;
}

const char *checkfile(std::string filename, const char *path2) {
	return access(std::string(path + path2 + filename).c_str(), F_OK) == 0
		? filename.c_str()
		: std::string(filename + "\t<MISSING!>").c_str();
}

const char *checksprfile(std::string filename) {
	return checkfile(filename, "/Sprites/");
}
