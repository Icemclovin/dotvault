// DotSync: A lightweight cross-platform dotfiles backup and synchronization tool.
#include <iostream>
// Include the filesystem library for managing files and directories.
#include <filesystem>
#include <vector>



namespace fs = std::filesystem;


// The entry point of the application.
int main () {
    // Print a welcome message to the terminal.
    std::cout << "Starting DotSync..." << std::endl;

    // Get the current user's home directory dynamically to ensure portability
    std::string homePath = std::getenv("HOME");

    // List of configuration directories to back up
    std::vector<std::string> targets = {"hypr", "kitty", "waybar"};

    // Process each target directory in the list
    for (const std::string& target : targets) {

    // Construct universal paths relative to the user's home directory
    fs::path configPath = fs::path(homePath) / ".config" / target;
    fs::path backupPath = fs::path(homePath) / "Dotfiles" / target;
    
    
    

    // Ensure the backup directory exists, create it if it doesn't
    fs::create_directories(backupPath);

    // Check if the target configuration directory exists
    if (fs::exists(configPath)) {

        // Notify the user that the directory was successfully found
        std::cout << "Target directory found!: " << configPath << std::endl;

        // Iterate through the directory and print the name of each file
        for (const auto& entry : fs::directory_iterator(configPath)) {

            // Extract the full filename including its extension
            std::string filename = entry.path().filename().string();
            std::cout << "Full file name: " << filename << std::endl;

            // Copy the file to the backup directory, overwriting if it already exists
            fs::copy(entry.path(), backupPath / filename, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

            // Log the successful backup of the file
            std::cout << "Backed up: " << entry.path() << " to " << backupPath / filename << std::endl;
         
        }
    
     }  else {
            std::cout << "Target directory not found: " << configPath << std::endl;
        }
    }
     
       // Automatically stage, commit, and push the backup changes to GitHub
       std::system("cd ~/Dotfiles && git add . && git commit -m 'DotSync: Automatic backup' && git push");   

        return 0;
        
    
}
       
    
    
