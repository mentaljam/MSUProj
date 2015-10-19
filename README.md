# MSUProj

**MSUProj** is a project to transform images of MSU-MR equipment of Russian ERS satellite "Meteor-M" to geographical projection.

It is developed specially for [LRPToffLineDecoder](http://meteor.robonuka.ru/for-experts/lrpt-analizer-2015/) to help amateur radio operators get remote sensing data from "Meteor-M No2" and uses [GDAL](http://www.gdal.org/) to operate with raster images.

**MSUProj** contains a command line and a Qt GUI application.

<img src="http://meteor.robonuka.ru/wp-content/uploads/2015/03/21082014_UTM37_ArcGis.jpg" alt="Meteor-M No2 MSU-MR 1km" style="width: 640px;"/>

## Using

For now MSUProj need a **GCP** file produced with **LRPToffLineDecoder**.

**LRPToffLineDecoder** uses the [SGP4](https://en.wikipedia.org/wiki/Simplified_perturbations_models) model and [TLE](https://en.wikipedia.org/wiki/Two-line_element_set) to calculate ground control points (GCP) for transforming MSU-MR images. To produce a GCP file you should set corresponding options in **LRPToffLineDecoder ini file**:

- RoughStartTimeUTC - an image date in format dd.mm.yyyy (for example "21.08.2014")
- TleFileName - a full path to TLE text file (for example "C:\decoder\norad_M2.txt")

TLE contains input parameters for SGP4 model and can be obtained from [celestrak.com](http://www.celestrak.com) on [this page](http://www.celestrak.com/NORAD/elements/weather.txt) or [space-track.org](https://www.space-track.org). Pay attention that TLE is being constantly updated so you need to refresh your TLE file with the last data.

In future releases KMSSProj library will be able to transform images using TLE directly.

## Documentation

Developer documentation is available on [GitHub Pages](http://mentaljam.github.io/MSUProj/).

## Precompiled binaries

- All stable binaries for Windows are in [Releases](https://github.com/mentaljam/MSUProj/releases)
- You also can grab latest development binaries for Windows from [Yandex.Disk](https://yadi.sk/d/5Lbb-1y5hzZdW)

## Localization

You can translate MSUProj-Qt to your language with [Transifex](https://www.transifex.com/projects/p/msuproj/).
[Translations instructions](http://mentaljam.github.io/MSUProj/translations.html).

## Build

#### 1. Install dependencies

**Requared:**
- **Compiler** (tested with GCC and MSVC2013)
- [CMake](https://cmake.org/)
- [OMZModules](https://github.com/mentaljam/MSUProj) - cmake modules for configuring project (read below)
- **make** or [Ninja](http://martine.github.io/ninja/) for GCC
- [GDAL](http://www.gdal.org/) development package with [geos](https://trac.osgeo.org/geos/) and [proj](http://trac.osgeo.org/proj/) support (libgdal-dev in debian)
- [ImageMagick](http://www.imagemagick.org/) for generating icons

**Additional:**
- [Qt](http://qt-project.org/) for building MSUProj-Qt
- [WIX](http://wixtoolset.org/) or [NSIS](http://nsis.sourceforge.net/) for building installer packages on Windows
- [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) for building documentation and help files

#### 2. Install OMZModules

    cd ~/build_dir
    git clone git@github.com:mentaljam/OMZModules.git
    mkdir OMZModules-build
    cd OMZModules-build
    
With make:

    cmake ../OMZModules
    make install

With ninja:

    cmake -G Ninja ../OMZModules
    ninja install


#### 3. Clone MSUProj

    cd ~/build_dir
    git clone git@github.com:mentaljam/MSUProj.git
    mkdir MSUProj_build
    cd MSUProj_build

#### 4. Compile ####

Use a *-DARCH={32/64}* argument for CMake to build a 32bit or a 64bit application, or leave to autodetect

Select which components must be compiled:
- *-DBUILD_CLI={ON/OFF}*
- *-DBUILD_QT={ON/OFF}*
- *-DBUILD_TOOLS={ON/OFF}*

You may want to specify an additional path to gdal lib/ include/ dirs with *-DSEARCH_PREFIX="..."* or *-DCMAKE_SYSTEM_PREFIX_PATH="..."* options

With make:

    cmake -DARCH=64 -DBUILD_QT=ON -DSEARCH_PREFIX="c:/local/" ../MSUProj
    make

With ninja:

    cmake -DARCH=64 -DBUILD_QT=ON -G Ninja -DSEARCH_PREFIX="c:/local/" ../MSUProj
    ninja
    
#### 5. Build a package or just install

Windows:

    cpack -G NSIS

or

    cpack -G ZIP

Debian:

    cpack -G DEB
    sudo dpkg -i msuproj*.deb

For full list of CPack generators read [documentation](http://www.cmake.org/Wiki/CMake:CPackPackageGenerators).
