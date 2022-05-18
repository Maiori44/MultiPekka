#include <stdio.h>
#include <string>
#include <time.h>
#include <windows.h>
#include "misc.cpp"
#include "zipper.cpp"
#include "sprites.cpp"
#define COMMAND_SETUP -666713357
#define COMMAND_ZIP -1877840
#define COMMAND_SPRITES -468740822
#define COMMAND_ONLINE -6603339
#define COMMAND_LOG -1731409
#define COMMAND_INFO -237985200
#define COMMAND_EXIT -248500869
#define COMMAND_DEBUG 1740352334
#define PCALL(function) try {function();} catch(error thrownerror) {consolelog("\nAn error accoured!\nError: %s\nError code: %d\n%s", thrownerror.msg, thrownerror.code, debugmsg != "" ? std::string("File: " + debugmsg + " (this error seems to have been caused by this file, checking it might help you fix the issue)\n").c_str() : ""); system("pause");}

int intaction;
FILE *pathfile;

int main() {
	system("title MultiPekka");
	system("color 0f");
	remove("templog");
	templog = fopen("templog", "w+");
	if (templog == NULL) {
		printf("Unable to create temporary log file, the \"log\" command will not work\n");
		system("pause");
	} else {
		SetFileAttributes("templog", GetFileAttributes("templog") + FILE_ATTRIBUTE_HIDDEN);
	}
	pathfile = fopen("MultiPekka.path", "r+");
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
	while (true) {
		system("cls");
		consolelog("Select command:\n"
			"setup\t- Change the PK2 path\n"
			"zip\t- Create an episode .zip\n"
			"sprites\t- Display all information about a .spr file\n"
			//"online\t- Show all mapstore related commands\n"
			"log\t- Save a .txt of all previous events\n"
			"info\t- Show basic information about the program\n"
			"exit\t- Close the program\n\nCurrent PK2 path: %s\n\n", path.c_str());
		intaction = getinput();
		switch(intaction) {
			case COMMAND_SETUP: {
				setup:
				std::string newpath = getfullinput(pathfile ? "Insert the path to your Pekka Kana 2 folder:\n(or type \"cancel\" to cancel this opertion)" : "Insert the path to your Pekka Kana 2 folder:");
				if (pathfile && newpath == "cancel") break;
				path = newpath;
				pathfile = fopen("MultiPekka.path", "w+");
				fprintf(pathfile, "%s", path.c_str());
				fclose(pathfile);
				break;
			}
			case COMMAND_ZIP: {
				PCALL(startzipper);
				break;
			}
			case COMMAND_SPRITES: {
				PCALL(startspriter);
				break;
			}
			case COMMAND_ONLINE: {
				/*PCALL([=]{
					if (access("mapstore/node.exe", F_OK) != 0) throw error("The program node.exe is missing", ERROR_MISSINGFILEFORNODEJS);
					if (access("mapstore/mapstore.js", F_OK) != 0) throw error("Missing mapstore.js file", ERROR_MISSINGFILEFORNODEJS);
					system("cls");
					system(".\\mapstore\\node.exe mapstore\\mapstore.js");
					system("color 0f");	
				})*/
				consolelog("\nYou'll have to wait for this\n");
				system("pause");
				break;
			}
			case COMMAND_LOG: {
				time_t t = time(NULL);
				time(&t);
				struct tm *ct = localtime(&t);
				std::string logname = "MultiPekka log " +
				std::to_string(ct->tm_mday) + " " +
				std::to_string(ct->tm_mon + 1) + " " +
				std::to_string(ct->tm_hour) + " " +
				std::to_string(ct->tm_min) + " " +
				std::to_string(ct->tm_sec) + ".txt";
				FILE *logfile = fopen(logname.c_str(), "w+");
				if (logfile == NULL) {
					consolelog("Something went wrong when creating the file..try again?\n");
				} else {
					fprintf(logfile, "%s", pkread(0, 2147483647, templog).c_str());
					fclose(logfile);
					consolelog("Saved log as \"%s\"\n", logname.c_str());
				}
				system("pause");
				break;
			}
			case COMMAND_INFO: {
				consolelog("MultiPekka v1.5 UNFINISHED\n"
					"Created by Maiori\n"
					"Github: https://github.com/Felix-44/MultiPekka\n\n");
				system("pause");
				break;
			}
			case COMMAND_EXIT: {
				fclose(templog);
				remove("templog");
				return 0;
			}
			case COMMAND_DEBUG: {
				printf("%d\n", getinput());
				system("pause");
				break;
			}
		}
		consolelog("\n");
	}
}
