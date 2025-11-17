#include "save.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

std::unique_ptr<SaveData> g_savedata = nullptr;

#ifdef EMSCRIPTEN
static bool idbfs_mounted = false;

void mountIDBFS() {
	if (idbfs_mounted) return;
	
	EM_ASM({
		if (!FS.analyzePath("/persistent").exists) {
			FS.mkdir("/persistent");
		}
		var mountpoint = FS.analyzePath("/persistent");
		if (!mountpoint.mount || mountpoint.mount.mountpoint !== "/persistent") {
			try {
				FS.mount(IDBFS, {}, "/persistent");
			} catch(e) {
				if (e.name !== "ErrnoError" || e.errno !== 16) {
					console.error("IDBFS mount error:", e);
				}
			}
		}
	});
	idbfs_mounted = true;
}

void syncFS(bool populate) {
	EM_ASM({
		FS.syncfs($0 ? true : false, function(err) {
			if (err) {
				console.error("FS.syncfs error:", err);
			}
		});
	}, populate);
}
#endif

// Creates new save file and initializes empty SaveData struct if save file did not exist
void saveInit(){
	#ifdef EMSCRIPTEN
	mountIDBFS();
	syncFS(true);
	#endif
	
	int placeholder = 0;
	std::ofstream ofs(SAVE_PATH, std::ios::out | std::ios::binary);
	if (!ofs){
		std::cerr << "File Error: Failed to create save file\n";
		return;
	}
	// Insert zeroes for each level in SaveData struct
	for (int i = 0; i < NUM_DIFFS; i++)
		ofs.write(reinterpret_cast<char*>(&placeholder), sizeof(uint16_t));
	g_savedata = std::unique_ptr<SaveData>(new SaveData());
	ofs.close();
	
	#ifdef EMSCRIPTEN
	syncFS(false);
	#endif
}

// Returns the high score of the specified level
int getHighScore(int level){ 
	int score = 0;	
	std::ifstream ifs;
	ifs.open(SAVE_PATH, std::fstream::in | std::fstream::binary);
	if (ifs){
		ifs.seekg((level-1) * sizeof(uint16_t));
		ifs.read(reinterpret_cast<char*>(&score), sizeof(uint16_t));
		ifs.close();
	} else { // This should never happen unless the player is messing with save data while playing
		std::cerr << "Warning: Some weird shit happening with save file!\n";
	}
	return score;
}

// After game over, checks score for the level, and updates save data 
// if score is higher than before
void saveUpdate(int level, int score){
	std::fstream fs;
	fs.open(SAVE_PATH, std::fstream::in | std::fstream::out | std::fstream::binary);
	int high_score = 0;
	if (fs){
		fs.seekp((level-1) * sizeof(uint16_t)); // Move pointer to proper address in save file
		// Read the score from the address
		fs.read(reinterpret_cast<char*>(&high_score), sizeof(uint16_t)); 
		if (score > high_score){
			std::cout << "New high score " << score << " for level " << level << "!\n";
			fs.seekp((level-1) * sizeof(uint16_t)); // Move pointer back to the proper address
			fs.write(reinterpret_cast<char*>(&score), sizeof(uint16_t)); // write new score to address
		}
		fs.close();
		
		#ifdef EMSCRIPTEN
		syncFS(false);
		#endif
	} else { // This should never happen unless the player deletes the save while the game is running
		std::cerr << "Warning: Some weird shit happening with save file!\n";
	}
}

// Returns SaveData struct if load was successful
std::unique_ptr<SaveData> saveLoad(){
	#ifdef EMSCRIPTEN
	mountIDBFS();
	syncFS(true);
	#endif
	
	std::unique_ptr<SaveData> save_data;
	std::ifstream ifs;
	ifs.open(SAVE_PATH, std::fstream::in | std::fstream::binary);
	if (ifs){
		// Loaded file successfully
		save_data = std::unique_ptr<SaveData>(new SaveData());
		for (int i = 0; i < NUM_DIFFS; i++)
			ifs.read(reinterpret_cast<char*>(&save_data->_bank[i]), sizeof(uint16_t));
		// std::cout << "Loaded save data\n";
		// for (int i = 0; i < NUM_DIFFS; i++){
		// 	int score = save_data->_bank[i];
		// 	std::cout << score << "\n";
		// }
		ifs.close();
		return save_data;
	} 
	// Does not exist, need to create the file
	return nullptr;
}
