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
#define ERROR_CANTOPENFILE 5

std::string path;
FILE *tmplog;

void consolelog(const char *text, ...) {
	va_list args;
	va_start(args, text);
	vfprintf(stdout, text, args);
	vfprintf(tmplog, text, args);
	//fflush(stdout);
}

struct error {
  int code;
  const char *msg;
};

void throwerror(const char* errormsg, int errorcode) {
	error newerror;
	newerror.code = errorcode;
	newerror.msg = errormsg;
	throw newerror;
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
		throwerror(errormsg.c_str(), ERROR_FOLDERNOTFOUND);
	}
	return directory;
}
