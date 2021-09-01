#include <dirent.h>
#include <unordered_set>
#include <zip.h>
#define ERROR_FOLDERNOTFOUND 1
#define ERROR_CANTCREATEZIP 2
#define ERROR_ABORTDUEOVERWRITE 3

zip_t *episodezip;

void throwerror(const char* errormsg, int errorcode) {
	error newerror;
	newerror.code = errorcode;
	newerror.msg = errormsg;
	throw newerror;
}

void iteratefiles(const char *zipdirpath, const char *directorypath, bool (*iterator)(char *, FILE *)) {
	printf("\nOpening folder %s...\n", directorypath);
	DIR *directory = opendir(directorypath);
	if (directory == NULL) {
			std::string errormsg;
			errormsg.append("No folder \"");
			errormsg.append(directorypath);
			errormsg.append("\" was found");
			throwerror(errormsg.c_str(), ERROR_FOLDERNOTFOUND);
	}
	struct dirent *entry;
	while((entry=readdir(directory))) {
		std::string filepath = directorypath;
		filepath.append("/");
		filepath.append(entry->d_name);
		FILE *file = fopen(filepath.c_str(), "rb");
		if (file != NULL) {
			bool check = (*iterator)(entry->d_name, file);
			if (check) {
				zip_source_t *source = zip_source_file(episodezip, filepath.c_str(), 0, 0);
				std::string zipfilepath = zipdirpath;
				zipfilepath.append(entry->d_name);
				zip_file_add(episodezip, zipfilepath.c_str(), source, 0);
				printf("Added file \"%s\" inside zip directory \"%s\"\n", entry->d_name, zipdirpath);
			}
		}
	}
}

bool episodeiterator(char *filename, FILE *file) {
	return true;
}

int startzipper() {
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
					case CHOICE_YES: break;
					case CHOICE_NO: {
						throwerror("Intentially aborted to avoid overwrite", ERROR_ABORTDUEOVERWRITE);
						break;
					}
					default: {
						printf("Invalid choice, valid: YES/NO\n");
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
	//add map files and get other files' names from them
	{
		//get episode path
		std::string episodepath;
		episodepath.append(path);
		episodepath.append("/episodes/");
		episodepath.append(episodename);
		//iterate in the directory
		zipepisodepath.append("/");
		iteratefiles(zipepisodepath.c_str(), episodepath.c_str(), episodeiterator);
	}
	//close the zip
	zip_close(episodezip);
	return 0;
}
