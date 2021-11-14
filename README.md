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
│   ├── game1
│   │   ├── config
│   │   ├── postlaunch
│   │   └── prelaunch
└── wine
    └── version1
```

Name/Version of a game/wine is it's folder name.

The config file is json encoded, example available in /extra/game/example/
