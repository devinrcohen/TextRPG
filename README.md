# TextRPG
Welcome to TextRPG! It's your first day of school. Will you be a preppie or a jock? A bully or a geek? A lady's man or a burnout? Are you on your way to Harvard or will you drop out? Will you become the class president? In TextRPG, you decide your fate. Inspired by the 1990's cult classic DOS text-based RPG, Virtua School, TextRPG is a creative outlet and an excuse for me to learn the curses TUI library and become more proficient in C programming.

## Compiling and Running
### Prerequisites
This project uses the ncurses ecosystem:

* Headers: ```ncurses.h``` (or ```nurses.h```), ```panel.h```, ```menu.h```
* Libraries: ```ncurses``` (or ```curses```), ```panel```, ```menu```

On most systems, panel and menu are shipped as part of ncurses. You may compile TextRPG with CMake or directly from the command line.
### Linux 
#### Debian / Ubuntu
##### Install Dependencies
```Bash
sudo apt update
sudo apt install build-essential cmake libncurses-dev
```
##### Build & Run
```Bash
cmake -S . -B build
cmake --build build
./build/TextRPG
```

#### Fedora / RHEL / CentOS
```Bash
sudo dnf install gcc clang make cmake ncurses-devel

cmake -S . -B build
cmake --build build
./build/TextRPG
```

#### Fedora / RHEL / CentOS
```Bash
sudo dnf install gcc clang make cmake ncurses-devel

cmake -S . -B build
cmake --build build
./build/TextRPG
```

#### Arch / Manjaro
```Bash
sudo pacman -S --needed base-devel cmake ncurses

cmake -S . -B build
cmake --build build
./build/TextRPG
```

#### Direct Compile
```Bash
gcc -std=c11 -O2 -Wall -Wextra -o TextRPG main.c ui.c game.c -lncurses -lpanel -lmenu

./TextRPG
```

### macOS
First, install [Homebrew](https://brew.sh).

#### Install Dependencies
```Bash
brew install ncurses cmake
```
#### Build with CMake
```Bash
cmake -S . -B build
cmake --build build
./build/TextRPG
```

#### Direct Compile
```Bash
NCURSES_PREFIX="$(brew --prefix ncurses)"

gcc -std=c11 -O2 -Wall -Wextra \
  -I"$NCURSES_PREFIX/include" \
  -L"$NCURSES_PREFIX/lib" \
  -o TextRPG main.c ui.c game.c \
  -lncurses -lpanel -lmenu

./TextRPG
```

### Windows
Instructions coming soon.
