// DotVault: a small command-line tool that backs up Linux dotfiles to a Git repository.
#include <cstdlib>
#include <filesystem>
#include <print>
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
        std::println("Skipped (not found): {}", configPath.string());
        return 0;
    }

    // Use error codes instead of exceptions, so one bad file does not stop the whole backup
    std::error_code ec;
    fs::create_directories(backupPath, ec);
    if (ec) {
        std::println(stderr, "Cannot create {}: {}", backupPath.string(), ec.message());
        return 1;
    }

    int failures = 0;
    for (const auto& entry : fs::directory_iterator(configPath, ec)) {
        fs::path destination = backupPath / entry.path().filename();
        fs::copy(entry.path(), destination,
                 fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
        if (ec) {
            std::println(stderr, "Failed: {}: {}", entry.path().string(), ec.message());
            failures++;
        } else {
            std::println("Backed up: {} -> {}", entry.path().string(), destination.string());
        }
    }
    if (ec) {
        std::println(stderr, "Cannot read {}: {}", configPath.string(), ec.message());
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
        std::println("No changes, nothing to commit.");
        return true;
    }
    return std::system((repo + "commit -q -m 'DotVault: automatic backup' && " + repo + "push -q").c_str()) == 0;
}

int main(int argc, char* argv[]) {
    // Never build paths from a missing $HOME: that would back up to the wrong place
    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0') {
        std::println(stderr, "Error: $HOME is not set");
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

    std::println("DotVault: backing up to {}", backupRoot.string());
    int failures = 0;
    for (const std::string& target : targets) {
        failures += backupTarget(configRoot, backupRoot, target);
    }

    if (useGit && !commitAndPush(backupRoot)) {
        std::println(stderr, "Git commit/push failed");
        return 2;
    }

    if (failures > 0) {
        std::println(stderr, "{} item(s) failed to back up", failures);
        return 3;
    }
    std::println("Done.");
    return 0;
}
