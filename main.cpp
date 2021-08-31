#include <stdio.h>
#include <string>
#include <zip.h>
#include <unordered_set>
#include "misc.cpp"
#define OPTION_SETUP -666713357
#define OPTION_START -1730724660
#define OPTION_INFO -237985200
#define OPTION_EXIT -248500869

int intaction;
bool close = false;
std::string path;
FILE *pathfile;

int main() {
	pathfile = fopen("PekkaZipper.path", "r+");
	if (!pathfile) {
		printf("This seems to be your first time using this program\nI need to ask you something before we continue\n");
		goto setup;
	}
	{
		char oldpath[500];
		fgets(oldpath, 500, pathfile);
		path = oldpath;
	}
	fclose(pathfile);
	while (!close) {
		system("cls");
		printf("Select command:\n"
		       "setup - change the PK2 path\n"
		       "start - create an episode .zip\n"
		       "info - show basic information about the program\n"
		       "exit - close the program\n\nCurrent PK2 path: %s\n\n", path.c_str());
		intaction = getinput();
		switch(intaction) {
			case OPTION_SETUP: {
				setup:
				printf("Insert path to your Pekka Kana 2 folder:\n\n");
				std::string newpath;
				std::getline(std::cin >> std::ws, newpath);
				if (pathfile && newpath == "cancel") break;
				path = newpath;
				pathfile = fopen("PekkaZipper.path", "w+");
				fprintf(pathfile, "%s", path.c_str());
				fclose(pathfile);
				break;
			}
			case OPTION_START: {
				break;
			}
			case OPTION_INFO: {
				printf("PekkaZipper Version 2 Indev\n"
				       "Created by Felix44\n"
					   "Github: https://github.com/Felix-44/Pekka-Zipper\n\n");
				system("pause");
				break;
			}
			case OPTION_EXIT: {
				close = true;
				break;
			}
		}
		printf("\n");
	}
	return 0;
}
