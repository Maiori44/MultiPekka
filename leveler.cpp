std::vec<std::string> mapfiles;

void startleveler() {
    mapfiles.clear();
    vectorpos = 0;
    std::string episodepath;
    {
        std::string episodename = getfullinput("Insert episode name:");
        pkepisodepath.append(path);
		pkepisodepath.append("/episodes/");
		pkepisodepath.append(episodename);
    }
}