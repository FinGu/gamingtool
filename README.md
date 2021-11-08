# gamingtool
Personal tool that's meant to be used as a helper to play games on linux

1. meson build
2. ninja -C build

Usage:
- run game
- list (wine or game)

On first usage the .gamingtool folder is created in $HOME
  
Folder structure of .gamingtool
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

The config file is json encoded, there's an example in /extra/game/example/
