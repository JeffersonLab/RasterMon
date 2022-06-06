# RasterMon
Raster Monitoring code for Run Group C with CLAS12

![Main window of RasterMon](https://github.com/JeffersonLab/RasterMon/wiki/images/main_window.png "Main window of RasterMon")

## Using this code at JLab

At JLab, on the CLON machines, there is a pre-compiled version in /home/clasrun/Rastermon/bin and the
libraries in /home/clasrun/Rastermon/lib. Using the "clasrun" account, an alias called `rastermon` is
setup to run this code. From a terminal, to run RasterMon with the default ET parameters, you can simply type:
```shell
rastermon
```
Or you can add various options.

If you are not using the "clasrun" account, you can run the code with:
To use them, you can do the following:
```shell
/home/clasrun/RasterMon/rastermon.sh  # Add optional arguments.
```
If you want to setup the environment for the code but not run it, you can source this using the bash shell, after
which you can run the code directly without a script:
```bash
. /home/clasrun/RasterMon/rastermon.sh
RasterMon # arguments
```

The standard behavior of the code is to connect to the ET ring at clondaq6. You can specify a different ET with
options:
```shell
rastermon --et --host clondaq7 --port 11111 --etname /et/clasprod 
```

Or you can analyze a set of files from disk with:
```shell
rastermon /cache/clas12/rg-c/data/clas_999999/clas_999999.evio.*   # replace with proper file path.
```

This will run through the set of EVIO files you specify on the commandline. You
can also use the GUI to select files or switch the ET under the "file" menu.

See ```RasterMon --help``` for details on the commandline options.

## Details

More details for running this code are in the 
[RasterMon GitHub wiki](https://github.com/JeffersonLab/RasterMon/wiki). 