# gamingtool
Personal tool that's meant to be used as a helper to play games on linux

Building:
1. meson build
2. ninja -C build

Usage:
- run game
- list (wine or game)

With the first usage of the program a folder named .gamingtool is created in $HOME
  
Folder structure of .gamingtool:

```bash
├── config
├── game
│   └── game1
│       ├── config
│       ├── postlaunch
│       └── prelaunch
├── log
│   └── game1
│       └── 2021-12-19 02:46:15.txt
└── wine
    └── version1
```

Name/Version of a game/wine is it's folder name.

The main config file contains two literals, "log" and "debug", "log" enables logging the output of the game to a file with the current date as it's name, "debug" enables the logging of program info, output of both scripts and game to the console

The game config files are json encoded, an example is available in /extra/game/example/
