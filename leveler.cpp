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
		else {
			consolelog("Placeholder\n");
		}
		consolelog("\n\nControls")
	}
}