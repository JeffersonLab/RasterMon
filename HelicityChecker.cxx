//
// Created by Maurik Holtrop on 9/2/22.
//
#include <iostream>
#include <cxxopts.hpp>
#include "RasterEvioTool.h"
#include "RasterScalerBank.h"
#include "TFile.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TH1D.h"

int main(int argc, char **argv) {

   std::string help_string =
   " ===============  Helicity Checker ===============\n";

   cxxopts::Options options(argv[0], help_string);
   options
         .positional_help(" infile1 infile2 ...")
         .show_positional_help();

   std::string output_file;
   options.add_options()
         ("d,debug", "Increase debug level")
         ("o,output", "Output file name", cxxopts::value(output_file)->default_value("HelicityChecker.root"),
               "Output ROOT file name.")
         ("i,inputfiles","List of input evio files.",
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

      auto evio = new RasterEvioTool();

      // Add the commandline files to the RasterEvioTool
      if (args.count("inputfiles")) {
         auto &infiles = args["inputfiles"].as<std::vector<std::string>>();
         for (auto &v: infiles) {
            if (debug) cout << "Adding " << v << " \n";
            evio->AddFile(v.c_str());
         }
      }

      TFile *output = new TFile(output_file.c_str(),"RECREATE","Helicity Checker output file.");

      //////////////////////////////////// LOOP CODE STARTS HERE /////////////////////////////////////////////////
      int hel_i = evio->AddChannel(19, 19, 0);  // Helicity
      int sync_i = evio->AddChannel(19, 19, 2);  // Sync
      int quart_i = evio->AddChannel(19, 19, 4);  // Quarted

      auto mg  = new TMultiGraph();
      auto h_dt_sync = new TH1D("h_dt_sync", "#Delta t sync signal flips; #Delta t(ms)", 1000, 0., 100.);
      auto h_sync_delay = new TH1D("h_sync_delay", "#Delta t , Struck Scalers - sync spin flip; #Delta t(ms)", 1000, -10., 10.);
      auto h_dt_stscaler = new TH1D("h_dt_stscaler", "#Delta t between Struck Scalers; #Delta t(ms)", 1000, -1., 60.);

      unsigned long icount=0;
      unsigned long iflipcount = 0;

      int last_sync = -1;
      long last_sync_time = 0;
      long last_struck_scaler_time = 0;

      int n_combine = 100;
      int istruckscaler = 0;
      unsigned long first_timestamp=0;
      std::vector<double> times;
      std::vector<double> beam_charge_asymmetry;
      std::vector<double> beam_charge_asymmetry_gated;
      std::vector<double> clock_asymmetry;
      std::vector<double> clock_asymmetry_gated;



      while( evio->Next() == 0 ){

         int sync_signal = (evio->GetData(sync_i) > 2000 ? 1 : 0);
         if( sync_signal != last_sync && evio->GetTimeStamp() > 0) {
            ++iflipcount;
            if (last_sync != -1 && last_struck_scaler_time != 0) {
               if (abs(4.e-6 * ( long(evio->GetTimeStamp()) - last_struck_scaler_time)) <
                   abs(4.e-6 * ( last_sync_time - last_struck_scaler_time))) {
                  h_sync_delay->Fill(4.e-6 * (last_struck_scaler_time - long(evio->GetTimeStamp())));
               } else {
                  h_sync_delay->Fill(4.e-6 * (last_struck_scaler_time - last_sync_time));
               }
               h_dt_sync->Fill(4.e-6 * (evio->GetTimeStamp() - last_sync_time));
               if(  4.e-6 * (evio->GetTimeStamp() - last_sync_time) > 50. ) {
                  std::cout << "Sync flip too late!! icount = " << icount << "  evt: " << evio->GetEventNumber() << " time: " << evio->GetTimeStamp();
                  std::cout << " sync: " << sync_signal;
                  std::cout << " delay: " << 4.e-6 * (evio->GetTimeStamp() - last_sync_time) << endl;
               }
               last_sync_time = evio->GetTimeStamp();
            }
            last_sync = sync_signal;
         }

         if( evio->fStruckScaler->fIntegrate->size() > 0){   // Struck scaler data is available in this event.
            h_dt_stscaler->Fill( 4.e-6*(evio->GetTimeStamp() - last_struck_scaler_time));
            last_struck_scaler_time = evio->GetTimeStamp();
            if(istruckscaler == 0){
               first_timestamp = evio->fStruckScaler->GetTimeStamp();
            }
            if( istruckscaler%n_combine == 0 && istruckscaler>0 ){
               times.push_back((evio->fStruckScaler->GetTimeStamp() - first_timestamp)*4.e-9);
               double assym =  0;
               ScalerInfo_t &ptr = evio->fStruckScaler->fInfo[I_FCUP_UNGATED];
               if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
                  assym = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                                 double(ptr.plus_integrated + ptr.minus_integrated);
               }else{
                  assym = 0;
               }
               beam_charge_asymmetry.push_back(assym);
               ptr = evio->fStruckScaler->fInfo[I_FCUP_GATED];
               if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
                  assym = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                          double(ptr.plus_integrated + ptr.minus_integrated);
               }else{
                  assym = 0;
               }
               beam_charge_asymmetry_gated.push_back(assym);
               ptr = evio->fStruckScaler->fInfo[I_CLOCK_UNGATED];
               if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
                  assym = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                          double(ptr.plus_integrated + ptr.minus_integrated);
               }else{
                  assym = 0;
               }
               clock_asymmetry.push_back(assym);
               ptr = evio->fStruckScaler->fInfo[I_CLOCK_GATED];
               if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
                  assym = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                          double(ptr.plus_integrated + ptr.minus_integrated);
               }else{
                  assym = 0;
               }
               clock_asymmetry_gated.push_back(assym);
            }
            ++istruckscaler;
         }
         if( evio->fHelicity->size() > 0){
            // std::cout << "Helicity decoder bank  : " << evio->GetEventNumber() << " time: " << evio->GetTimeStamp() << "\n";

         }


         if(evio->GetEventNumber()>0 && evio->GetTimeStamp() > 0) {
            ++icount;
            if(debug > 0){
               if( (icount % 100000) == 0){
                  printf("I: %7lu  Event: %7d \n", icount, evio->GetEventNumber());
               }
            }
         }

      }

      auto gr_bc_asym = new TGraph(times.size(), times.data(), beam_charge_asymmetry.data() );
      gr_bc_asym->SetTitle("Beam Charge Asymmetry");
      gr_bc_asym->Write("BeamChargeAsym");
      auto gr_bc_asym_g = new TGraph(times.size(), times.data(), beam_charge_asymmetry_gated.data() );
      gr_bc_asym_g->SetTitle("Beam Charge Asymmetry Gated");
      gr_bc_asym_g->Write("BeamChargeAsymGated");
      auto gr_cl_asym = new TGraph(times.size(), times.data(), clock_asymmetry.data() );
      gr_cl_asym->SetTitle("Clock Tick Asymmetry");
      gr_cl_asym->Write("ClockAsym");
      auto gr_cl_asym_g = new TGraph(times.size(), times.data(), clock_asymmetry_gated.data() );
      gr_cl_asym_g->SetTitle("Clock Tick Asymmetry Gated");
      gr_cl_asym_g->Write("ClockAsymGated");

      output->Write();
      output->Close();

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   }catch(const cxxopts::OptionException &e){
      cout << e.what() << endl;
      std::cout << options.help() << std::endl;
      return 1;
   }

   return 0;
}