#include "RasterMon.h"

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

   options.add_options()
      ("d,debug", "Increase debug level")
      ("q,quiet", "Run extra quiet.")
      ("t,et", "Connect to ET instead of reading from file. ")
      ("h, host", "Host computer for the ET system, default: " ET_DEFAULT_HOST,
            cxxopts::value(host)->default_value(ET_DEFAULT_HOST))
      ("p, port", "Port to use for the ET system, default: " + to_string(ET_DEFAULT_PORT),
            cxxopts::value(port)->default_value(to_string(ET_DEFAULT_PORT)))
      ("f, etname", "filename for ET system direct reads, default: " ET_DEFAULT_NAME,
            cxxopts::value(etname)->default_value(ET_DEFAULT_NAME))
      ("i,inputfiles","List of input evio files. The -i is optional. ",
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

      auto evio = new RasterEvioTool();
      evio->SetETHost(host);
      evio->SetETPort(port);
      evio->SetETName(etname);

      if (debug == 1) evio->fDebug = 0;
      if (debug > 1) evio->fDebug = EvioTool::EvioTool_Debug_Info;
      if (debug > 2) evio->fDebug += EvioTool::EvioTool_Debug_Info2;
      if (debug > 3) evio->fDebug += EvioTool::EvioTool_Debug_L1;
      if (debug > 4) evio->fDebug += EvioTool::EvioTool_Debug_L2;

      // Add the commandline files to the RasterEvioTool
      if (args.count("inputfiles")) {
         auto &infiles = args["inputfiles"].as<std::vector<std::string>>();
         for (auto &v: infiles) {
            if (debug) cout << "Adding " << v << " \n";
            evio->AddFile(v.c_str());
         }
      }

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

      auto RHists = new RasterHists(evio);
      RHists->SetDebug(debug);
      auto rastermon = new RasterMonGui(RHists, gClient->GetRoot(), 800, 600);
      rastermon->fDebug = debug;
      theApp.Run();
   }catch(exception e){
      cout << e.what() << endl;
      std::cout << options.help() << std::endl;
      return 1;
   }

   return 0;
}
