#!/bin/bash

# Setup and run the RasterMon code.
export PATH=/home/clasrun/RasterMon/bin:/u/apps/gcc/9.3.0/bin:/u/apps/cmake/3.22.1/bin/:${PATH}
export LD_LIBRARY_PATH=/home/clasrun/RasterMon/lib:/u/apps/gcc/9.3.0/lib64:/u/apps/gcc/9.3.0/lib:${LD_LIBRARY_PATH}
export VIRTUAL_ENV_DISABLE_PROMPT="true"
source /home/clasrun/Python3/bin/activate
source /home/clasrun/RasterMon/root/bin/thisroot.sh 

# The "logentry" executable from the accelerator apparently requires oracle 8:
export JAVA_HOME=/usr/clas12/third-party-libs/jdk/1.8.0_31
export PATH=${JAVA_HOME}/bin:${PATH}

if [[ "$(basename -- "$0")" == "rastermon.sh" ]]; then
    /home/clasrun/RasterMon/bin/RasterMon $@
else
    echo "Setup Rastermon for bash."
fi
