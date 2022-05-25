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
}