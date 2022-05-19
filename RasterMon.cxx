//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        RasterMon
//
// Author: Maurik Holtrop  - UNH - 2022
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Some notes:
// In general you want a GUI to be multithreaded. It turns out that this is trickier than it should be, for many
// reasons, the biggest of which is that we are dealing with lots of legacy code. XLib, the windowing used on Linux
// is still not properly thread safe! (Yes, Metal, Cocoa and Windows are, but we use Linux at the lab.)
// We are not using ROOT 7 yet, and ROOT 6.x is also not properly thread safe, see
// https://root.cern.ch/root/htmldoc/guides/users-guide/Threads.html
// Also note that officially the TThread class in ROOT is obsolete, so probably don't use it.
//
// The main implementations issues then for a GUI code are that *all the GUI stuff* has to be done from the main
// thread. This can be okay with cooperative multi-tasking, i.e. a loop that checks for signals, which is how the
// GUI is implemented, Qt style. (I am not using Qt, though I would like to, because of the embedding of a TCanvas
// in a Qt app, which seems fragile at this point. ROOT7 and QtWeb may improve this in the the future.)
//
// TODO:: Currently the histograms are not following the ROOT::TThreadedObject<TH1D> to create the histograms.
//        We are filling histograms in a single thread right now.
//
// TODO:: Port this to ROOT7.
//        Useful thread: https://root-forum.cern.ch/t/is-one-canvas-per-thread-possible/47014/27
//

#include "RasterMon.h"
#include "TRint.h"

extern void Initialize_Histograms(RasterHists *r, RasterEvioTool *e);

int main(int argc, char **argv) {
   ROOT::EnableThreadSafety();

   // This is a nicer way to do options in C++. See cxxopts.hpp file.
   string help_string =
         " ===============  RasterMon ===============\n"
         " This is a GUI code that will monitor the raster for CLAS12, which is used by Run Group C. \n"
         " You should be able to just run the code and the defaults will work with the CLAS12 ET ring.\n"
         " You can also supply a file name, or a list of filenames, on the command line, or through the GUI \n"
         " (File -> Open ) to have the code analyze the data in these EVIO files.\n\n"

         " Version: 1.0.0, using ROOT version: ";
   help_string += gROOT->GetVersion();
   help_string += "\n Compiled with " __VERSION__ "\n";

   cxxopts::Options options(argv[0], help_string);
   options
         .positional_help(" infile1 infile2 ...")
         .show_positional_help();

   string host;
   unsigned int port = 11111;
   string etname;
   string config_file;

   options.add_options()
      ("d,debug", "Increase debug level")
      ("q,quiet", "Run extra quiet.")
      ("et", "Connect to ET instead of reading from file. ")
      ("host", "Host computer for the ET system, default: " ET_DEFAULT_HOST,
            cxxopts::value(host)->default_value(ET_DEFAULT_HOST))
      ("port", "Port to use for the ET system, default: " + to_string(ET_DEFAULT_PORT),
            cxxopts::value(port)->default_value(to_string(ET_DEFAULT_PORT)))
      ("etname", "filename for ET system direct reads, default: " ET_DEFAULT_NAME,
            cxxopts::value(etname)->default_value(ET_DEFAULT_NAME))
      ("config", "Configuration root macro file. default (none)",
            cxxopts::value(config_file)->default_value(""))
      ("inputfiles","List of input evio files. The -i is optional. ",
            cxxopts::value<std::vector<std::string>>())
      ("help", "Print help")
      ;

   options.parse_positional({"inputfiles"});

   try {
      auto args = options.parse(argc, argv);
      if (args.count("help")) {
         std::cout << options.help() << std::endl;
         exit(0);
      }

      int debug = args.count("debug");       // Debug: 0 = info,  1 = more info, 2+ debug info.
      if (args.count("quiet") > 0) debug = -1; // Debug: -1 = no info.
      
      TApplication theApp("App", &argc, argv);
      // TRint theApp("App", &argc, argv);

      auto evio = new RasterEvioTool();
      auto RHists = new RasterHists(evio);
      auto rastermon = new RasterMonGui(RHists, gClient->GetRoot(), 800, 600);

      if (debug == 1) evio->fDebug = 0;
      if (debug > 1) evio->fDebug = EvioTool::EvioTool_Debug_Info;
      if (debug > 2) evio->fDebug += EvioTool::EvioTool_Debug_Info2;
      if (debug > 3) evio->fDebug += EvioTool::EvioTool_Debug_L1;
      if (debug > 4) evio->fDebug += EvioTool::EvioTool_Debug_L2;
      RHists->SetDebug(debug);

      // Parse the Config file FIRST. Then have command line options potentially override some of them (i.e. for ET)
//      if( args.count("config")){
//         cout << "Config file: " << config_file << " specified on command line. Parsing it.\n";
//         cout << "This file exists? " << std::filesystem::exists( std::filesystem::path(config_file)) << "\n";
//         string process_line = ".x " + config_file; // + "++";
//         gROOT->ProcessLine(".x tests/config.C");
//       void *test_ptr;
//      gROOT->LoadMacro("tests/config.C");
//      }



      Default_Initialize_Histograms(RHists, evio);
      RHists->SetupData();


      // Add the commandline files to the RasterEvioTool
      if (args.count("inputfiles")) {
         auto &infiles = args["inputfiles"].as<std::vector<std::string>>();
         for (auto &v: infiles) {
            if (debug) cout << "Adding " << v << " \n";
            evio->AddFile(v.c_str());
         }
      }

      if(args.count("host") || evio->fETHost.empty() ) evio->SetETHost(host);
      if(args.count("port") || evio->fETPort == 0) evio->SetETPort(port);
      if(args.count("etname") || evio->fETName.empty()) evio->SetETName(etname);

      if (args.count("et")) {
         if (debug)
            cout << "Using the ET system with host: " << host << ", port: " << port << " , et file: " << etname
                 << ". \n";
         int stat = evio->OpenEt("RasterMon",etname, host, port);
         if (stat != 0) {
            cout << "ERROR -- could not attach to ET system. abort. \n";
            return (3);
         }
      }

      rastermon->SetupGUI();
      rastermon->fDebug = debug;
      theApp.Run();

   }catch(const cxxopts::OptionException &e){
      cout << e.what() << endl;
      std::cout << options.help() << std::endl;
      return 1;
   }

   return 0;
}
