# RasterMon
Raster Monitoring code for Run Group C with CLAS12

## Getting the code.

You need to check this code out in 4 steps, so that you also get the EvioTool submodule:

* git clone https://github.com/JeffersonLab/RasterMon.git
* cd RasterMin
* git submodule init
* git submodule update

## Building the code.

This code requires a recent version of ROOT, a C++17 compiler, and cmake > 3.5.
You also need your ROOT distribution with the C++17 features enabled. 

At JLab, on the ifarms, you can execute:

```bash
module use /apps/modulefiles
module use /group/clas12/packages/local/etc/modulefiles
module load gcc/9.2.0
module load cmake/3.19.4
source /home/holtrop/root/bin/thisroot.sh
```

You can then build this code with:
```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install  ..
make -j8
make install
```

The code will be installed in /path/to/install/bin and /path/to/install/lib