#include <dirent.h>
#include <sys/stat.h>
#include <unordered_set>
#include <zip.h>
#include <unistd.h>
#define ERROR_FOLDERNOTFOUND 1
#define ERROR_CANTCREATEZIP 2
#define ERROR_ABORTDUEOVERWRITE 3
#define ERROR_FILENOTFOUND 4
#define ERROR_CANTOPENFILE 5

zip_t *episodezip;
std::unordered_set<std::string> sprqueue;
std::unordered_set<int> addedsprites;
std::string debugmsg;
//int progressbar, maxbar;

/*void displaybar() {
	const int currentprg = 100 * progressbar / maxbar;
	printf("[");
	for(int i = 1; i <= 20; i++)
		printf("%s", i * 5 <= currentprg ? "#" : " ");
	printf("] %d%% done\n", currentprg);
}
commented out because it slows down too much
void updatebar() {
	progressbar++;
	displaybar();
}*/

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

void memorizespr(std::string element) {
	if (element == "" || addedsprites.find(hash(element.c_str())) != addedsprites.end() ){
		//maxbar--;
		return;	
	}
	if (sprqueue.find(element) == sprqueue.end()) {
		sprqueue.insert(element);
		addedsprites.insert(hash(element.c_str()));
		//maxbar += 11;
		consolelog("Memorized filename \"%s\" for later use [%d]\n", element.c_str(), sprqueue.size());
		//displaybar();
	}
}

bool addpkfile(const char *zippath, const char *filepath, const char *filename) {
	if (access(filepath, F_OK) != 0) return false;
	zip_source_t *source = zip_source_file(episodezip, filepath, 0, 0);
	std::string zipfilepath = zippath;
	zipfilepath.append(filename);
	zip_file_add(episodezip, zipfilepath.c_str(), source, 0);
	consolelog("Added file \"%s\" inside zip directory \"%s\"\n", filename, zippath);
	//updatebar();
	return true;
}

void findandadd(std::string filename, paths episodepath, const char *normalpath) {
	if (filename == "") return;
	std::string episodefilepath;
	episodefilepath.append(episodepath.pk);
	episodefilepath.append("/");
	episodefilepath.append(filename);
	if (!addpkfile(episodepath.zip.c_str(), episodefilepath.c_str(), filename.c_str()) && !addpkfile(normalpath, std::string(path + "/" + normalpath + filename).c_str(), filename.c_str())) {
		throwerror(std::string("The file \"" + filename + "\" was not found").c_str(), ERROR_FILENOTFOUND);
	}
}

void startzipper() {
	/*//reset the progress bar
	progressbar = 0;
	maxbar = 3;*/
	//reset the sprites sets
	sprqueue.clear();
	addedsprites.clear();
	//get episode name
	std::string episodename = getfullinput("Insert episode name:");
	//create zip file
	{
		consolelog("\nCreating zip file...\n");
		std::string zippath = episodename;
		zippath.append(".zip");
		//check if a zip with this name already exists
		{
			zip_t *testzip = zip_open(zippath.c_str(), 0, 0);
			if (testzip != NULL) {
				zip_discard(testzip);
				consolelog("\nWARNING: A zip named \"%s.zip\" already exists! Overwrite it? (YES/NO)\n\n", episodename.c_str());
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
						consolelog("Invalid choice, valid choices are YES or NO\n");
						goto overwritechoice;
						break;
					}
				}
			}
		}
		episodezip = zip_open(zippath.c_str(), ZIP_CREATE|ZIP_TRUNCATE, 0);
		if (episodezip == NULL) throwerror("Failed to create the .zip file", ERROR_CANTCREATEZIP);
	}
	clock_t start = clock();
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
		consolelog("Entering Map Loop...\n");
		//updatebar();
		//open the episode directory
		DIR *episodedir = openpkdir(episodepath.pk.c_str());
		struct dirent *entry;
		struct stat filestat;
		//iterate trough all the files
		while ((entry = readdir(episodedir))) {
			stat(entry->d_name, &filestat);
        	if (!S_ISDIR(filestat.st_mode)) {
        		debugmsg = entry->d_name;
        		std::string filepath = episodepath.pk;
				filepath.append("/");
				filepath.append(entry->d_name);
        		FILE *mapfile = fopen(filepath.c_str(), "rb");
        		if (mapfile == NULL) continue;
        		//maxbar++;
        		std::string filename = entry->d_name;
        		if (filename.find(".map") != filename.npos) {
        			//maxbar += 3;
					//get and insert the map tileset, bg and music
					findandadd(pkread(0x5, 12, mapfile), episodepath, "gfx/tiles/");
					findandadd(pkread(0x12, 12, mapfile), episodepath, "gfx/scenery/");
					findandadd(pkread(0x1F, 12, mapfile), episodepath, "music/");
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
		debugmsg = "";
		closedir(episodedir);
	}
	//SPRITE LOOP - iterate trough the memorized sprites' names, find their file, get the sprites they need and, them to the queue and insert the current sprite
	{
		consolelog("Entering Sprite loop...\n");
		//updatebar();
		//get important paths
		paths spritepath;
		spritepath.pk.append(path);
		spritepath.pk.append("/sprites/");
		spritepath.zip = "sprites/";
		//iterate trough the sprite queue until it's empty
		while (sprqueue.size() > 0) {
			//prepare the current iteration's set
			std::unordered_set<std::string> spriterate;
			spriterate.swap(sprqueue);
			for (auto spritename = begin(spriterate); spritename != end(spriterate); ++spritename) { 
    			findandadd(*spritename, episodepath, spritepath.zip.c_str());
    			FILE *spritefile;
    			spritefile = fopen(std::string(spritepath.pk + "/" + (*spritename)).c_str(), "rb");
    			if (spritefile == NULL) {
    				spritefile = fopen(std::string(episodepath.pk + "/" + (*spritename)).c_str(), "rb");
    				if (spritefile == NULL) {
    					debugmsg = "";
    					throwerror(std::string("Could not open the file \"" + (*spritename) + "\"").c_str(), ERROR_CANTOPENFILE);
					}
				}
				debugmsg = *spritename;
				//insert the sprite sounds and bitmap
				findandadd(pkread(0x8, 12, spritefile), episodepath, spritepath.zip.c_str());
				findandadd(pkread(0x6C, 12, spritefile), episodepath, spritepath.zip.c_str());
				findandadd(pkread(0xD0, 12, spritefile), episodepath, spritepath.zip.c_str());
				findandadd(pkread(0x134, 12, spritefile), episodepath, spritepath.zip.c_str());
				findandadd(pkread(0x198, 12, spritefile), episodepath, spritepath.zip.c_str());
				findandadd(pkread(0x1FC, 12, spritefile), episodepath, spritepath.zip.c_str());
				//add to the queue the sprites this sprite needs
				memorizespr(pkread(0x4E0, 12, spritefile));
				memorizespr(pkread(0x544, 12, spritefile));
				memorizespr(pkread(0x5A8, 12, spritefile));
				memorizespr(pkread(0x60C, 12, spritefile));
				fclose(spritefile);
			}
		}
	}
	//save the zip
	
	consolelog("Saving the zip file, please wait...\n");
	//updatebar();
	zip_close(episodezip);
	consolelog("Successfully saved the zip\n");
	consolelog("Process completed in %d seconds\n", (clock() - start) / CLOCKS_PER_SEC);
}
