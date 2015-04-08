# MSUGeo #

**MSUGeo** is a project to geo transform images of MSU-MR equipment of Russian ERS satellite "Meteor-M".

It is developed specially for [LRPToffLineDecoder](http://meteor.robonuka.ru/for-experts/lrpt-analizer-2015/) to help amateur radio operators get remote sensing data from "Meteor-M No2" and uses [GDAL](http://www.gdal.org/) to operate with raster images.

# Build #

### 1. Install the required packages ###

- *Compiler* (gcc)
- *CMake*
- *make* or *Ninja*
- *GDAL* development package (libgdal-dev in debian)
- *WIX* for building msi packages on Windows

### 2. Clone ###

    git clone --recursive https://github.com/mentaljam/msugeo

### 3. Make a building directory ###

    mkdir msugeo_build
    cd msugeo_build

### 4. Compile

Use a *-D IF64={YES/NO}* argument for CMake to build a 32bit or a 64bit application, default is 32bit

With make:

    cmake -D IF64=YES ../msugeo
    make

With ninja:

    cmake -D IF64=YES -G Ninja ../msugeo
    ninja
    
### Build a package or just install ###

Windows:

    make package

or

    cpack -G ZIP

Debian:

    cpack -G DEB
    sudo dpkg -i msugeo*.deb

For full list of CPack generators read [documentation](http://www.cmake.org/Wiki/CMake:CPackPackageGenerators).