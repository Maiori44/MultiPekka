#include <stdio.h>
#include <string>
//#include <zip.h>
#include "misc.cpp"

bool close = false;
std::string path;
FILE *paths;

int main() {
	paths = fopen("PekkaZipper.path", "r+");
	while (!close) {
		system("cls");
		printf("Select action:\n"
		       "setup - change the PK2 path\n"
		       "start - create an episode .zip\n"
		       "info - show basic information about the program\n"
		       "exit - close the program\n\n");
		int intaction = getinput();
		printf("%d\n", intaction);
		system("pause");
	}
	return 0;
}
