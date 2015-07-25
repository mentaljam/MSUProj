# MSUProj #

**MSUProj** is a project to geo transform images of MSU-MR equipment of Russian ERS satellite "Meteor-M".

It is developed specially for [LRPToffLineDecoder](http://meteor.robonuka.ru/for-experts/lrpt-analizer-2015/) to help amateur radio operators get remote sensing data from "Meteor-M No2" and uses [GDAL](http://www.gdal.org/) to operate with raster images.

**MSUProj** contains a command line and a Qt GUI application.

<img src="http://meteor.robonuka.ru/wp-content/uploads/2015/03/21082014_UTM37_ArcGis.jpg" alt="Meteor-M No2 MSU-MR 1km" style="width: 640px;"/>

## Precompiled binaries ##

- All stable binaries for Windows are in [Releases](https://github.com/mentaljam/MSUProj/releases)
- You also can grab latest development binaries for Windows from [Yandex Disk](https://disk.yandex.ru/client/disk/MSUProj-Qt)

## Localisation ##

You can translate MSUProj-Qt to your language with [Transifex](https://www.transifex.com/projects/p/msuproj/).

## Build ##

#### 1. Install the required packages ###

- *Compiler* (tested with GCC and MSVC2013)
- *CMake*
- *make* or *Ninja* for GCC
- *GDAL* development package with geos and proj support (libgdal-dev in debian)
- *WIX* or *NSIS* for building installer packages on Windows

#### 2. Clone ###

    git clone --recursive https://github.com/mentaljam/MSUProj

#### 3. Make a building directory ###

    mkdir msuproj_build
    cd msuproj_build

#### 4. Compile

Use a *-DARCH={32/64}* argument for CMake to build a 32bit or a 64bit application, or leave to autodetect

Select which components must be compiled:
- *-DBUILD_CLI={ON/OFF}*
- *-DBUILD_QT={ON/OFF}*
- *-DBUILD_TOOLS={ON/OFF}*

You may want to specify an additional path to gdal lib/ include/ dirs with *-DSEARCH_PREFIX="..."* or *-DCMAKE_SYSTEM_PREFIX_PATH="..."* options

With make:

    cmake -DARCH=64 -DBUILD_QT=ON -DSEARCH_PREFIX="c:/local/" ../msuproj
    make

With ninja:

    cmake -DARCH=64 -DBUILD_QT=ON -G Ninja -DSEARCH_PREFIX="c:/local/" ../msuproj
    ninja
    
#### Build a package or just install ###

Windows:

    cpack -G NSIS

or

    cpack -G ZIP

Debian:

    cpack -G DEB
    sudo dpkg -i msuproj*.deb

For full list of CPack generators read [documentation](http://www.cmake.org/Wiki/CMake:CPackPackageGenerators).
