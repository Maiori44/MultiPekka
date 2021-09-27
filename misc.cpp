#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <stdarg.h>
#include <dirent.h>
#define CHOICE_YES -1926432
#define CHOICE_NO -14561
#define ERROR_FOLDERNOTFOUND 1
#define ERROR_CANTCREATEZIP 2
#define ERROR_ABORTDUEOVERWRITE 3
#define ERROR_FILENOTFOUND 4
#define ERROR_NOSPRITESFOUND 5
#define ERROR_INVALIDSPRITEVERSION 6
#define ERROR_CANTSTARTDUKTAPE 7

std::string path;
FILE *templog;

void consolelog(const char *text, ...) {
	va_list args;
	va_start(args, text);
	vfprintf(stdout, text, args);
	vfprintf(templog, text, args);
}

struct error {
  int code;
  const char *msg;
  error(const char *msg, int code) :
    msg(msg),
    code(code)
  {}
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

std::string pkread(int offset, int length, FILE *file) {
	fseek(file, offset, SEEK_SET);
	std::string result;
	for(int i = 1; i <= length; i++) {
		int byte = fgetc(file);
		if (byte == 0 or byte == EOF) break;
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
		throw error(errormsg.c_str(), ERROR_FOLDERNOTFOUND);
	}
	return directory;
}
