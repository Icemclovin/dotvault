// DotVault: a small command-line tool that backs up Linux dotfiles to a Git repository.
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

// Copies one config directory from ~/.config/<target> to <backupRoot>/<target>.
// Returns the number of entries that failed to copy.
static int backupTarget(const fs::path& configRoot, const fs::path& backupRoot, const std::string& target) {
    fs::path configPath = configRoot / target;
    fs::path backupPath = backupRoot / target;

    if (!fs::exists(configPath)) {
        std::cout << "Skipped (not found): " << configPath << '\n';
        return 0;
    }

    // Use error codes instead of exceptions, so one bad file does not stop the whole backup
    std::error_code ec;
    fs::create_directories(backupPath, ec);
    if (ec) {
        std::cerr << "Cannot create " << backupPath << ": " << ec.message() << '\n';
        return 1;
    }

    int failures = 0;
    for (const auto& entry : fs::directory_iterator(configPath, ec)) {
        fs::path destination = backupPath / entry.path().filename();
        fs::copy(entry.path(), destination,
                 fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
        if (ec) {
            std::cerr << "Failed: " << entry.path() << ": " << ec.message() << '\n';
            failures++;
        } else {
            std::cout << "Backed up: " << entry.path() << " -> " << destination << '\n';
        }
    }
    if (ec) {
        std::cerr << "Cannot read " << configPath << ": " << ec.message() << '\n';
        failures++;
    }
    return failures;
}

// Commits and pushes the backup folder. Only commits when something changed.
static bool commitAndPush(const fs::path& backupRoot) {
    const std::string repo = "git -C '" + backupRoot.string() + "' ";

    if (std::system((repo + "add -A").c_str()) != 0) return false;

    // "git diff --cached --quiet" returns 0 when there is nothing new to commit
    if (std::system((repo + "diff --cached --quiet").c_str()) == 0) {
        std::cout << "No changes, nothing to commit.\n";
        return true;
    }
    return std::system((repo + "commit -q -m 'DotVault: automatic backup' && " + repo + "push -q").c_str()) == 0;
}

int main(int argc, char* argv[]) {
    // Never build paths from a missing $HOME: that would back up to the wrong place
    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0') {
        std::cerr << "Error: $HOME is not set\n";
        return 1;
    }

    // Usage: dotvault [--no-git] [config dirs...]   (default dirs: hypr kitty waybar)
    bool useGit = true;
    std::vector<std::string> targets;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--no-git") {
            useGit = false;
        } else {
            targets.push_back(arg);
        }
    }
    if (targets.empty()) {
        targets = {"hypr", "kitty", "waybar"};
    }

    const fs::path configRoot = fs::path(home) / ".config";
    const fs::path backupRoot = fs::path(home) / "Dotfiles";

    std::cout << "DotVault: backing up to " << backupRoot << '\n';
    int failures = 0;
    for (const std::string& target : targets) {
        failures += backupTarget(configRoot, backupRoot, target);
    }

    if (useGit && !commitAndPush(backupRoot)) {
        std::cerr << "Git commit/push failed\n";
        return 2;
    }

    if (failures > 0) {
        std::cerr << failures << " item(s) failed to back up\n";
        return 3;
    }
    std::cout << "Done.\n";
    return 0;
}
