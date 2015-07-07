# MSUProj #

**MSUProj** is a project to geo transform images of MSU-MR equipment of Russian ERS satellite "Meteor-M".

It is developed specially for [LRPToffLineDecoder](http://meteor.robonuka.ru/for-experts/lrpt-analizer-2015/) to help amateur radio operators get remote sensing data from "Meteor-M No2" and uses [GDAL](http://www.gdal.org/) to operate with raster images.

<img src="http://meteor.robonuka.ru/wp-content/uploads/2015/03/21082014_UTM37_ArcGis.jpg" alt="Meteor-M No2 MSU-MR 1km" style="width: 640px;"/>

# Build #

### 1. Install the required packages ###

- *Compiler* (gcc)
- *CMake*
- *make* or *Ninja*
- *GDAL* development package (libgdal-dev in debian)
- *WIX* for building msi packages on Windows

### 2. Clone ###

    git clone --recursive https://github.com/mentaljam/MSUProj

### 3. Make a building directory ###

    mkdir msuproj_build
    cd msuproj_build

### 4. Compile

Use a *-DARCH={32/64}* argument for CMake to build a 32bit or a 64bit application, or leave to autodetect

With make:

    cmake -DARCH=64 ../msuproj
    make

With ninja:

    cmake -DARCH=64 -G Ninja ../msuproj
    ninja
    
### Build a package or just install ###

Windows:

    make package

or

    cpack -G ZIP

Debian:

    cpack -G DEB
    sudo dpkg -i msuproj*.deb

For full list of CPack generators read [documentation](http://www.cmake.org/Wiki/CMake:CPackPackageGenerators).
