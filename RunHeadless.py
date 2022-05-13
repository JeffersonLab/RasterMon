#!/usr/bin/env python
#
# This is an example Python script to run the RasterMon code "headless" (i.e. without the GUI) on the
# commandline.
#
import sys
import ROOT
ROOT.EnableThreadSafety()

lib_found = ROOT.gSystem.FindDynamicLibrary("libRasterLib")
if len(lib_found):
    ROOT.gSystem.Load("libRasterLib")
else:
    print("Please make sure the libRasterLib (.so, .dylib) is in your LD_LIBRARY_PATH.")
    sys.exit(1)

hists = None
evio = None

def main(argv):

    global hists, evio
    evio = ROOT.RasterEvioTool()
    hists = ROOT.RasterHists(evio)





if __name__ == '__main__':
    import sys
    import time

    if len(sys.argv) < 2:
        print(f"Please specify input file. {sys.argv[0]}")
        sys.exit(1)

    main(sys.argv)


