//
// Created by Maurik Holtrop on 9/2/22.
//
#include <iostream>
#include <cxxopts.hpp>
#include "RasterEvioTool.h"
#include "RasterScalerBank.h"
#include "TFile.h"
#include "TTree.h"
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
            // if (debug) cout << "Adding " << v << " \n";
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
      auto h_dt_stscaler = new TH1D("h_dt_stscaler", "#Delta t between Struck Scalers; #Delta t(ms)", 602, -0.5, 300.5);
      auto h_stscaler_count = new TH1D("h_stscaler_count","Number of scaler readouts in event; N", 2001, -0.5, 2000.5);

      auto tree = new TTree("Helicity", "Helicity scaler tree.");

      unsigned long icount=0;
      unsigned long iflipcount = 0;

      unsigned long event_num = 0;
      tree->Branch("event_num", &event_num);
      unsigned long event_time = 0;
      tree->Branch("event_time", &event_time);

      int sync_signal = 0;
      int last_sync = -1;
      long last_sync_time = 0;
      unsigned long run_start_time = 0;
      int hel_signal = 0;
      int last_hel = -1;
      long last_hel_flip_time = 0;
      int quart_signal = 0;
      int last_quart = -1;
      long last_quart_flip_time = 0;

      tree->Branch("last_sync_time", &last_sync_time);
      long last_struck_scaler_time = 0;
      tree->Branch("last_struck_scaler_time", &last_struck_scaler_time);

      int n_combine = 120;   // 30 Hz, so this should be about 4s.
      int istruckscaler = 0;
      tree->Branch("i_struckscaler", &istruckscaler);
      unsigned long first_timestamp=0;
      tree->Branch("first_timestamp", &first_timestamp);
      double relative_time=0;
      tree->Branch("relative_timestamp", &relative_time);
      double fcup = 0;
      tree->Branch("fcup", &fcup);
      double fcup_gated = 0;
      tree->Branch("fcup_gated", fcup_gated);
      double clock = 0;
      tree->Branch("clock", clock);
      double clock_gated = 0;
      tree->Branch("clock_gated", clock_gated);
      bool hel_evt = 0;
      tree->Branch("hel_evt", &hel_evt);
      bool helicity = 0;
      tree->Branch("helicity", &helicity);
      bool helicity_valid = 0;
      tree->Branch("helicity_valid", &helicity_valid);
      bool helicity_raw = 0;
      tree->Branch("helicity_raw", &helicity_raw);

      double fcup_plus_int = 0;
      tree->Branch("fcup_plus_int", &fcup_plus_int);
      double fcup_plus_gated_int = 0;
      tree->Branch("fcup_plus_gated_int", fcup_plus_gated_int);
      double clock_plus_int = 0;
      tree->Branch("clock_plus_int", clock_plus_int);
      double clock_plus_gated_int = 0;
      tree->Branch("clock_plus_gated_int", clock_plus_gated_int);
      double fcup_min_int = 0;
      tree->Branch("fcup_min_int", &fcup_min_int);
      double fcup_min_gated_int = 0;
      tree->Branch("fcup_min_gated_int", fcup_min_gated_int);
      double clock_min_int = 0;
      tree->Branch("clock_min_int", clock_min_int);
      double clock_min_gated_int = 0;
      tree->Branch("clock_min_gated_int", clock_min_gated_int);

      double assym_fcup =  0;
      tree->Branch("assym_fcup", &assym_fcup);
      double assym_fcup_gated =  0;
      tree->Branch("assym_fcup_gated", &assym_fcup_gated);
      double assym_clock =  0;
      tree->Branch("assym_clock", &assym_clock);
      double assym_clock_gated =  0;
      tree->Branch("assym_clock_gated", &assym_clock_gated);

      std::vector<double> times;
      const int ASSYM_ALLOC_SIZE = 6500;
      times.reserve(ASSYM_ALLOC_SIZE);
      std::vector<double> beam_charge_asymmetry;
      beam_charge_asymmetry.reserve(ASSYM_ALLOC_SIZE);
      std::vector<double> beam_charge_asymmetry_gated;
      beam_charge_asymmetry_gated.reserve(ASSYM_ALLOC_SIZE);
      std::vector<double> clock_asymmetry;
      clock_asymmetry.reserve(ASSYM_ALLOC_SIZE);
      std::vector<double> clock_asymmetry_gated;
      clock_asymmetry_gated.reserve(ASSYM_ALLOC_SIZE);

      evio->fStruckScaler->fDebug = 0x01;

//      tree->Branch("sync_signal", &sync_signal);
      int n_channel_0 = 0;
      tree->Branch("n_channel_0", &n_channel_0);
      int n_channel_2 = 0;
      tree->Branch("n_channel_2", &n_channel_2);
      double dt_st_scaler;
      tree->Branch("dt_st_scaler", &dt_st_scaler);
      double sync_delay = 0;
      tree->Branch("sync_delay", &sync_delay);
      double dt_sync = 0;
      tree->Branch("dt_sync", dt_sync);
      bool flip_bit = 0;
      tree->Branch("flip_bit", &flip_bit);

      unsigned int n_rising_edge_last = 0;
      unsigned int n_falling_edge_last = 0;
      Helicity_Decoder_t last_helicity_decoder;
      unsigned int last_helicity_state = 0;
      unsigned int last_helicity_state_at_start = 0;
      unsigned int last_helicity_seed = 0;

      while( evio->Next() == 0 ){
         event_num = evio->GetEventNumber();
         event_time = evio->GetTimeStamp();
         if(event_num == 90) run_start_time = evio->GetTimeStamp();
         hel_evt = evio->GetHelicity();

         flip_bit = evio->fStruckScaler->fTimeMarkerFlipped*1;

         hel_signal   = (evio->GetData(hel_i  ) < 2000 ? 0 : 1);
         sync_signal  = (evio->GetData(sync_i ) < 2000 ? 0 : 1);
         quart_signal = (evio->GetData(quart_i) < 2000 ? 0 : 1);
         if( hel_signal != last_hel && evio->GetTimeStamp() > 0) {
            if(sync_signal == last_sync){
               cout << "!!!!!!HELICITY FLIP BUT THERE WAS NO SYNC!!!!!! ";
               cout << evio->GetEventNumber() << endl;
            }
            last_hel = hel_signal;
         }

         if( sync_signal != last_sync && evio->GetTimeStamp() > 0) {
            ++iflipcount;
            if (last_sync != -1 && last_struck_scaler_time != 0) {
               if (abs(4.e-6 * ( long(evio->GetTimeStamp()) - last_struck_scaler_time)) <
                   abs(4.e-6 * ( last_sync_time - last_struck_scaler_time))) {
                  sync_delay = 4.e-6 * (last_struck_scaler_time - long(evio->GetTimeStamp()));
               } else {
                  sync_delay = 4.e-6 * (last_struck_scaler_time - last_sync_time);
               }
               h_sync_delay->Fill(sync_delay);
               dt_sync = 4.e-6 * (evio->GetTimeStamp() - last_sync_time);
               h_dt_sync->Fill(dt_sync);
               if(debug > 1 &&  4.e-6 * (evio->GetTimeStamp() - last_sync_time) > 50. ) {
                  std::cout << "Sync flip too late!! icount = " << icount << "  evt: " << evio->GetEventNumber() << " time: " << evio->GetTimeStamp();
                  std::cout << " sync: " << sync_signal;
                  std::cout << " delay: " << 4.e-6 * (evio->GetTimeStamp() - last_sync_time) << endl;
               }
               last_sync_time = evio->GetTimeStamp();
            }
            if(debug) {
               cout << "SYNC:     " << setw(9) << evio->GetEventNumber() << " ";
               if (last_sync == 0 && sync_signal == 1) cout << "⬆︎ ";
               else if (last_sync == 1 && sync_signal == 0) cout << "⬇︎︎ ";
               else cout << "? ";
               printf(" dt: %10.4f  ΔT: %8.4f", (event_time - run_start_time) * 4e-6, dt_sync);
               cout << "  Q: " << quart_signal << " S: " << sync_signal << " H: " << hel_signal << endl;
            }
            last_sync = sync_signal;
         }

         if(evio->fHelicity->size() > 0){
            for(int idecode = 0; idecode <  evio->fHelicity->size(); ++idecode) {
               if (evio->fHelicity->fDecoder->At(idecode).n_falling_edge != n_falling_edge_last ||
                   evio->fHelicity->fDecoder->At(idecode).n_rising_edge != n_rising_edge_last) {
                  n_falling_edge_last = evio->fHelicity->fDecoder->At(idecode).n_falling_edge;
                  n_rising_edge_last = evio->fHelicity->fDecoder->At(idecode).n_rising_edge;
               }
               if (evio->fHelicity->fDecoder->At(idecode) != last_helicity_decoder ) {
                  if(debug) {
                     cout << "DECODER   " << setw(9)
                          << evio->GetEventNumber() - evio->fHelicity->fDecoder->size() + idecode + 1 << " ";
                     if (evio->fHelicity->fDecoder->At(idecode).helicity_state & 0x01) cout << "⬇ ︎";
                     else cout << "⬆︎ ";
                     printf(" dt: %10.4f ", (event_time - run_start_time) * 4e-6);
                     printf(" %7d  %7d  %6d  %6d  %9d  %9d ",
                            evio->fHelicity->fDecoder->At(idecode).n_falling_edge,
                            evio->fHelicity->fDecoder->At(idecode).n_rising_edge,
                            evio->fHelicity->fDecoder->At(idecode).n_pair_sync,
                            evio->fHelicity->fDecoder->At(idecode).n_pattern_sync,
                            evio->fHelicity->fDecoder->At(idecode).time_since_start_of_stable,
                            evio->fHelicity->fDecoder->At(idecode).time_since_end_of_stable);
                     cout << " Count: " << evio->fHelicity->fDecoder->At(idecode).status.pattern_phase_count;
                     cout << " Pat: " << evio->fHelicity->fDecoder->At(idecode).status.pattern_sync;
                     cout << " Pair: " << evio->fHelicity->fDecoder->At(idecode).status.pair_sync;
                     cout << " Hs: " << evio->fHelicity->fDecoder->At(idecode).status.helicity_at_pattern_start;
                     cout << " H: " << evio->fHelicity->fDecoder->At(idecode).status.helicity << endl;
                  }
                  // cout << *evio->fHelicity->fDecoder.back();
                  last_helicity_decoder = evio->fHelicity->fDecoder->At(idecode);
                  last_helicity_state = evio->fHelicity->fDecoder->At(idecode).helicity_state;
               }
               if (evio->fHelicity->fDecoder->At(idecode).helicity_state_at_start_of_pattern !=
                   last_helicity_state_at_start) {
                  if(debug) {
                     cout << "DECODER   " << setw(9)
                          << evio->GetEventNumber() - evio->fHelicity->fDecoder->size() + idecode + 1 << "   ";
                     printf(" dt: %10.4f ", (event_time - run_start_time) * 4e-6);
                     cout << " state: "
                          << std::bitset<32>(evio->fHelicity->fDecoder->At(idecode).helicity_state_at_start_of_pattern);
                     cout << " Count: " << evio->fHelicity->fDecoder->At(idecode).status.pattern_phase_count;
                     cout << " Hs:" << evio->fHelicity->fDecoder->At(idecode).status.helicity_at_pattern_start;
                     cout << " H: " << evio->fHelicity->fDecoder->At(idecode).status.helicity << endl;
                  }
                  //  cout << *evio->fHelicity->fDecoder.back();
                  last_helicity_state_at_start = evio->fHelicity->fDecoder->At(idecode).helicity_state_at_start_of_pattern;
               }
            }
         }

         if(evio->fStruckScaler->DataSize() > 0){
            n_channel_0 = 0;
            n_channel_2 = 0;
            for(int ii=0; ii< evio->fStruckScaler->fUngatedScalers->size(); ++ii){
               if(evio->fStruckScaler->fUngatedScalers->At(ii).channel_id == 0) ++n_channel_0;
               if(evio->fStruckScaler->fUngatedScalers->At(ii).channel_id == 2) ++n_channel_2;
            }
            h_stscaler_count->Fill(n_channel_2);
         }

         if( evio->fStruckScaler->fIntegrate->size() > 0){   // Struck scaler data is available in this event.
            dt_st_scaler = 4.e-6*( double(evio->GetTimeStamp()) - double(last_struck_scaler_time));
            h_dt_stscaler->Fill( dt_st_scaler);

            if(istruckscaler == 0){
               first_timestamp = evio->fStruckScaler->GetTimeStamp();
            }
            ScalerInfo_t &ptr = evio->fStruckScaler->fInfo[I_FCUP_UNGATED];
            fcup = ptr.value;
            helicity = ptr.helicity;
            helicity_valid = ptr.helicity_valid;
            helicity_raw = ptr.helicity_raw;
            fcup_plus_int = ptr.plus_integrated;
            fcup_min_int = ptr.minus_integrated;
            relative_time = (evio->fStruckScaler->GetTimeStamp() - first_timestamp)*4.e-6;
            //printf("Event:    %9lu    dt: %8.4f  ΔT: %8.4f   fcup ug: %9.2f  HE: %1d  H: %1d  HV: %1d  HR: %1d\n",
            //       event_num, relative_time, dt_st_scaler, fcup, hel_evt, helicity, helicity_valid, helicity_raw);

            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               assym_fcup = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               assym_fcup = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_FCUP_GATED];
            fcup_gated = ptr.value;
//            printf("Evt: %9lu  R-time: %9.6f  fcup ga: %9.2f  H: %1d \n", event_num, relative_time, fcup_gated, helicity);
            fcup_plus_gated_int  = ptr.plus_integrated;
            fcup_min_gated_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               assym_fcup_gated = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               assym_fcup_gated = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_CLOCK_UNGATED];
            clock = ptr.value;
            clock_plus_int = ptr.plus_integrated;
            clock_min_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               assym_clock = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               assym_clock = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_CLOCK_GATED];
            clock_gated = ptr.value;
            clock_plus_gated_int = ptr.plus_integrated;
            clock_min_gated_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               assym_clock_gated = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               assym_clock_gated = 0;
            }


            if( istruckscaler%n_combine == 0 && istruckscaler>0 ){
               times.push_back(relative_time);
               beam_charge_asymmetry.push_back(assym_fcup);
               beam_charge_asymmetry_gated.push_back(assym_fcup_gated);
               clock_asymmetry.push_back(assym_clock);
               clock_asymmetry_gated.push_back(assym_clock_gated);
            }

            tree->Fill();
            last_struck_scaler_time = evio->GetTimeStamp();
            ++istruckscaler;
         }
         if( evio->fHelicity->size() > 0){
            // std::cout << "Helicity decoder bank  : " << evio->GetEventNumber() << " time: " << evio->GetTimeStamp() << "\n";

         }


         if(evio->GetEventNumber()>0 && evio->GetTimeStamp() > 0) {
            ++icount;
            if(debug > 0){
               if( (icount % 100000) == 0){
                  printf("I: %8lu  Event: %8d  Beam Charge Assym: %9.6f  %9.6f  %9.6f  %9.6f\n", icount, evio->GetEventNumber(),
                         beam_charge_asymmetry.back(), beam_charge_asymmetry_gated.back(), clock_asymmetry.back(), clock_asymmetry_gated.back());

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