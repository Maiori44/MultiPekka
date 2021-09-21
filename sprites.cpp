#include <vector>
#include <conio.h>
#include <stdint.h>
#define CHAR_ARROW 224
#define CHAR_ESC 27
#define ARROW_RIGHT 77
#define ARROW_LEFT 75

std::vector<std::string> sprfiles;
int vectorpos;

const char *checkfile(std::string filename) {
	return access(std::string(path + "/Sprites/" + filename).c_str(), F_OK) == 0 ?
	       filename.c_str() :
	       std::string("\x1B[41m" + filename + "\x1B[40m").c_str();
}

void startspriter() {
	//reset the vector
	sprfiles.clear();
	vectorpos = 0;
	//get all sprite names
	{
		consolelog("Locating all sprite files...\n");
		DIR *spritesdir = openpkdir(std::string(path + "/Sprites").c_str());
		struct dirent *sprentry;
		while ((sprentry = readdir(spritesdir))) {
			std::string sprfilename = sprentry->d_name;
			if (sprfilename.find(".spr") != sprfilename.npos) {
				sprfiles.insert(sprfiles.end(), sprfilename);
				consolelog("Found sprite %s\n", sprfilename.c_str());
			}
		}
	}
	//enter the loop
	while (true) {
		std::string filename = sprfiles[vectorpos];
		FILE *sprfile = fopen(std::string(path + "/Sprites/" + filename).c_str(), "rb");
		if (sprfile == NULL) {
			throwerror(std::string("Could not open the file \"" + filename + "\"").c_str(), ERROR_CANTOPENFILE);
		}
		system("cls");
		consolelog("File:\t\t\t%s [%d/%d]\n", filename.c_str(), vectorpos + 1, static_cast<int>(sprfiles.size()));
		if (pkread(0x0, 3, sprfile) != "1.3") {
			consolelog("\nInvalid sprite version! only version 1.3 is supported.");
		} else {
			consolelog("Name:\t\t\t%s\n", pkread(0x44C, 32, sprfile).c_str());
			consolelog("Image:\t\t\t%s\n\n- Sounds -\n", checkfile(pkread(0x8, 12, sprfile)));
			consolelog("Damage Sound:\t\t%s\n", checkfile(pkread(0x6C, 12, sprfile)));
			consolelog("KO Sound:\t\t%s\n", checkfile(pkread(0xD0, 12, sprfile)));
			consolelog("Attack 1 Sound:\t\t%s\n", checkfile(pkread(0x134, 12, sprfile)));
			consolelog("Attack 2 Sound:\t\t%s\n", checkfile(pkread(0x198, 12, sprfile)));
			consolelog("Random Sound:\t\t%s\n\n- Sprites -\n", checkfile(pkread(0x1FC, 12, sprfile)));
			consolelog("Transformation Sprite:\t%s\n", checkfile(pkread(0x4E0, 12, sprfile)));
			consolelog("Bonus Sprite:\t\t%s\n", checkfile(pkread(0x544, 12, sprfile)));
			consolelog("Attack 1 Sprite:\t%s\n", checkfile(pkread(0x5A8, 12, sprfile)));
			consolelog("Attack 2 Sprite:\t%s\n", checkfile(pkread(0x60C, 12, sprfile)));
		}
		consolelog("\n\n(names in red are for missing files)\n\n"
		           "Controls:\n"
				   "[<-]\t- load previous sprite\n"
				   "[->]\t- load next sprite\n"
				   "[S]\t- search for a specific sprite\n"
				   "[ESC]\t- end operation\n\n");
		fclose(sprfile);
		input:
		switch (getch()) {
			case CHAR_ESC:
				return;
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
