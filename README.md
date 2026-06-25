cat << 'EOF' > README.md
# DotVault 🔒

A lightweight, high-performance CLI utility written in C++20 designed to automatically scan, back up, and synchronize Linux dotfiles directly to a secure cloud repository.

## 🚀 Features

- **Automated Directory Scanning:** Dynamically discovers and indexes targeted configuration directories inside `~/.config/`.
- **Cross-Platform Portability:** Resolves user home directories dynamically via environment variables (`$HOME`), eliminating hardcoded paths.
- **Smart Mirroring:** Copies subdirectories recursively and selectively overwrites existing records using efficient `std::filesystem` rules.
- **Git & GitHub Pipeline Automation:** Automatically aggregates changes, generates a system commit timestamp, and pushes assets to remote repositories natively without shell context degradation.

## 📁 Monitored Configurations

By default, DotVault monitors and mirrors the following mission-critical configurations:
- **Hyprland** (`~/.config/hypr`) - Core window manager rules, inputs, and keybinds.
- **Kitty** (`~/.config/kitty`) - GPU-accelerated terminal configurations and themes.
- **Waybar** (`~/.config/waybar`) - System status bars, workspace indicators, and tracking shell scripts.

## 🛠️ Compilation & Installation

Ensure you have a modern C++ compiler supporting the **C++20 standard** (e.g., GCC 10+ or Clang 11+).

### 1. Clone the Source
```bash
git clone [https://github.com/Icemclovin/dotvault.git](https://github.com/Icemclovin/dotvault.git)
cd dotvault