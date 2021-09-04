#include <stdio.h>
#include <string>
#include <time.h>
#include <windows.h>
#include "misc.cpp"
#include "zipper.cpp"
#define COMMAND_SETUP -666713357
#define COMMAND_START -1730724660
#define COMMAND_INFO -237985200
#define COMMAND_EXIT -248500869
#define COMMAND_DEBUG 1740352334
#define COMMAND_LOG -1731409

int intaction;
bool closeprogram = false;
FILE *pathfile;

int main() {
	system("title Pekka Zipper");
	remove("templog");
	tmplog = fopen("templog", "w+");
	if (tmplog == NULL) {
		printf("Unable to create temporary log file, the \"log\" command will not work\n");
		system("pause");
	} else {
		SetFileAttributes("templog", GetFileAttributes("templog") + FILE_ATTRIBUTE_HIDDEN);
	}
	pathfile = fopen("PekkaZipper.path", "r+");
	if (!pathfile) {
		consolelog("This seems to be your first time using this program\n");
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
		consolelog("Select command:\n"
		       "setup - Change the PK2 path\n"
		       "start - Create an episode .zip\n"
		       "log   - Save a .txt of all previous events\n"
		       "info  - Show basic information about the program\n"
		       "exit  - Close the program\n\nCurrent PK2 path: %s\n\n", path.c_str());
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
				try {
					startzipper();
				} catch (error newerror) {
					consolelog("\nAn error accoured!\nError: %s\nError code: %d\n", newerror.msg, newerror.code);
				}
				system("pause");
				break;
			}
			case COMMAND_LOG: {
				time_t t = time(NULL);
				time(&t);
				struct tm *ct = localtime(&t);
				std::string logname = "Pekka Zipper log " +
				std::to_string(ct->tm_mday) + " " +
				std::to_string(ct->tm_mon + 1) + " " +
				std::to_string(ct->tm_hour) + " " +
				std::to_string(ct->tm_min) + " " +
				std::to_string(ct->tm_sec) + ".txt";
				FILE *logfile = fopen(logname.c_str(), "w+");
				if (logfile == NULL) {
					consolelog("Something went wrong when creating the file..try again?\n");
				} else {
					fprintf(logfile, "%s", pkread(0, 2147483647, tmplog).c_str());
					fclose(logfile);
					consolelog("Saved log as \"%s\"\n", logname.c_str());
				}
				system("pause");
				break;
			}
			case COMMAND_INFO: {
				consolelog("PekkaZipper Version 8 Release Candidate\n"
				       "Created by Felix44\n"
					   "Github: https://github.com/Felix-44/Pekka-Zipper\n\n");
				system("pause");
				break;
			}
			case COMMAND_EXIT: {
				closeprogram = true;
				break;
			}
			case COMMAND_DEBUG: {
				consolelog("zip_t *: %x\n", episodezip);
				consolelog("sprites in queue:\n");
				for (auto i = begin(sprqueue); i != end(sprqueue); ++i) { 
    				consolelog("\"%s\"\n", (*i).c_str());
				}
				consolelog("saved sprites:\n");
				for (auto i = begin(addedsprites); i != end(addedsprites); ++i) { 
    				consolelog("\"%d\"\n", (*i));
				}
				system("pause");
				break;
			}
		}
		consolelog("\n");
	}
	fclose(tmplog);
	remove("templog");
	return 0;
}
