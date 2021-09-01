#include <stdio.h>
#include <string>
#include "misc.cpp"
#include "zipper.cpp"
#define COMMAND_SETUP -666713357
#define COMMAND_START -1730724660
#define COMMAND_INFO -237985200
#define COMMAND_EXIT -248500869

int intaction;
bool closeprogram = false;
FILE *pathfile;

int main() {
	pathfile = fopen("PekkaZipper.path", "r+");
	if (!pathfile) {
		printf("This seems to be your first time using this program\n");
		goto setup;
	}
	{
		char oldpath[500];
		fgets(oldpath, 500, pathfile);
		path = oldpath;
	}
	fclose(pathfile);
	while (!closeprogram) {
		system("cls");
		printf("Select command:\n"
		       "setup - change the PK2 path\n"
		       "start - create an episode .zip\n"
		       "info - show basic information about the program\n"
		       "exit - close the program\n\nCurrent PK2 path: %s\n\n", path.c_str());
		intaction = getinput();
		switch(intaction) {
			case COMMAND_SETUP: {
				setup:
				std::string newpath = getfullinput(pathfile ? "Insert the path to your Pekka Kana 2 folder:\n(or type \"cancel\" to cancel this opertion)" : "Insert the path to your Pekka Kana 2 folder:");
				if (pathfile && newpath == "cancel") break;
				path = newpath;
				pathfile = fopen("PekkaZipper.path", "w+");
				fprintf(pathfile, "%s", path.c_str());
				fclose(pathfile);
				break;
			}
			case COMMAND_START: {
				switch(startzipper()) {
					case ERROR_NOEPISODEFOLDER: {
						senderror("The episode folder was not found", ERROR_NOEPISODEFOLDER);
						break;
					}
					case ERROR_CANTCREATEZIP: {
						senderror("Failed to create the .zip file", ERROR_CANTCREATEZIP);
						break;
					}
					default: {
						printf("The episode was successfully zipped\n");
						break;
					}
				}
				system("pause");
				break;
			}
			case COMMAND_INFO: {
				printf("PekkaZipper Version 3 Indev\n"
				       "Created by Felix44\n"
					   "Github: https://github.com/Felix-44/Pekka-Zipper\n\n");
				system("pause");
				break;
			}
			case COMMAND_EXIT: {
				closeprogram = true;
				break;
			}
		}
		printf("\n");
	}
	return 0;
}
