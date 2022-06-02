std::vector<std::string> mapfiles;

void startleveler() {
	mapfiles.clear();
	vectorpos = 0;
	std::string episodepath;
	{
		std::string episodename = getfullinput("Insert episode name:");
		episodepath.append(path);
		episodepath.append("/episodes/");
		episodepath.append(episodename);
	}
	{
		consolelog("Locating all map files in the episode %s...", episodepath.c_str());
		DIR *episodedir = openpkdir(episodepath.c_str());
		struct dirent *mapentry;
		while ((mapentry = readdir(episodedir))) {
			std::string mapfilename = mapentry->d_name;
			if (mapfilename.find(".map") != mapfilename.npos) {
				mapfiles.insert(mapfiles.end(), mapfilename);
				consolelog("Found map %s\n", mapfilename.c_str());
			}
		}
	}
	if (mapfiles.empty())
		throw "No maps found!";
	while (true) {
		std::string filename = mapfiles[vectorpos];
		FILE *mapfile = fopen(std::string(episodepath + "/" + filename).c_str(), "rb");
		system("cls");
		consolelog("File:\t\t\t%s [%d/%d]\n", filename.c_str(), vectorpos + 1, static_cast<int>(mapfiles.size()));
		if (mapfile == NULL)
			consolelog("\nFailed to open file!");
		else if (pkread(0x0, 3, mapfile) != "1.3") {
			consolelog("\nInvalid map version! only version 1.3 is supported.");
			fclose(mapfile);
		} else {
			consolelog("Name:\t\t\t%s\n", cpkread(0x2C, 40, 0xCD, mapfile).c_str());
			consolelog("Author:\t\t\t%s\n", cpkread(0x54, 32, 0xCC, mapfile).c_str());
			consolelog("Tileset:\t\t%s\n", checkfile(pkread(0x5, 12, mapfile), "/gfx/tiles/"));
			consolelog("Background:\t\t%s\n", checkfile(pkread(0x12, 12, mapfile), "/gfx/scenery/"));
			consolelog("Theme:\t\t\t%s\n\n", checkfile(pkread(0x1F, 12, mapfile), "/music/"));
			int numsprites = std::stoi(pkread(0xDC, 8, mapfile));
			if (numsprites > 0) {
				consolelog("- Sprites (%d) -\n", numsprites);
				for (int i = 0; i < numsprites; i++)
					consolelog("%s\n", checksprfile(pkread(0xE4 + (13 * i), 12, mapfile)));
			} else
				consolelog("No sprites used by this map.");

		}
		consolelog("\n\nControls:\n"
				"[<-]\t- Load previous map\n"
				"[->]\t- Load next map\n"
				"[F]\t- Find a broken map\n"
				"[ESC]\t- End operation\n\n");
		input:
		switch (getch()) {
			case CHAR_ESC:
				return;
			case CHAR_FINDBROKEN: {
				consolelog("Searching for broken maps...\n");
				std::string check;
				bool found = false;
				for (int i = 0; i < static_cast<int>(mapfiles.size()); i++) {
					FILE *filetocheck = fopen(std::string(episodepath + "/" + mapfiles[i]).c_str(), "rb");
					if (filetocheck == NULL) {
						throw ("Failed to open file \"" + mapfiles[i] + "\"").c_str();
					} else if (pkread(0x0, 3, filetocheck) != "1.3") continue;
					check = std::string(
						checkfile(pkread(0x5, 12, filetocheck), "/gfx/tiles/")) +
						checkfile(pkread(0x12, 12, filetocheck), "/gfx/scenery/") +
						checkfile(pkread(0x1F, 12, filetocheck), "/music/");
					int numsprites = std::stoi(pkread(0xDC, 8, filetocheck));
					if (numsprites > 0)
						for (int i = 0; i < numsprites; i++)
							check += checksprfile(pkread(0xE4 + (13 * i), 12, filetocheck));
					fclose(filetocheck);
					if (check.find("<MISSING!>") != check.npos) {
						found = true;
						vectorpos = i;
						break;
					}
					printf("Checked %d files\r", i + 1);
				}
				if (!found) {
					consolelog("No broken maps found\n");
					system("pause");
				}
				break;
			}
			case CHAR_ARROW: {
				switch (getch()) {
					case ARROW_RIGHT: {
						vectorpos = filename == mapfiles.back() ? 0 : vectorpos + 1;
						break;
					}
					case ARROW_LEFT: {
						vectorpos = filename == mapfiles[0] ? static_cast<int>(mapfiles.size()) - 1 : vectorpos - 1;
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