# RasterMon
Raster Monitoring code for Run Group C with CLAS12

## Using this code at JLab

At JLab, on the CLON machines, you can find a pre-compiled version in /home/clasrun/Rastermon/bin and the
libraries in /home/clasrun/Rastermon/lib. To use them, you can do the following:
```shell
# For tcsh, csh:
source Rastermon/setup.csh
# For bash, zsh
source Rastermon/setup.sh
```

You can then start the code and connect to the ET ring on clondaq6 with:
```shell
RasterMon --et --host clondaq6
```

Or you can analyze a set of files with:

```shell
RasterMon /cache/clas12/rg-c/data/clas_999999/clas_999999.evio.*   # replace with proper file path.
```

This will run through the set of EVIO files you specify on the commandline. You
can also use the GUI to select files or switch the ET under the "file" menu.

See ```RasterMon --help``` for details on the commandline options.

## Running RasterMon

Once you have the GUI running, you will find a set of tabs at the top of the screen where you can select
the different sets of histograms. The "scope" tab shows the time evolution of the raster signals. If you "pause"
or "stop" the program, each of the windows can be zoomed, or logy can be changed, just as with any other 
ROOT histogram in a canvas.

At the bottom row of buttons you will also find the "config" button. This brings up a configuration panel 
where you can select the debug verbosity and the conversion from RAW to calibrated histograms. 

The "Scope event buffer depth" in the config panel sets the number of events that are in the circular buffer
for the "Scope" tab. Setting a higher number will give you a longer period of data, but it will also significantly
slow down the window updating, since ROOT needs to re-draw the full graph at each update.

You can set the rate of the window updates with the window refresh rate. Lower numbers need a faster computer
and windowing system to keep up.

![RasterMon Config Panel](http://nuclear.unh.edu/CLAS12/Raster/ConfigDialogScreenShot.png)

## Getting the code.

You need to check out this code from Github with the submodules. The easiest way 
to do this is with --recurse-submodules:

* git clone --recurse-submodules  https://github.com/JeffersonLab/RasterMon.git

Alternatively, you can check this code out in 4 steps, so that you also get the submodules:

* git clone https://github.com/JeffersonLab/RasterMon.git
* cd RasterMin
* git submodule init
* git submodule update

## Building the code.

This code requires a recent version of ROOT, a C++17 compiler, and cmake > 3.5.
You also need your ROOT distribution with the C++17 features enabled. 
This code has been tested on MacOS, MacOS-M1, Ubuntu, Ubuntu-arm and Centos 7.

At JLab, on the ifarms, you can execute:

```shell
module use /apps/modulefiles
module use /group/clas12/packages/local/etc/modulefiles
module load gcc/9.2.0
module load cmake/3.19.4
source /home/holtrop/root/bin/thisroot.sh
```

You can then build this code with:
```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install  ..
make -j8
make install
```

The code will be installed in /path/to/install/bin and /path/to/install/lib