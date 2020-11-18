# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased on develop]
### Added
- Readthedocs website at [docs.moja.global.com](http://docs.moja.global/) by  [@Tlazypanda](https://github.com/Tlazypanda)
- A welcome bot for new contributors from [@Tlazypanda](https://github.com/Tlazypanda)
- A pull request template from [@Tlazypanda](https://github.com/Tlazypanda)
- Support for outputting properties of DynamicObject-style variables by [@mfellows](https://github.com/mfellows).
- New out of memory handler to give the user more information about the cause of hard crashes in some cases by [@mfellows](https://github.com/mfellows).
- Pre-allocated tuples, tuple collections and persistable vectors in the `RecordAccumulator` from [@mal](https://github.com/malfrancis)
- README badges from [@Tlazypanda](https://github.com/Tlazypanda)
- Support for developing inside a container using VScode from [@leitchy](https://github.com/leitchy)
- A CHANGELOG from [@aornugent](https://github.com/aornugent)

### Changed
- Updated minimum requirements to C++17
- Made raster metadata optional
- Streamlined test modules
- Enabled environment variable expansion in `librarymanager`
- Fixed Poco imports in the GDAL raster reader for Linux systems
- Fixed unrecorded disturbance events in `OutputerStreamFlux`
- Updated the FLINT Dockerfiles (have images been pushed to [https://hub.docker.com/r/mojaglobal/](https://hub.docker.com/r/mojaglobal/)?)

## [Unreleased on master]
**Note:** Ideally this section wouldn't exist. Rather, unreleased work would live on `develop` and any commits to `master` would create a (probably minor) version bump. See [Semantic Versioning](https://semver.org/spec/v2.0.0.html) for details or [A successful branching policy](https://nvie.com/posts/a-successful-git-branching-model/) for motivation.

### Added
- New ['How to use FLINT' guides](https://github.com/moja-global/FLINT/tree/master/How_to_use_FLINT) by [@GeoffRoberts](https://github.com/GeoffRoberts)
- [`fmt`](https://fmt.dev/latest/index.html) library for improved output formatting.
- New TIFF reader for importing spatial data from [@mfellows](https://github.com/mfellows) and
[@mal](https://github.com/malfrancis). The GDAL raster reader now supports all data types and resolutions; testing with large simulations confirms that results are identical to the custom tile/block/cell format.

### Changed
- Updated LICENSE by [@gmajan](https://github.com/gmajan)
- Update README
- Configured SQLite to explicitly read-only / no-mutex to improve read-only performance

### Removed
- https://moja-global.github.io/FLINT GitHub Page


## [1.1.0] - 2019-12-20
Earlier changes TBC

### Added
### Changed
### Removed

## [1.0.1] - 2019-07-30
### Added
### Changed


## [1.0.0] - 2019-05-25
### Added
### Changed
### Removed

[Unreleased on develop]: https://github.com/moja-global/FLINT/compare/1.1.0...develop
[Unreleased on master]: https://github.com/moja-global/FLINT/compare/1.1.0...master
[1.1.0]: https://github.com/moja-global/FLINT/compare/1.0.1...1.1.0
[1.0.1]: https://github.com/moja-global/FLINT/compare/1.0.0...1.0.1
[1.0.0]: https://github.com/moja-global/FLINT/releases/tag/1.0.0
