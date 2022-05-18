#include <vector>
#include <conio.h>
#include <stdint.h>
#define ARROW_RIGHT 77
#define ARROW_LEFT 75
#define CHAR_ARROW 224
#define CHAR_SEARCH 115
#define CHAR_FINDBROKEN 102
#define CHAR_LEVELSEARCH 108
#define CHAR_ESC 27

std::vector<std::string> sprfiles;
int vectorpos;

const char *checkfile(std::string filename) {
	return access(std::string(path + "/Sprites/" + filename).c_str(), F_OK) == 0 ?
		filename.c_str() :
		std::string(filename + "\t<MISSING!>").c_str();
}

void startspriter() {
	//reset the vector
	sprfiles.clear();
	vectorpos = 0;
	//get all sprite names
	{
		consolelog("Locating all sprite files...\n");
		DIR *spritesdir = openpkdir(std::string(path + "/sprites").c_str());
		struct dirent *sprentry;
		while ((sprentry = readdir(spritesdir))) {
			std::string sprfilename = sprentry->d_name;
			if (sprfilename.find(".spr") != sprfilename.npos) {
				sprfiles.insert(sprfiles.end(), sprfilename);
				consolelog("Found sprite %s\n", sprfilename.c_str());
			}
		}
	}
	//check if there were any sprites found
	if (sprfiles.empty()) {
		throw error("No sprites found!", ERROR_NOSPRITESFOUND);
	}
	//enter the loop
	while (true) {
		std::string filename = sprfiles[vectorpos];
		FILE *sprfile = fopen(std::string(path + "/sprites/" + filename).c_str(), "rb");
		system("cls");
		consolelog("File:\t\t\t%s [%d/%d]\n", filename.c_str(), vectorpos + 1, static_cast<int>(sprfiles.size()));
		if (sprfile == NULL) {
			consolelog("\nFailed to open file!");
		} else if (pkread(0x0, 3, sprfile) != "1.3") { //check if the sprite is in the correct verion
			consolelog("\nInvalid sprite version! only version 1.3 is supported.");
			fclose(sprfile);
		} else {
			//write all the sprite's info
			consolelog("Name:\t\t\t%s\n", pkread(0x44C, 32, sprfile).c_str());
			consolelog("Image:\t\t\t%s\n\n- Sounds -\n", checkfile(pkread(0x8, 32, sprfile)));
			consolelog("Damage Sound:\t\t%s\n", checkfile(pkread(0x6C, 32, sprfile)));
			consolelog("KO Sound:\t\t%s\n", checkfile(pkread(0xD0, 32, sprfile)));
			consolelog("Attack 1 Sound:\t\t%s\n", checkfile(pkread(0x134, 32, sprfile)));
			consolelog("Attack 2 Sound:\t\t%s\n", checkfile(pkread(0x198, 32, sprfile)));
			consolelog("Random Sound:\t\t%s\n\n- Sprites -\n", checkfile(pkread(0x1FC, 32, sprfile)));
			consolelog("Transformation Sprite:\t%s\n", checkfile(pkread(0x4E0, 32, sprfile)));
			consolelog("Bonus Sprite:\t\t%s\n", checkfile(pkread(0x544, 32, sprfile)));
			consolelog("Attack 1 Sprite:\t%s\n", checkfile(pkread(0x5A8, 32, sprfile)));
			consolelog("Attack 2 Sprite:\t%s\n", checkfile(pkread(0x60C, 32, sprfile)));
			fclose(sprfile);
		}
		consolelog("\n\nControls:\n"
				"[<-]\t- load previous sprite\n"
				"[->]\t- load next sprite\n"
				"[S]\t- search for a specific sprite\n"
				"[F]\t- find a broken sprite\n"
				"[L]\t- show a list of all levels that use this sprite\n"
				"[ESC]\t- end operation\n\n");
		input:
		//get an input and do accordingly
		switch (getch()) {
			case CHAR_ESC:
				return;
			case CHAR_SEARCH: {
				std::vector<std::string>::iterator pos = std::find(sprfiles.begin(), sprfiles.end(), getfullinput("Insert file name:"));
				if (pos == sprfiles.end()) {
					consolelog("\nThe file was not found\n");
					system("pause");
					break;
				}
				vectorpos = std::distance(sprfiles.begin(), pos);
				break;
			}
			case CHAR_FINDBROKEN: {
				consolelog("Searching for broken files...\n");
				std::string check;
				bool found = false;
				for (int i = 0; i < static_cast<int>(sprfiles.size()); i++) {
					FILE *filetocheck = fopen(std::string(path + "/sprites/" + sprfiles[i]).c_str(), "rb");
					if (filetocheck == NULL) {
						throw error(("Failed to open file \"" + sprfiles[i] + "\"").c_str(), ERROR_FILENOTFOUND);
					} else if (pkread(0x0, 3, filetocheck) != "1.3") continue;
					check = std::string(checkfile(pkread(0x8, 32, filetocheck))) +
							checkfile(pkread(0x6C, 32, filetocheck)) +
							checkfile(pkread(0xD0, 32, filetocheck)) +
							checkfile(pkread(0x134, 32, filetocheck)) +
							checkfile(pkread(0x198, 32, filetocheck)) +
							checkfile(pkread(0x1FC, 32, filetocheck)) +
							checkfile(pkread(0x4E0, 32, filetocheck)) +
							checkfile(pkread(0x544, 32, filetocheck)) +
							checkfile(pkread(0x5A8, 32, filetocheck)) +
							checkfile(pkread(0x60C, 32, filetocheck));
					fclose(filetocheck);
					if (check.find("<MISSING!>") != check.npos) {
						found = true;
						vectorpos = i;
						break;
					}
					printf("Checked %d files\r", i + 1);
				}
				if (!found) {
					consolelog("No broken sprites found\n");
					system("pause");
				}
				break;
			}
			case CHAR_LEVELSEARCH: {
				DIR *episodesdir = openpkdir(std::string(path + "/Episodes/").c_str());
				DIR *episodedir;
				struct dirent *epsentry;
				struct dirent *epentry;
				struct stat stats;
				FILE *mapfile;
				int numsprites;
				while ((epsentry = readdir(episodesdir))) {
					stat(epsentry->d_name, &stats);
					if (S_ISDIR(stats.st_mode)) continue;
					consolelog("In episode %s:\n", epsentry->d_name);
					episodedir = openpkdir(std::string(path + "/Episodes/" + epsentry->d_name).c_str());
					while ((epentry = readdir(episodedir))) {
						std::string filename = epentry->d_name;
						if (filename.find(".map") == filename.npos) continue;
						mapfile = fopen(std::string(path + "/Episodes/" + epsentry->d_name + "/" + filename).c_str(), "rb");
						if (mapfile == NULL) continue;
						numsprites = std::stoi(pkread(0xDC, 8, mapfile));
						if (numsprites <= 0) continue;
						for(int i = 0; i < numsprites; i++) {
							if (pkread(0xE4 + (13 * i), 12, mapfile) == sprfiles[vectorpos]) {
								consolelog("\t%s\n", filename.c_str());
								break;
							}
						}
					}
					consolelog("\n");
				}
				system("pause");
				break;
			}
			case CHAR_ARROW: {
				switch (getch()) {
					case ARROW_RIGHT: {
						vectorpos = filename == sprfiles.back() ? 0 : vectorpos + 1;
						break;
					}
					case ARROW_LEFT: {
						vectorpos = filename == sprfiles[0] ? static_cast<int>(sprfiles.size()) - 1 : vectorpos - 1;
						break;
					}
					default:
						goto input;
				}
				break;
			}
			default:
				goto input;
		}
	}
}
