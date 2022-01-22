# Building this project

Here are instructions on how to build this project using vscode.

First, Make sure to install CMake and the C++ toolchain for your particular system if
it isn't already installed. Also install vscode extensions for C++ and Cmake Tools.

Then, you should be able to build and run (Hint: Ctrl+Shift+P then "CMake:Select a Kit" to configure the compiler/toolchain).

**NOTE:** This should automatically download and install raylib! If you run into issues, however, see Manual Compilation below.

You should be able to use both MSVC toolchain or otherwise MSYS2 MINGW64 (i.e., gcc) (See https://www.msys2.org/ for installation instructions; you may also have to edit `cmake-tools-kits.json` --- for info run "Cmake: Select a Kit" in vscode). Please delete/temporarily rename build folder before switching toolchains.

---

## Manual Compilation

If you are on windows, you can find information in https://github.com/raysan5/raylib/wiki/Working-on-Windows.

To build for the first time (adapted from https://github.com/raysan5/raylib/wiki/Working-with-CMake),
use `build.sh` (Do this in Git Bash if you are in windows!)

**NOTE:** Running the above WILL DOWNLOAD AND INSTALL raylib.

Once this is done, the executable will be present in `build/Debug/`.
