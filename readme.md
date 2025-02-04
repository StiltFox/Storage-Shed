# Stilt Fox&reg; Storage Shed
#### Version 2.x.x
Stilt Fox&reg; Storage Shed is a C++ library made with the aim of creating a universal database api. This api which is provided by modules/DatabaseConnection.h++ will over time
be implemented by different databases as Stilt Fox&reg; uses them in other projects. This is a project that used to be a part of Stilt Fox&reg; Universal library, however
this project was split into many smaller ones for better maintainability and modularity.

## Legal Stuff
Stilt Fox&reg; is not liable for any damages done to your hardware. For more information see LICENSE file.\
Stilt Fox&reg; is trademarked. You may not use the Stilt Fox™ name, however this code is free to reference and use.\
You may contribute to this library, however all contributions will share the same license as this library and you agree that Stilt Fox&reg; owns the copyright for any contributions.

## Prerequisites
- CMake
  - version 3.5.0 or greater
- C++ compiler
  - must support C++ version 20 or higher
  - We at Stilt Fox&reg; tend to use g++
- Stilt Fox&reg; Scribe
  - Can be found [here](https://github.com/StiltFox/Scribe)
- SQLite3
    - This code is statically linked as it has no copyright nor eula.
    - You can obtain this using `sudo apt install libsqlite3-dev`
      - Arch junkies can get their fix [here](https://archlinux.org/packages/core/x86_64/sqlite/)
    - Source code found [here](https://www.sqlite.org/index.html)
- GTests
  - This is only required for unit testing, and can be skipped
    - To skip, use the `SFSkipTesting=true` option when compiling
  - You can obtain GTest by using `sudo apt install libgtest-dev`
    - if you are not running debian based linux, or are allergic to apt... it can be found [here](https://github.com/google/googletest)

## Installation
The following commands will install the libraries to your system. These will exist globally to all users. If you wish to
install for a single user, please adjust accordingly. Also, you will require sudo privileges to run the `cmake --install .`
command.
``` bash
mkdir build
cd build
cmake -DSFSkipTesting=true -DCMAKE_BUILD_TYPE=Release .. 
cmake --build .
cmake --install .
cd ..
```

## Cross Compiling For Windows
Because this project was made primarily for Linux based systems, getting the libraries running on windows can be tricky. Several configurations need to be made that Linux simply does not need.

### Prerequisites
- CMake
  - version 3.5.0 or greater
- C++ compiler
  - must support C++ version 20 or higher
  - must be mingw using a posix threading system
    - example: `x86_64-w64-mingw32-g++-posix`

### Setup
#### Compiler and Toolchain
The first order of business should be getting your compiler and toolchain up and running. To make this happen you first need to install the mingw compiler onto your computer and ninja build.\
`sudo apt install g++-mingw-w64 gcc-mingw-w64 ninja-build` 

#### Installing GTest for Windows
Because we are running on Linux, by default only the Linux binaries for the GTest library are available on the repo and we will have to compile the GTest library from scratch.
1. clone GTest from the github repo [here](https://github.com/google/googletest)
2. copy the toolchain file from [toolchains/windows.toolchain](toolchains) into the GTest directory.
3. do the following CMake commands:
``` bash
 mkdir build
 cd build
 rm -rf * #careful this is dangerous!
 cmake -DCMAKE_TOOLCHAIN_FILE=windows.toolchain -DCMAKE_INSTALL_PREFIX=/usr/i686-w64-mingw32/ ..
 cmake --build .
 cmake --install .
 ```

 #### Compiling and Installing Stilt Fox&reg; Storage Shed for Windows
```bash
mkdir build
cd build
rm -rf * # DANGER be sure you are in the build directory
# Here we will compile the whole project tests and all.
# If you just wish to install the library without running the tests
# you can specify the -DSFSkipTesting=true option. Just like
# in the Linux build.
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/windows.toolchain -DCMAKE_INSTALL_PREFIX=/usr/i686-w64-mingw32/ -DCMAKE_BUILD_TYPE=Release -G Ninja ..
cmake --build .
cmake --install .
```

#### Running the Windows Tests
Because these directions produce a windows executable, Linux cannot execute the compiled tests directly. Instead they need to be run using wine. Wine can be activated from the command line using something like:\
`wine <path to program>`\
for an example please checkout the [helper scripts](helper_scripts).\
to install wine on your system do the following:\
`sudo apt install wine`

## Linking to Stilt Fox&reg; Storage Shed
Linking to Stilt Fox&reg; Storage Shed is easy. In your CMakeLists.txt file include the following line:\
`find_package(StorageShed)`\
then to link to your project use the following line:\
`target_link_libraries(MyProject StiltFox::StorageShed::SqliteConnection)`\
please notice that each module you want to link must be linked separately as shown above. There is no way to just include all
modules. This allows you to include only what you need in your build.

## Tutorials and Examples
If you want to see the classes of Stilt Fox&reg; Storage Shed, feel free to check out the [test folder](src/test). All public methods
are thoroughly unit tested, and also demonstrate intended usage of the classes.

## Helper Scripts
Due to the complexity of compiling for multiple systems, a .sh file was created for each tested system to simply run the unit tests. If all prerequisites are installed, these scripts should run without error. They use relative paths so caution should be used when executing them. Be sure to execute them from the project root like so:\
`./helper_scripts/linux_run_tests.sh`