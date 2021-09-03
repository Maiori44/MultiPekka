#include <dirent.h>
#include <sys/stat.h>
#include <unordered_set>
#include <zip.h>
#include <unistd.h>
#define ERROR_FOLDERNOTFOUND 1
#define ERROR_CANTCREATEZIP 2
#define ERROR_ABORTDUEOVERWRITE 3
#define ERROR_FILENOTFOUND 4

zip_t *episodezip;
std::unordered_set<std::string> sprites;

struct paths {
	std::string pk;
	std::string zip;
};

void throwerror(const char* errormsg, int errorcode) {
	error newerror;
	newerror.code = errorcode;
	newerror.msg = errormsg;
	throw newerror;
}

std::string pkread(int offset, int length, FILE *file) {
	fseek(file, offset, SEEK_SET);
	std::string result;
	for(int i = 1; i <= length; i++) {
		int byte = fgetc(file);
		if (byte == 0) break;
		char crt[2];
		sprintf(crt, "%c", byte);
		result.append(crt);
	}
	return result;
}

void memorizespr(std::string element) {
	if (sprites.find(element) == sprites.end()) {
		sprites.insert(element);
		printf("Memorized filename \"%s\" for later use [%d]\n", element.c_str(), sprites.size());
	}
}

bool addpkfile(const char *zippath, const char *filepath, const char *filename) {
	if (access(filepath, F_OK) != 0) return false;
	zip_source_t *source = zip_source_file(episodezip, filepath, 0, 0);
	std::string zipfilepath = zippath;
	zipfilepath.append(filename);
	zip_file_add(episodezip, zipfilepath.c_str(), source, 0);
	printf("Added file \"%s\" inside zip directory \"%s\"\n", filename, zippath);
	return true;
}

void findandadd(std::string filename, paths episodepath, const char *normalpath) {
	std::string episodefilepath;
	episodefilepath.append(episodepath.pk);
	episodefilepath.append("/");
	episodefilepath.append(filename);
	if (!addpkfile(episodepath.zip.c_str(), episodefilepath.c_str(), filename.c_str()) && !addpkfile(normalpath, std::string(path + "/" + normalpath + filename).c_str(), filename.c_str())) {
		throwerror(std::string("The file \"" + filename + "\" was not found").c_str(), ERROR_FILENOTFOUND);
	}
}

DIR *openpkdir(const char *path) {
	DIR *directory = opendir(path);
	if (directory == NULL) {
		std::string errormsg;
		errormsg.append("The folder \"");
		errormsg.append(path);
		errormsg.append("\" was not found");
		throwerror(errormsg.c_str(), ERROR_FOLDERNOTFOUND);
	}
	return directory;
}

int startzipper() {
	//reset the sprites set
	sprites.clear();
	//get episode name
	std::string episodename = getfullinput("Insert episode name:");
	//create zip file
	{
		printf("\nCreating zip file...\n");
		std::string zippath = episodename;
		zippath.append(".zip");
		//check if a zip with this name already exists
		{
			zip_t *testzip = zip_open(zippath.c_str(), 0, 0);
			if (testzip != NULL) {
				zip_discard(testzip);
				printf("\nWARNING: A zip named \"%s.zip\" already exists! Overwrite it? (YES/NO)\n\n", episodename.c_str());
				overwritechoice:
				int choice = getinput();
				switch(choice) {
					case CHOICE_YES:
						break;
					case CHOICE_NO: {
						throwerror("Intentionally aborted to avoid overwrite", ERROR_ABORTDUEOVERWRITE);
						break;
					}
					default: {
						printf("Invalid choice, valid choices are YES or NO\n");
						goto overwritechoice;
						break;
					}
				}
			}
		}
		episodezip = zip_open(zippath.c_str(), ZIP_CREATE|ZIP_TRUNCATE, 0);
		if (episodezip == NULL) throwerror("Failed to create the .zip file", ERROR_CANTCREATEZIP);
	}
	//create the folders
	zip_dir_add(episodezip, "episodes", 0);
	zip_dir_add(episodezip, "gfx", 0);
	zip_dir_add(episodezip, "gfx/tiles", 0);
	zip_dir_add(episodezip, "gfx/scenery", 0);
	zip_dir_add(episodezip, "music", 0);
	zip_dir_add(episodezip, "sprites", 0);
	//get the path for the episode folder and create it
	std::string zipepisodepath = "episodes/";
	zipepisodepath.append(episodename);
	zip_dir_add(episodezip, zipepisodepath.c_str(), 0);
	//get important paths
	paths episodepath;
	{
		std::string pkepisodepath;
		pkepisodepath.append(path);
		pkepisodepath.append("/episodes/");
		pkepisodepath.append(episodename);
		episodepath.pk = pkepisodepath;
		zipepisodepath.append("/");
		episodepath.zip = zipepisodepath;
	}
	//MAP LOOP - add the files for and from the .map files, and memorize the .spr files for the sprite loop
	{
		printf("Entering Map Loop...\n");
		//open the episode directory
		DIR *episodedir = openpkdir(episodepath.pk.c_str());
		struct dirent *entry;
		struct stat filestat;
		//iterate trough all the files
		while((entry = readdir(episodedir))) {
			stat(entry->d_name, &filestat);
        	if(!S_ISDIR(filestat.st_mode)) {
        		std::string filepath = episodepath.pk;
				filepath.append("/");
				filepath.append(entry->d_name);
        		FILE *mapfile = fopen(filepath.c_str(), "rb");
        		if (mapfile == NULL) continue;
        		std::string filename = entry->d_name;
        		if (filename.find(".map") != filename.npos) {
					//get and insert the map tileset, bg and music
					findandadd(pkread(0x5, 12, mapfile), episodepath, "gfx/tiles/");
					findandadd(pkread(0x12, 12, mapfile), episodepath, "gfx/scenery/");
					findandadd(pkread(0x1f, 12, mapfile), episodepath, "music/");
					//get and memorize the .spr files this map uses
					int numsprites = std::stoi(pkread(0xDC, 8, mapfile));
					if (numsprites > 0) {
						for(int i = 0; i < numsprites; i++) {
							memorizespr(pkread(0xE4 + (13 * i), 12, mapfile));
						}
					}
				}
				fclose(mapfile);
				//add the file to the zip
				addpkfile(episodepath.zip.c_str(), filepath.c_str(), entry->d_name);
			}
		}
		closedir(episodedir);
	}
	//SPRITE LOOP - iterate trough the sprites set, find their file, memorize the sprites they use, and add the sprite to the zip
	//save the zip
	printf("Saving the zip file, please wait...\n");
	zip_close(episodezip);
	printf("Successfully saved the zip, ");
	return 0;
}
