# FLINT Open-source Library

[![All Contributors](https://img.shields.io/badge/all_contributors-5-orange.svg?style=flat-square)](#contributors)

## What is `FLINT`?

The Full Lands Integration Tool (FLINT) is a C++ framework that combines satellite and ground data in ways that meet policy needs. It is based on over 20 years of experience building and operating integration tools in Australia and Canada.   It's an Open-source Library maintained under [**moja.global**](https://github.com/moja-global/About-moja-global/blob/master/README.md), a project under the Linux Foundation

## Why FLINT?

The FLINT makes developing and operating advanced systems achievable by all countries. It is a generic platform with a modular structure, allowing countries to attach any variety of models or data to build country-specific systems. The platform handles complex computer science tasks, such as the storage and processing of large data sets, leaving users to focus on monitoring, reporting and scenario analyses.  

## How is FLINT different from earlier integrating tools?

The FLINT is using the lessons learned from first generation tools, to build a new framework that meets present and future needs. The key improvements compared to the first generation tools include:
+ a flexible, modular approach, allowing for country specific implementations from [IPCC Tier 1 to 3 ](https://www.reddcompass.org/mgd-content-v1/dita-webhelp/en/Box1.html)
+ support for [wall-to-wall, sample based, jurisdictional, and supply chain approaches](https://static1.squarespace.com/static/5896200f414fb57d26f3d600/t/59362b028419c2db8f57e747/1496722191543/REDD_nested_projects.pdf)
+ the ability to cover all [land uses and land use changes, and activity-based reporting such as REDD+](https://theredddesk.org/what-redd)
+ scenario analysis systems to allow the development of projections
+ the potential to be used for multiple other purposes, including economics, water and biodiversity
+ development managed using a true open source approach under [moja global](http://moja.global), which will allow users (countries, companies and organizations) to direct strategy and control the budget.
+ software that allows data processing on local desktops or cloud-based systems   


## How to use FLINT?  

### Development Environment How-To for Windows

These instructions are for building the FLINT on Windows using Visual Studio 2017 (version 15.9.11).

#### Required Installs

##### Windows 8.1 SDK

- download [Windows 8.1 SDK](http://msdn.microsoft.com/en-US/windows/desktop/bg162891)

##### CMake

- download [cmake-3.14.3-win64-x64.msi](https://github.com/Kitware/CMake/releases/download/v3.14.3/cmake-3.14.3-win64-x64.msi)

##### Python 3

- download [python-3.6.3-amd64](https://www.python.org/ftp/python/3.6.3/python-3.6.3-amd64.exe)

##### OpenSSL Library

- download [Win64OpenSSL-1_1_1b.exe](http://slproweb.com/download/Win64OpenSSL-1_1_1b.exe)
- Install using the defaults and ignore any errors about the VS2008 redistributables.

#### Required Libraries

##### Boost C++ Libraries

While the Boost libraries can be built, for the purpose of this document we will download the pre-built binaries. These can be downloaded from [Prebuilt Windows Boost C++ Libraries](https://www.boost.org/users/download/).

Current version in use: 1.70.0 (*boost_1_70_0-msvc-14.1-64.exe*).

##### POCO C++ Libraries

Source code can be downloaded from [Poco project](https://pocoproject.org/download.html). Current version in use '*1.9.0 (Basic Edition)*' download [poco-1.9.0](https://pocoproject.org/releases/poco-1.9.0/poco-1.9.0.zip):

The **Basic Edition** ([poco-1.9.0.tar.gz](https://pocoproject.org/releases/poco-1.9.0/poco-1.9.0.tar.gz) or [poco-1.9.0.zip](https://pocoproject.org/releases/poco-1.9.0/poco-1.9.0.zip)) only contains the Foundation, JSON, XML, Util and Net libraries, but does not require any external dependencies.

###### Build instructions

- Extract the archive into `C:\Development\poco-1.9.0`
- Edit the file “components” and delete the lines:

```
  CppUnit
  CppUnit/WinTestRunner
  Data/MySQL
  NetSSL/OpenSSL
  Crypto
  PageCompiler
  PageCompiler/File2PageA
```

- **Visual Studio 2017:** Open up a command prompt where the archive was extracted and type:

```
buildwin 150 build all both x64
```

##### Turtle Library

This is a C++ mock object library for Boost ([Turtle](https://github.com/mat007/turtle)), used in the test projects. Download at [turtle-1.3.1](https://github.com/mat007/turtle/releases/download/v1.3.1/turtle-1.3.1.zip)

- Extract the archive into `C:\Development\turtle-1.3.1`

##### SQLite Library

[download sqlite-amalgamation-3260000](https://www.sqlite.org/2018/sqlite-amalgamation-3260000.zip)

- Extract the archive into `C:\Development\sqlite-amalgamation-3260000`

#### Non-Essential Libraries

Various FLINT Providers and Modules have been separated into sub-projects. These projects can be excluded/included in builds by setting ***ENABLE_XXX*** flags in cmake (i.e. ***ENABLE_MOJA.MODULES.LIBPQ*** & ***ENABLE_MOJA.MODULES.ZIPPER***).

By disabling certain of these projects some external libraries will not be required. Making the build process less onerous.

##### PostgreSQL

CMake Flag: **ENABLE_MOJA.MODULES.LIBPQ**

Currently using version 9.5.6-2 (found at http://www.postgresql.org/)

- [download postgresql-9.5.6-2-windows-x64.exe](http://get.enterprisedb.com/postgresql/postgresql-9.5.6-2-windows-x64.exe)
- This includes LibPQ library which is required. I had some issues with this not being found in the PATH (or the wrong version being found in QGis first). Moved the PostgreSQL version in the environment variable to before the GDAL version.

##### Zlib

CMake Flag: **ENABLE_MOJA.MODULES.ZIPPER**

[download zlib-1.2.11](http://www.zlib.net/zlib1211.zip)

- Extract the archive into `C:\Development\zlib-1.2.11`
- Run cmake-gui and set options:
  - source code: `C:\Development\zlib-1.2.11`
  - binaries: `C:\Development\zlib-1.2.11\build`
  - CMAKE_INSTALL_PREFIX: `C:\Development\zlib-1.2.11\dist`
    +Edit all INSTALL paths (INSTALL_BIN_DIR, etc.) to point to location for CMAKE_INSTALL_PREFIX
- Configure / Generate
- Build Release, Release-Install, Debug, and Debug-Install
- if the global.moja cmake zlib find package doesn't have your install path, set system environment vars: 
  - `ZLIB_SOURCE=C:\Development\zlib-1.2.11`
  - `ZLIBROOT=%ZLIB_SOURCE%\dist`

##### Zipper

**NEED TO MAKE THIS A MOJA VERSION**

[github for Zipper for in SLEEK-TOOLS](https://github.com/SLEEK-TOOLS/zipper)

[git clone link for repository](https://github.com/SLEEK-TOOLS/zipper.git)

- Install Zlib as described in this document
- Clone the Zipper repository into `C:\Development\zipper`
- make sure to do a recursive clone (--recursive)
- Run cmake-gui and set options:
  - source code: `C:\Development\zipper`
  - binaries: `C:\Development\zipper\build`
  - CMAKE_INSTALL_PREFIX: `C:\Development\zipper\dist`
- **WARNING:** check that the correct zlib (lib & source) is found
- Configure / Generate
- Build Release, Release-Install, Debug, and Debug-Install
- if the global.moja cmake zipper find package doesn't have your install path, Set system environment varibales:
  - `ZIPPER_ROOT=C:\Development\zipper`

[Original Zipper on GitHub](https://github.com/sebastiandev/zipper)

Changes made in the SLEEK-TOOLS fork:

Edited `zipper\CMakeLists.txt` and insert this text at line 105:

```cmake
IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
ELSE()
	IF(NOT CMAKE_DEBUG_POSTFIX)
		SET(CMAKE_DEBUG_POSTFIX d)
	ENDIF()
ENDIF()
```

#### Set Environment Variables

To help the current build system find the required libraries we use Environment Variables.

Add this to your system environment variables: Control Panel -> System -> Advanced system settings -> Environment Variables -> System variables

```
BOOST_ROOT=<path to Boost library install>
```

##### for example:

```
BOOST_ROOT=C:\Development\boost\boost_1_70_0
```

For FLINT to run (both in and external to the IDE) the various libraries built need to be found. The easiest way is to add the builds you have done to the Path.

```
BOOST_LIB_DIR=<path to Boost library libraries>
POCO_LIB_DIR=<path to POCO library libraries>
ZLIB_LIB_DIR
ZIPPER_LIB_DIR
```

for example:

```
BOOST_LIB_DIR=C:\Development\boost\boost_1_70_0\lib64-msvc-14.1
POCO_LIB_DIR=C:\Development\poco-1.9.0\bin64
```

The Environment variables can be added to the System Path.

#### Install Moja Libraries

It is possible to use the Visual Studio moja solution to install built versions of the Moja libraries. To do this you need to set the CMAKE variable '***CMAKE_INSTALL_PREFIX***' to the install path (i.e. "*C:/Development/Software/moja/moja_develop*"). Then re-generate your Moja project file.

Now, in the Visual Studio solution, select the project INSTALL and build the target you want to install (i.e. Release, Debug, etc.). 

This will build all files required for other projects to use the Moja library (DLLS, LIBS and copy in required header files).

#### Generate Visual Studio Solution Using CMake

1. Launch the CMake GUI (current version 3.14.3)
2. Click “Browse Source…” and point to `C:\Development\moja.global\Source`
3. To do an ‘*out-of-source build*’ set the build directory to `C:\Development\moja.global\Source\Build`. This will build everything under the directory you specify, making it easier to cleanup when required.
4. Click “***Configure***” – assuming all libraries and required software has been installed you should have no errors. If there are errors the most likely cause is the cmake packages aren't setup to find in the correct paths. Our versions of these can be found in your cloned git folder for moja.global. So, `C:\Development\moja.global\Source\cmake`. Depending on which library is failing, load the required Find package and check the search paths listed.

```
FindLibpq.cmake
FindPoco.cmake
FindTurtle.cmake
```

Once you have configured you can adjust some of the values to change the project that will be generated:

```
BOOST_TEST_REPORTING_LEVEL          =[DETAILED|SHORT]
ENABLE_TESTS                        =[ON|OFF]
RUN_UNIT_TESTS_ON_BUILD             =[ON|OFF]

ENABLE_MOJA.MODULES.ZIPPER 			=[ON|OFF]
ENABLE_MOJA.MODULES.POCO 			=[ON|OFF]
ENABLE_MOJA.MODULES.LIBPQ 			=[ON|OFF]
ENABLE_MOJA.MODULES.GDAL 			=[ON|OFF]
ENABLE_MOJA.CLI 					=[ON|OFF]
ENABLE_MOJA.SYSTEMTEST 				=[ON|OFF]
```

By turning the enable flags to `OFF` you can stop certain parts of the project being generated.

##### Install Other Useful Tools

##### SQLIte Studio

a simple windows SQLite database manager (http://sqlitestudio.pl/)
[sqlitestudio-3.1.0.zip](http://sqlitestudio.pl/files/sqlitestudio3/complete/win32/sqlitestudio-3.1.0.zip)

##### TortoiseGit

[TortoiseGit](https://code.google.com/p/tortoisegit/wiki/Download)

### Docker for Ubuntu 18:04

Containers are a simple way to build FLINT and all required dependencies. Examples of how this can be done are provided for Ubuntu 18.04. See the [Examples docker directory.](https://github.com/moja-global/flint/tree/master/Examples/docker)

#### Building the containers

The build has been split into 2\two Dockerfiles, the first to get and build required libraries. The second to get and build the moja FLINT libraries and CLI program.

```bash
# working from the examples folder "flint/tree/master/Examples/docker"

# build the base
docker build -f Dockerfile.base.ubuntu.18.04 --build-arg NUM_CPU=4 -t moja/baseimage:ubuntu-18.04 .

# build the flint container
docker build  -f Dockerfile.flint.ubuntu.18.04 --build-arg NUM_CPU=4 --build-arg FLINT_BRANCH=master -t moja/flint:ubuntu-18.04 .

docker build  -f Dockerfile.flint.ubuntu.18.04 --build-arg NUM_CPU=4 --build-arg GITHUB_AT=XXXX --build-arg FLINT_BRANCH=master -t moja/flint:ubuntu-18.04 .
```

How to use the final container depends on the task. However, the following command will bash into the flint container and allow you to use the CLI program.

```bash
# run bash on the flint container
docker run --rm -ti moja/flint:ubuntu-18.04 bash
```

Once in, you should be able to run the CLI program `moja.cli`

```
# run CLI
moja.cli --help
```

That should respond:

```
Allowed options:

General options:
  -h [ --help ]          produce a help message
  --help-section arg     produce a help message for a named section
  -v [ --version ]       output the version number

Commandline only options:
  --logging_config arg   path to Moja logging config file
  --config_file arg      path to Moja run config file
  --provider_file arg    path to Moja data provider config file

Configuration file options:
  --config arg           path to Moja project config files
  --config_provider arg  path to Moja project config files for data providers
```


## How to Get Involved?  

moja global welcomes a wide range of contributions as explained in [Contributing document](https://github.com/moja-global/About-moja-global/blob/master/CONTRIBUTING.md) and in the [About moja-global Wiki](https://github.com/moja-global/.github/wiki).  

  
## FAQ and Other Questions  

* You can find FAQs on the [Wiki](https://github.com/moja.global/.github/wiki).  
* If you have a question about the code, submit [user feedback](https://github.com/moja-global/About-moja-global/blob/master/Contributing/How-to-Provide-User-Feedback.md) in the relevant repository  
* If you have a general question about a project or repository or moja global, [join moja global](https://github.com/moja-global/About-moja-global/blob/master/Contributing/How-to-Join-moja-global.md) and 
    * [submit a discussion](https://help.github.com/en/articles/about-team-discussions) to the project, repository or moja global [team](https://github.com/orgs/moja-global/teams)
    * [submit a message](https://get.slack.help/hc/en-us/categories/200111606#send-messages) to the relevant channel on [moja global's Slack workspace](mojaglobal.slack.com). 
* If you have other questions, please write to info@moja.global   
  

## Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore -->
<table><tr><td align="center"><a href="https://github.com/mtbdeligt"><img src="https://avatars3.githubusercontent.com/u/16447169?v=4" width="100px;" alt="Rob de Ligt"/><br /><sub><b>Rob de Ligt</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mtbdeligt" title="Documentation">📖</a> <a href="#maintenance-mtbdeligt" title="Maintenance">🚧</a></td><td align="center"><a href="https://github.com/malfrancis"><img src="https://avatars0.githubusercontent.com/u/5935221?v=4" width="100px;" alt="Mal"/><br /><sub><b>Mal</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=malfrancis" title="Code">💻</a></td><td align="center"><a href="https://github.com/leitchy"><img src="https://avatars0.githubusercontent.com/u/3417817?v=4" width="100px;" alt="James Leitch"/><br /><sub><b>James Leitch</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=leitchy" title="Code">💻</a></td><td align="center"><a href="https://github.com/mfellows"><img src="https://avatars0.githubusercontent.com/u/8548157?v=4" width="100px;" alt="Max Fellows"/><br /><sub><b>Max Fellows</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mfellows" title="Code">💻</a></td></tr></table>

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!  

## Maintainers Reviewers Ambassadors Coaches

The following people are Maintainers of this repository

<table><tr><td align="center"><a href="https://github.com/malfrancis"><img src="https://avatars0.githubusercontent.com/u/5935221?v=4" width="100px;" alt="Mal"/><br /><sub><b>Mal</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=malfrancis" title="Code">💻</a></td><td align="center"><a href="https://github.com/leitchy"><img src="https://avatars0.githubusercontent.com/u/3417817?v=4" width="100px;" alt="James Leitch"/><br /><sub><b>James Leitch</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=leitchy" title="Code">💻</a></td><td align="center"><a href="https://github.com/mfellows"><img src="https://avatars0.githubusercontent.com/u/8548157?v=4" width="100px;" alt="Max Fellows"/><br /><sub><b>Max Fellows</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mfellows" title="Code">💻</a></td></tr></table>

**Maintainers** review and accept proposed changes  
**Reviewers** check proposed changes before they go to the Maintainers  
**Ambassadors** are available to provide training related to this repository  
**Coaches** are available to provide information to new contributors to this repository  

