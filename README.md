# DotVault

A small C++26 command-line tool that backs up my Linux config files (dotfiles) from `~/.config` to `~/Dotfiles` and pushes them to GitHub.

I built it to stop losing my Hyprland setup when reinstalling Arch Linux, and to practise modern C++ with `std::filesystem` and `std::println`.

## What it does

1. Copies the chosen folders from `~/.config/<name>` to `~/Dotfiles/<name>` (recursively, overwriting older copies).
2. Skips folders that do not exist, and keeps going when a single file fails to copy.
3. Runs `git add`, and only makes a commit and pushes when something actually changed.

## Build

Needs a compiler with C++26 support (GCC 14+ or Clang 19+) and CMake 3.25+.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Use

`~/Dotfiles` must already be a Git repository with a remote.

```sh
./build/dotvault                  # backs up hypr, kitty and waybar, then commits and pushes
./build/dotvault nvim fish        # backs up other folders instead
./build/dotvault --no-git kitty   # only copies, no commit or push
```

Exit codes: `0` success, `1` `$HOME` not set, `2` git failed, `3` one or more files failed to copy.

## Limitations

- Files deleted from `~/.config` are not deleted from the backup.
- Git is called through `std::system`, so `git` must be on your `PATH`.
- Linux/macOS only (uses `$HOME`).
