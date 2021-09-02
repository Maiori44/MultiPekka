#include <dirent.h>
#include <sys/stat.h>
#include <unordered_set>
#include <zip.h>
#define ERROR_FOLDERNOTFOUND 1
#define ERROR_CANTCREATEZIP 2
#define ERROR_ABORTDUEOVERWRITE 3
#define ERROR_FILENOTFOUND 4

zip_t *episodezip;
std::unordered_set<std::string> sprites;

void throwerror(const char* errormsg, int errorcode) {
	error newerror;
	newerror.code = errorcode;
	newerror.msg = errormsg;
	throw newerror;
}

std::string pkread(int offset, int origin, int stopbyte, FILE *file) {
	fseek(file, offset, origin);
	std::string result;
	for(int i = 1; i <= 12; i++) { //it seems that every filename found in files doesn't use more than 12 characters, not including the \0 at the end
		int byte = fgetc(file);
		if (byte == stopbyte) break;
		char crt[2];
		sprintf(crt, "%c", byte);
		result.append(crt);
	}
	return result;
}

/*void insertonce(std::unordered_set<std::string> *set, std::string element) {
	if (set->find(element) == set->end()) {
		set->insert(element);
		printf("Memorized filename \"%s\" for later use [%d]\n", element.c_str(), set->size());
	}
}*/

void addpkfile(std::string zippath, const char *filepath, const char *filename) {
	zip_source_t *source = zip_source_file(episodezip, filepath, 0, 0);
	std::string zipfilepath = zippath;
	zipfilepath.append(filename);
	zip_file_add(episodezip, zipfilepath.c_str(), source, 0);
	printf("Added file \"%s\" inside zip directory \"%s\"\n", filename, zippath.c_str());
}

void //FUNCTION TO GET FILE FROM EPISODE FOLDER AS WELL

DIR *openpkdir(const char * path) {
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
		printf("\nCreating zip file...");
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
	//add the files from the map files
	{
		//get paths
		std::string episodepath;
		episodepath.append(path);
		episodepath.append("/episodes/");
		episodepath.append(episodename);
		zipepisodepath.append("/");
		//open the episode directory
		DIR *episodedir = openpkdir(episodepath.c_str());
		struct dirent *entry;
		struct stat filestat;
		//iterate trough all the files
		while((entry = readdir(episodedir))) {
			stat(entry->d_name, &filestat);
        	if(!S_ISDIR(filestat.st_mode)) {
        		std::string filepath = episodepath.c_str();
				filepath.append("/");
				filepath.append(entry->d_name);
        		FILE *mapfile = fopen(filepath.c_str(), "rb");
        		if (mapfile == NULL) continue;
        		std::string filename = entry->d_name;
        		if (filename.find(".map") != filename.npos) {
					//get and insert all files this map uses
					std::string tilesetname = pkread(5, SEEK_SET, 0, mapfile);
					addpkfile("gfx/tiles/", std::string(path + "/gfx/tiles/" + tilesetname).c_str(), tilesetname.c_str());
					std::string bgname = pkread(18, SEEK_SET, 0, mapfile);
					addpkfile("gfx/scenery/", std::string(path + "/gfx/scenery/" + bgname).c_str(), bgname.c_str());
					std::string musicname = pkread(31, SEEK_SET, 0, mapfile);
					addpkfile("music/", std::string(path + "music/" + musicname).c_str(), musicname.c_str());
				}
				fclose(mapfile);
				//add the file to the zip
				addpkfile(zipepisodepath, filepath.c_str(), entry->d_name);
			}
		}
		closedir(episodedir);
	}
	//save the zip
	printf("Saving the zip file, please wait...\n");
	zip_close(episodezip);
	printf("Successfully saved the zip, ");
	return 0;
}
