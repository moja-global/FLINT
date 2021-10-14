# FLINT Open-source Library

[![All Contributors](https://img.shields.io/badge/all_contributors-5-orange.svg?style=flat-square)](#contributors)

## What is FLINT?

Full Lands Integration Tool (FLINT) is a C++ framework that combines satellite and ground data in a way that meets policy requirements. It is based on more than 20 years of experience in building and operating integrated tools in Australia and Canada. It's an Open-Source Library maintained under [**moja global**](https://github.com/moja-global/About-moja-global/blob/master/README.md), a project under the [Linux Foundation.](https://linuxfoundation.org/)

## Why FLINT?

FLINT enables all countries to develop and operate advanced systems. It is a universal platform with a modular structure, allowing countries to attach any kind of models or data to build a country-specific system. The platform handles complex computer science tasks, such as storing and processing large data sets, allowing users to focus on monitoring, reporting, and scenario analysis.

## How is FLINT different from earlier integrating tools?

FLINT is using the lessons learned from the first generation of tools to build a new framework that meets current and future needs. Compared with the first-generation tools, the main improvements include:

- A flexible and modular approach that allows specific countries from IPCC Tier 1 to 3 to implement a [comprehensive, sample-based, jurisdictional and supply chain-based approach.](https://gcftf.org/wp-content/uploads/2020/12/REDD_nested_projects.pdf)
- The ability to cover all land uses and land use changes, and activity-based reporting such as REDD+.
- Scenario analysis systems to allow the development of projections.
- Potential for a variety of other purposes, including economics, water, and biodiversity.
- Development managed using a true open source approach under [moja global](http://moja.global), which will allow users (countries, companies and organizations) to direct strategy and control the budget.
- Software that allows data processing on local desktops or cloud-based systems.

## Getting Started with FLINT

Below are instructions for quick installation.

**Note:** If you need more detailed information, you can use a [step-by-step guide](https://github.com/moja-global/FLINT/tree/master/How_to_use_FLINT) or view the [video](https://www.youtube.com/watch?v=Jfi2-vEhfkg).

Please provide us with feedback on how to improve these guidelines. You can leave your comment in the [version under development.](https://github.com/moja-global/FLINT/blob/master/How_to_use_FLINT/How_to_use_FLINT.md)

### Development Environment How-To for Windows

These instructions are for building the FLINT on Windows using Visual Studio 2017, or Visual Studio 2019.

#### Required Installs

##### CMake

- Download [cmake-3.15.2-win64-x64.msi](https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2-win64-x64.msi)

#### Using vcpkg to install required libraries

A fork of a _Vcpkg_ repository has been created for the FLINT required libraries. To build these libraries you can use the following process:

- Clone the Vcpkg repository: https://github.com/moja-global/vcpkg

- Start a command shell in the Vcpkg repository folder and use the following commands:

  ```powershell
  # bootstrap
  bootstrap-vcpkg.bat

  # install packages
  vcpkg.exe install boost-test:x64-windows boost-program-options:x64-windows boost-log:x64-windows turtle:x64-windows zipper:x64-windows poco:x64-windows libpq:x64-windows gdal:x64-windows sqlite3:x64-windows boost-ublas:x64-windows
  ```

- Once this has completed, start a command shell in your FLINT repository folder. Now use the following commands to create the Visual Studio solution:

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

It is possible to use the Visual Studio moja solution to install built versions of the Moja libraries. To do this you need to set the CMAKE variable '**_CMAKE_INSTALL_PREFIX_**' to your install path (i.e. "_C:/Development/Software/moja_").

#### Make edits to the Visual Studio Solution using CMake

1. Launch the CMake GUI
2. In the '_Where to build the binaries_' field click “Browse Build…” and select the folder you created above (i.e. `C:\Development\moja-global\FLINT\Source\build`)`. The '_Where is the source code:_' field should update, if not, set it correctly.
3. You should be able to edit any CMake setting now (i.e. ENABLE flags like `ENABLE_TESTS`), then click “**_Configure_**” – assuming all libraries and required software has been installed you should have no errors. Now click **_"Generate"_** and the Solution with adjustments should be ready to load into Visual Studio.

#### Other Useful Tools

##### SQLIte Studio

A simple windows SQLite database manager (http://sqlitestudio.pl/)
[sqlitestudio-3.1.0.zip](http://sqlitestudio.pl/files/sqlitestudio3/complete/win32/sqlitestudio-3.1.0.zip)

##### TortoiseGit

[TortoiseGit](https://code.google.com/p/tortoisegit/wiki/Download)

### Docker for Ubuntu 18:04

Containers are a simple way to build FLINT and all required dependencies. Examples of how this can be done are provided for Ubuntu 18.04. See the [Examples docker directory.](https://github.com/moja-global/flint/tree/master/Examples/docker)

#### Building the containers

The build has been split into two Dockerfiles, the first to get and build required libraries. The second to get and build the moja FLINT libraries and CLI program.

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

moja global welcomes a wide range of contributions as explained in the [Contributing Guide](https://community.moja.global/community/contributing) and in the [About moja global page](https://community.moja.global/docs/about-moja-global).

## FAQ and Other Questions

- You can find FAQs on the [FAQs section of our docs](https://docs.moja.global/en/latest/faq.html).
- If you have a question about the code, submit a [user feedback](https://github.com/moja-global/About-moja-global/blob/master/Contributing/How-to-Provide-User-Feedback.md) on the relevant repository
- If you have a general question about a project or repository or moja global, [join moja global](https://docs.moja.global/en/latest/contact.html) and:
  - [Submit a discussion](https://help.github.com/en/articles/about-team-discussions) to the project, repository or moja global [team](https://github.com/orgs/moja-global/teams)
  - [Submit a message](https://get.slack.help/hc/en-us/categories/200111606#send-messages) to the relevant channel on [moja global's Slack workspace](https://mojaglobal.slack.com).
- If you have other questions, please write to [info@moja.global](mailto:info@moja.global).

## Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore -->
<table><tr><td align="center"><a href="https://github.com/mtbdeligt"><img src="https://avatars3.githubusercontent.com/u/16447169?v=4" width="100px;" alt="Rob de Ligt"/><br /><sub><b>Rob de Ligt</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mtbdeligt" title="Documentation">📖</a> <a href="#maintenance-mtbdeligt" title="Maintenance">🚧</a></td><td align="center"><a href="https://github.com/malfrancis"><img src="https://avatars0.githubusercontent.com/u/5935221?v=4" width="100px;" alt="Mal"/><br /><sub><b>Mal</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=malfrancis" title="Code">💻</a></td><td align="center"><a href="https://github.com/leitchy"><img src="https://avatars0.githubusercontent.com/u/3417817?v=4" width="100px;" alt="James Leitch"/><br /><sub><b>James Leitch</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=leitchy" title="Code">💻</a></td><td align="center"><a href="https://github.com/mfellows"><img src="https://avatars0.githubusercontent.com/u/8548157?v=4" width="100px;" alt="Max Fellows"/><br /><sub><b>Max Fellows</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mfellows" title="Code">💻</a></td></tr></table>
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!

## Maintainers Reviewers Ambassadors Coaches

The following people are Maintainers, Reviewers, Ambassadors or Coaches.

<table><tr><td align="center"><a href="https://github.com/malfrancis"><img src="https://avatars0.githubusercontent.com/u/5935221?v=4" width="100px;" alt="Mal"/><br /><sub><b>Mal</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=malfrancis" title="Code">💻</a></td><td align="center"><a href="https://github.com/leitchy"><img src="https://avatars0.githubusercontent.com/u/3417817?v=4" width="100px;" alt="James Leitch"/><br /><sub><b>James Leitch</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=leitchy" title="Code">💻</a></td><td align="center"><a href="https://github.com/mfellows"><img src="https://avatars0.githubusercontent.com/u/8548157?v=4" width="100px;" alt="Max Fellows"/><br /><sub><b>Max Fellows</b></sub></a><br /><a href="https://github.com/moja-global/FLINT/commits?author=mfellows" title="Code">💻</a></td></tr></table>

**Maintainers** review and accept proposed changes.
**Reviewers** check proposed changes before they go to the Maintainers.
**Ambassadors** are available to provide training related to this repository.
**Coaches** are available to provide information to new contributors to this repository.

