#include <zip.h>
#include <unordered_set>
#define ERROR_NOEPISODEFOLDER 1
#define ERROR_CANTCREATEZIP 2

zip_t *episodezip;

void episodeiterator(char *filename) {
	
}

int startzipper() {
	//get episode name
	std::string episodename = getfullinput("Insert episode name:");
	//create zip file
	{
		printf("\nCreating zip file...");
		std::string zippath = episodename;
		zippath.append(".zip");
		episodezip = zip_open(zippath.c_str(), ZIP_CREATE|ZIP_TRUNCATE, 0);
		if (episodezip == NULL) return ERROR_CANTCREATEZIP;
	}
	//create the episode folders
	{
		zip_dir_add(episodezip, "episodes", 0);
		std::string zipepisodepath = "episodes\\";
		zipepisodepath.append(episodename);
		zip_dir_add(episodezip, zipepisodepath.c_str(), 0);
	}
	//add map files and get other files' names from them
	{
		//get episode path
		std::string episodepath;
		episodepath.append(path);
		episodepath.append("\\episodes\\");
		episodepath.append(episodename);
		//get .map files
		printf("\nOpening folder %s...\n", episodepath.c_str());
		DIR *episode = opendir(episodepath.c_str());
		if (episode == NULL) return ERROR_NOEPISODEFOLDER;
		iteratefiles(episode, episodeiterator);
	}
	//close the zip
	zip_close(episodezip);
	return 0;
}
