Open Asset Import Library (assimp)
==================================

Open Asset Import Library is a library that loads various 3D file formats into a shared, in-memory format. It supports more than __40 file formats__ for import and a growing selection of file formats for export.

### Current project status ###
![C/C++ CI](https://github.com/assimp/assimp/workflows/C/C++%20CI/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/9973693b7bdd4543b07084d5d9cf4745)](https://www.codacy.com/gh/assimp/assimp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=assimp/assimp&amp;utm_campaign=Badge_Grade)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=assimp_assimp&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=assimp_assimp)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/assimp/assimp.svg)](http://isitmaintained.com/project/assimp/assimp "Average time to resolve an issue")
[![Percentage of issues still open](http://isitmaintained.com/badge/open/assimp/assimp.svg)](http://isitmaintained.com/project/assimp/assimp "Percentage of issues still open")
[![Gurubase](https://img.shields.io/badge/Gurubase-Ask%20assimp%20Guru-006BFF)](https://gurubase.io/g/assimp)
[![Financial Contributors on Open Collective](https://opencollective.com/assimp/all/badge.svg?label=financial+contributors)](https://opencollective.com/assimp)
<br>

APIs are provided for C and C++. Various bindings exist to other languages (C#, Java, Python, Delphi, D). Assimp also runs on Android and iOS.
Additionally, assimp features various __mesh post-processing tools__: normals and tangent space generation, triangulation, vertex cache locality optimization, removal of degenerate primitives and duplicate vertices, sorting by primitive type, merging of redundant materials and many more.

## Project activity ##
![Alt](https://repobeats.axiom.co/api/embed/997f84e5f9fcf772da1e687f3a4f3a8afdbf4cf0.svg "Repobeats analytics image")

### Documentation ###
Read [our latest documentation](https://the-asset-importer-lib-documentation.readthedocs.io/en/latest/).

### Pre-built binaries ###
Download binaries from [our Itchi Projectspace](https://kimkulling.itch.io/the-asset-importer-lib).

### Test data ###
Clone [our model database](https://github.com/assimp/assimp-mdb).

### Communities ###
- Ask questions at [the Assimp Discussion Board](https://github.com/assimp/assimp/discussions).
- Find us on [https://discord.gg/s9KJfaem](https://discord.gg/kKazXMXDy2)
- Ask [the Assimp community on Reddit](https://www.reddit.com/r/Assimp/).
- Ask on [StackOverflow with the assimp-tag](http://stackoverflow.com/questions/tagged/assimp?sort=newest).
- Nothing has worked? File a question or an issue report at [The Assimp-Issue Tracker](https://github.com/assimp/assimp/issues)

#### Supported file formats ####
See [the complete list of supported formats](https://github.com/assimp/assimp/blob/master/doc/Fileformats.md).

### Building ###
Start by reading [our build instructions](https://github.com/assimp/assimp/blob/master/Build.md). We are available in vcpkg, and our build system is CMake; if you used CMake before there is a good chance you know what to do.

### Ports ###
* [Android](port/AndroidJNI/README.md)
* [Python](port/PyAssimp/README.md)
* [.NET](https://github.com/Saalvage/AssimpNetter)
* [Pascal](port/AssimpPascal/Readme.md)
* [Javascript (Alpha)](https://github.com/makc/assimp2json)
* [Javascript/Node.js Interface](https://github.com/kovacsv/assimpjs)
* [Unity 3d Plugin](https://ricardoreis.net/trilib-2/)
* [Unreal Engine Plugin](https://github.com/irajsb/UE4_Assimp/)
* [JVM](https://github.com/kotlin-graphics/assimp) Full JVM port (current [status](https://github.com/kotlin-graphics/assimp/wiki/Status))
* [HAXE-Port](https://github.com/longde123/assimp-haxe) The Assimp-HAXE-port.
* [Rust](https://github.com/jkvargas/russimp)

### Other tools ###
[Qt5-ModelViewer](https://github.com/sharjith/ModelViewer-Qt5) is a powerful viewer based on Qt5 and Assimp's import and export abilities.<br>
[Assimp-Viewer](https://github.com/assimp/assimp_view) is an experimental implementation for an Asset-Viewer based on ImGUI and Assimp (experimental).

#### Repository structure ####
Open Asset Import Library is implemented in C++. The directory structure looks like this:

	/code		Source code
	/contrib	Third-party libraries
	/doc		Documentation (Doxygen source and pre-compiled docs)
	/fuzz           Contains the test code for the Google Fuzzer project
	/include	Public header C and C++ header files
	/scripts 	Scripts are used to generate the loading code for some formats
	/port		Ports to other languages and scripts to maintain those.
	/test		Unit- and regression tests, test suite of models
	/tools		Tools (old assimp viewer, command line `assimp`)
	/samples	A small number of samples to illustrate possible use cases for Assimp

The source code is organized in the following way:

	code/Common			The base implementation for importers and the infrastructure
	code/CApi                       Special implementations which are only used for the C-API
	code/Geometry                   A collection of geometry tools
	code/Material                   The material system
	code/PBR                        An exporter for physical-based models
	code/PostProcessing		The post-processing steps
	code/AssetLib/<FormatName>	Implementation for import and export of the format

### Contributing ###
I would greatly appreciate contributing to assimp. The easiest way to get involved is to submit
a pull request with your changes against the main repository's `master` branch.

## Contributors

### Code Contributors

This project exists thanks to all the people who contribute. [[Contribute](CONTRIBUTING.md)].

<a href="https://github.com/assimp/assimp/graphs/contributors"><img src="https://opencollective.com/assimp/contributors.svg?width=890&button=false" /></a>

### Financial Contributors

Become a financial contributor and help us sustain our community. [[Contribute](https://opencollective.com/assimp/contribute)]

#### Individuals

<a href="https://opencollective.com/assimp"><img src="https://opencollective.com/assimp/individuals.svg?width=890"></a>


#### Organizations

You can support the project with your organization. Your logo will show up here with a link to your website. [[Contribute](https://opencollective.com/assimp/contribute)]

<a href="https://opencollective.com/assimp/organization/0/website"><img src="https://opencollective.com/assimp/organization/0/avatar.svg"></a>

### License ###
Our license is based on the modified, __3-clause BSD__-License.

An _informal_ summary is: do whatever you want, but include Assimp's license text with your product -
and don't sue us if our code doesn't work. Note that, unlike LGPLed code, you may link statically to Assimp.
For the legal details, see the `LICENSE` file.
