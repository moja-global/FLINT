# FLINT

[![All Contributors](https://img.shields.io/badge/all_contributors-5-orange.svg?style=flat-square)](#contributors)

The Full Lands Integration Tool (FLINT) is a C++ framework that combines satellite and ground data in ways that meet policy needs. Its based on over 20 years of experience building and operating integration tools in Australia and Canada. It's an Open-source Library maintained under [**moja global**](https://github.com/moja-global/About-moja-global/blob/master/README.md), a project under the Linux Foundation.

## Why FLINT?

FLINT makes developing and operating advanced systems achievable by all countries. Its a generic platform with a modular structure, allowing countries to attach any variety of models or data to build country-specific systems. The platform handles complex computer science tasks, such as the storage and processing of large data sets, leaving users to focus on monitoring, reporting and scenario analyses.

## How is FLINT different from earlier integrating tools?

FLINT uses the lessons learned from previous tools to meet present and future needs. The key improvements compared to earlier tools include:

+ a flexible, modular approach, allowing for country specific implementations from [IPCC Tier 1 to 3 ](https://www.reddcompass.org/mgd-content-v1/dita-webhelp/en/Box1.html).
+ support for [wall-to-wall, sample based, jurisdictional, and supply chain approaches](https://static1.squarespace.com/static/5896200f414fb57d26f3d600/t/59362b028419c2db8f57e747/1496722191543/REDD_nested_projects.pdf).
+ the ability to cover all [land uses and land use changes, and activity-based reporting such as REDD+](https://theredddesk.org/what-redd).
+ scenario analysis systems to allow the development of projections.
+ the potential for use in other purposes, like economics, water and biodiversity.
+ development managed using a true open source approach under [moja global](http://moja.global), which will allow users (countries, companies and organizations) to direct strategy and control the budget.
+ software that allows data processing on local desktops or cloud-based systems.

## Installation

[Step-by-step instruction guides](https://github.com/moja-global/FLINT/tree/master/How_to_use_FLINT) are available if you would like to have more detailed information. Below are some quick installation instructions:

### Windows

These instructions are for building the FLINT on Windows using Visual Studio 2017, or Visual Studio 2019.

#### Required Installs

##### CMake

- download [cmake-3.15.2-win64-x64.msi](https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2-win64-x64.msi)

#### Using vcpkg to install required libraries

A fork of vcpkg repository is present for building FLINT's required libraries. To use it follow the instructions below:

+ Clone the vcpkg repository: https://github.com/moja-global/vcpkg

+ Start a command shell in the vcpkg repository folder and use the following commands:

  ```powershell
  # bootstrap
  bootstrap-vcpkg.bat

  # install packages
  vcpkg.exe install boost-test:x64-windows boost-program-options:x64-windows boost-log:x64-windows turtle:x64-windows zipper:x64-windows poco:x64-windows libpq:x64-windows gdal:x64-windows sqlite3:x64-windows boost-ublas:x64-windows
  ```

+ Once this has completed, start a command shell in your FLINT repository folder. Now use the following commands to create the Visual Studio solution:

  ```powershell
  # Create a build folder under the Source folder
  cd Source
  mkdir build
  cd build

  # now create the Visual Studio Solution (2019)
  cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=C:/Development/Software/moja -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_TESTS=OFF -DENABLE_MOJA.MODULES.ZIPPER=OFF -DCMAKE_TOOLCHAIN_FILE=c:\Development\moja-global\vcpkg\scripts\buildsystems\vcpkg.cmake ..

  # OR Visual Studio Solution (2017)
  cmake -G "Visual Studio 15 2017" -DCMAKE_INSTALL_PREFIX=C:/Development/Software/moja -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_TESTS=OFF -DENABLE_MOJA.MODULES.ZIPPER=OFF -DCMAKE_TOOLCHAIN_FILE=c:\Development\moja-global\vcpkg\scripts\buildsystems\vcpkg.cmake ..
  ```

#### Install Moja Libraries

It's possible to use the Visual Studio moja solution to install built versions of the Moja libraries. To do this you need to set the CMake variable `CMAKE_INSTALL_PREFIX` to your install path (i.e. `C:/Development/Software/moja`).

#### Make edits to the Visual Studio Solution using CMake

1. Launch the CMake GUI
1. In the **Where to build the binaries** box click **Browse Build…** and select the folder you created above (i.e. `C:\Development\moja-global\FLINT\Source\build`). The **Where is the source code:** field should update, if not, set it manually.
1. You should be able to edit any CMake setting now (i.e. `ENABLE` flags like `ENABLE_TESTS`), then click **Configure** – assuming all libraries and required software is already installed, no errors should occur. Now click **Generate** and the Solution with adjustments should be ready to load into Visual Studio.

#### Other Useful Tools

[SQLite Studio](https://sqlitestudio.pl/): a SQLite database manager.

[TortoiseGit](https://tortoisegit.org/): Windows shell integration for Git.

### Docker on Ubuntu 18.04

It's possible to use Containers to build FLINT and all its required dependencies. An example on how to do this on Ubuntu 18.04 is as follows. See also the [`docker` directory.](https://github.com/moja-global/flint/tree/master/Examples/docker)

#### Building the containers

This example uses two `Dockerfile`s; the first to download and build the required libraries, and the second to download and build the moja FLINT libraries and CLI program.

```bash
# working from the examples folder "flint/tree/master/Examples/docker"

# build the base
docker build -f Dockerfile.base.ubuntu.18.04 --build-arg NUM_CPU=4 -t moja/baseimage:ubuntu-18.04 .

# build the flint container
docker build  -f Dockerfile.flint.ubuntu.18.04 --build-arg NUM_CPU=4 --build-arg FLINT_BRANCH=master -t moja/flint:ubuntu-18.04 .

docker build  -f Dockerfile.flint.ubuntu.18.04 --build-arg NUM_CPU=4 --build-arg GITHUB_AT=XXXX --build-arg FLINT_BRANCH=master -t moja/flint:ubuntu-18.04 .
```

How to use the final container depends on the task. The following command will bash into the flint container and allow you to use the CLI program.

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

## License

This project is licensed under the [Mozilla Public License 2.0](LICENSE)

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

**Maintainers** review and accept proposed changes\
**Reviewers** check proposed changes before they go to the Maintainers\
**Ambassadors** are available to provide training related to this repository\
**Coaches** are available to provide information to new contributors to this repository
