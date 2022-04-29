# RasterMon
Raster Monitoring code for Run Group C with CLAS12

## Using this code at JLab

At JLab, you can find a pre-compiled version in /home/holtrop/bin and the
libraries in /home/holtrop/lib. To use them, you can do the following:
```shell
source /home/holtrop/root/bin/thisroot.sh
export PATH=${PATH};/home/holtrop/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH};/home/holtrop/lib
```

You can now run the code on EVIO files with:
```shell
RasterMon clas_evio_file.evio.000*
```
This will run through the EVIO files you specify on the commandline. You
can also use the GUI to select files. See ```RasterMon --help``` for
commandline options.

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