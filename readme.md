# Stilt Fox&reg; Storage Shed
#### Version 2.x.x
Stilt Fox&reg; Storage Shed is a C++ library made with the aim of creating a universal database api. This api which is 
provided by modules/DatabaseConnection.h++ will over time be implemented by different databases as Stilt Fox&reg; uses 
them in other projects. This project will be designed in a way that support for each database type can be left out of
the build using CMake options. This will avoid including dependencies in your project that you wish to not support.

#### Currently Implemented Databases
- Sqlite
- mariadb

## Legal Stuff
Stilt Fox&reg; is not liable for any damages done to your hardware. For more information see LICENSE file.\
Stilt Fox&reg; is trademarked. You may not use the Stilt Fox™ name, however this code is free to reference and use.\
You may contribute to this library, however all contributions will share the same license as this library, and you agree
that Stilt Fox&reg; owns the copyright for any contributions.

## Important!
These instructions assume you are using a Linux based operating system to compile the code. No testing or compiling
has been performed on Windows by Stilt Fox&reg;. If you choose to use Windows as your main development platform you are
on your own. If you have a strong desire to use Stilt Fox&reg; libraries and products, switching to Linux is highly
recommended.

With The death of Windows 10 (as of the time of writing this) directly around the corner, the roadmap to support Windows
systems has been completely canceled.

## Prerequisites
### Main Program
These libraries are required for the core functionality of this project.
- CMake
  - version 3.5.0 or greater
- C++ compiler
  - must support C++ version 20 or higher
  - We at Stilt Fox&reg; tend to use g++
- Stilt Fox&reg; Scribe
  - Can be found [here](https://github.com/StiltFox/Scribe)

### For Sqlite Support
To skip compiling this provide the `SFSkipSqlite=true` option when compiling.
- SQLite3
    - This code is statically linked as it has no copyright nor eula.
    - You can obtain this using `sudo apt install libsqlite3-dev`
      - Arch junkies can get their fix [here](https://archlinux.org/packages/core/x86_64/sqlite/)
    - Source code found [here](https://www.sqlite.org/index.html)
  
### For MariaDB Support
To skip compiling this provide the `SFSkipMariaDB=true` option when compiling.
- MariaDB Connector/C++
  - you can obtain this [here](https://mariadb.com/downloads/connectors/connectors-data-access/cpp-connector)
  - You can also use the package manager `yay -S mariadb-connector-cpp-git`
  
NOTE: MariaDB libraries may have issues compiling on newer versions of CMake. If your CMake version has a higher major
version than 3, it may be prudent to install a seperate instance of CMake 3.x.x to compile the libraries. This can be
done manually and the different CMake versions will not interfere with each other. See 
[here](https://github.com/Kitware/CMake/releases) for details.

### Testing
These libraries are only needed if you're running the unit tests. To skip the unit tests, use `SFSkipTesting=true` as an
option when compiling the code. You probably only want to do this during an installation.
- GTests
  - You can obtain this [here](https://github.com/google/googletest)
  - You can also use a package manager like this `sudo pacman -S gtest`
- JSON for modern C++
  - you can obtain this [here](https://github.com/nlohmann/json)
  - you can also use a package manager like this `sudo pacman -S nlohmann-json`

## Installation
The following commands will install the libraries to your system. These will exist globally to all users. If you wish to
install for a single user, please adjust accordingly. Also, you will require sudo privileges to run the 
`cmake --install .` command.
``` bash
#note: this will install all databases by default
mkdir build
cd build
cmake -DSFSkipTesting=true -DCMAKE_BUILD_TYPE=Release .. #if you wish to skip a database, put the option here.
cmake --build .
sudo cmake --install .
cd ..
```

## Linking to Stilt Fox&reg; Storage Shed
Linking to Stilt Fox&reg; Storage Shed is easy. In your CMakeLists.txt file include the following line:\
`find_package(StorageShed)`\
then to link to your project use the following line:\
`target_link_libraries(MyProject StiltFox::StorageShed::SqliteConnection)`\
please notice that each module you want to link must be linked separately as shown above. There is no way to just 
include all modules. This allows you to include only what you need in your build.

## Tutorials and Examples
If you want to see the classes of Stilt Fox&reg; Storage Shed, feel free to check out the [test folder](src/test). All 
public methods are thoroughly unit tested, and also demonstrate intended usage of the classes.

## Helper Scripts
Due to the complexity of compiling for multiple systems, a .sh file was created for each tested system to simply run the
unit tests. If all prerequisites are installed, these scripts should run without error. They use relative paths so 
caution should be used when executing them. Be sure to execute them from the project root like so:\
`./helper_scripts/linux_run_tests.sh`