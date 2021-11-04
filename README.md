# gamingtool
Personal tool that's meant to be used as a helper to play games on linux

-> meson build
-> ninja -C build

Usage:
run <game>
list <wine or game>

On first usage the .gamingtool folder is created in $HOME
  
Folder structure:
.gamingtool ->
  game ->
    <game1> ->
       config,
       prelaunch,
       postlaunch
  wine ->
    <version1>,
    <version2>,
    ...

Name/Version of a game/wine is it's folder name
the config file is json encoded, there's an example in /extra/game/example/
