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
         ("q,quiet", "Supress update message.")
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
      int quiet = args.count("quiet");

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
      auto h_dt_stscaler = new TH1D("h_dt_stscaler", "#Delta t between Struck Scalers; #Delta t(ms)", 1000, -0.05, 99.95);
      auto h_stscaler_count = new TH1D("h_stscaler_count","Number of scaler readouts in event; N", 2001, -0.5, 2000.5);

      auto tree = new TTree("Helicity", "Helicity scaler tree.");

      struct Tree_Struct_t {
         unsigned long event_num = 0;
         unsigned long event_time = 0;
         unsigned long last_sync_time_store = 0;
         unsigned long last_struck_scaler_time_store = 0;
         int i_struckscaler_store= 0;
         unsigned long struck_scaler_first_timestamp=0;
         double relative_time=0;
         double fcup = 0;
         double fcup_gated = 0;
         double clock = 0;
         double clock_gated = 0;
         bool hel_evt = 0;
         bool helicity = 0;
         bool helicity_valid = 0;
         bool helicity_raw = 0;
         int  raw_hel = 0;
         int  raw_sync = 0;
         int  raw_quad = 0;
         int  raw_is_sync = 0;
         int  raw_n_sync_flips = 0;
         int  raw_quad_phase = 0;
         int  is_struck_scaler = 0;
         double fcup_plus_int = 0;
         double fcup_plus_gated_int = 0;
         double clock_plus_int = 0;
         double clock_plus_gated_int = 0;
         double fcup_min_int = 0;
         double fcup_min_gated_int = 0;
         double clock_min_int = 0;
         double clock_min_gated_int = 0;
         double assym_fcup =  0;
         double assym_fcup_gated =  0;
         double assym_clock =  0;
         double assym_clock_gated =  0;
         int n_channel_0 = 0;
         int n_channel_2 = 0;
         double dt_st_scaler;
         double sync_delay = 0;
         double dt_sync = 0;
         bool flip_bit = 0;

         unsigned int n_rising_edge = 0;
         unsigned int n_falling_edge = 0;
         unsigned int n_pattern_sync = 0;
         unsigned int n_pair_sync = 0;
         unsigned int time_since_start_of_stable = 0;  // Unit = 8 ns
         unsigned int time_since_end_of_stable = 0;    // Unit = 8 ns
         unsigned int delta_t_last_settle = 0;
         unsigned int delta_t_last_stable = 0;
         int pattern_phase_count = 0;
         int decoder_helicity = 0;
         int decoder_helicity_at_start = 0;

         void assign_branches(TTree *tree){
            tree->Branch("event_num", &event_num);
            tree->Branch("event_time", &event_time);
            tree->Branch("last_sync_time", &last_sync_time_store);
            tree->Branch("last_struck_scaler_time", &last_struck_scaler_time_store);
            tree->Branch("i_struckscaler", &i_struckscaler_store);
            tree->Branch("struck_scaler_first_timestamp", &struck_scaler_first_timestamp);
            tree->Branch("relative_timestamp", &relative_time);
            tree->Branch("fcup", &fcup);
            tree->Branch("fcup_gated", fcup_gated);
            tree->Branch("clock", clock);
            tree->Branch("clock_gated", clock_gated);
            tree->Branch("hel_evt", &hel_evt);
            tree->Branch("helicity", &helicity);
            tree->Branch("helicity_valid", &helicity_valid);
            tree->Branch("helicity_raw", &helicity_raw);
            tree->Branch("raw_hel", &raw_hel);
            tree->Branch("raw_sync", &raw_sync);
            tree->Branch("raw_quad", &raw_quad);
            tree->Branch("raw_n_sync_flips", &raw_n_sync_flips);
            tree->Branch("raw_is_sync", &raw_is_sync);
            tree->Branch("raw_quad_phase", &raw_quad_phase);
            tree->Branch("is_struck_scaler", &is_struck_scaler);
            tree->Branch("fcup_plus_int", &fcup_plus_int);
            tree->Branch("fcup_plus_gated_int", fcup_plus_gated_int);
            tree->Branch("clock_plus_int", clock_plus_int);
            tree->Branch("clock_plus_gated_int", clock_plus_gated_int);
            tree->Branch("fcup_min_int", &fcup_min_int);
            tree->Branch("fcup_min_gated_int", fcup_min_gated_int);
            tree->Branch("clock_min_int", clock_min_int);
            tree->Branch("clock_min_gated_int", clock_min_gated_int);
            tree->Branch("assym_fcup", &assym_fcup);
            tree->Branch("assym_fcup_gated", &assym_fcup_gated);
            tree->Branch("assym_clock", &assym_clock);
            tree->Branch("assym_clock_gated", &assym_clock_gated);
            tree->Branch("n_channel_0", &n_channel_0);
            tree->Branch("n_channel_2", &n_channel_2);
            tree->Branch("dt_st_scaler", &dt_st_scaler);
            tree->Branch("sync_delay", &sync_delay);
            tree->Branch("dt_sync", dt_sync);
            tree->Branch("flip_bit", &flip_bit);
            tree->Branch("decoder_n_rising_edge", &n_rising_edge);
            tree->Branch("decoder_n_falling_edge", &n_falling_edge);
            tree->Branch("decoder_n_pattern_sync", &n_pattern_sync);
            tree->Branch("decoder_n_pair_sync", &n_pair_sync);
            tree->Branch("decoder_time_since_start_of_stable", &time_since_start_of_stable);
            tree->Branch("decoder_time_since_end_of_stable", &time_since_end_of_stable);
            tree->Branch("decoder_delta_t_last_settle", &delta_t_last_settle);
            tree->Branch("decoder_delta_t_last_stable", &delta_t_last_stable);
            tree->Branch("decoder_pattern_phase_count", &pattern_phase_count);
            tree->Branch("decoder_helicity", &decoder_helicity);
            tree->Branch("decoder_helicity_at_start", &decoder_helicity_at_start);
         }
      };

      int i_tree_store = 0;
      std::vector<Tree_Struct_t> tree_store(12);
      Tree_Struct_t fill_tree;
      fill_tree.assign_branches(tree);

      int n_combine = 120;   // 30 Hz, so this should be about 4s.

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

      evio->fStruckScaler->fDebug = (debug>0? 1: 0);

      unsigned long icount=0;
      unsigned long iflipcount = 0;

      int sync_signal = 0;
      int last_sync = -1;
      int last_quad = -1;
      int quad_phase = -10;
      unsigned long run_start_time = 0;
      int hel_signal = 0;
      int last_hel = -1;
      long last_hel_flip_time = 0;
      int quad_signal = 0;
      long last_quad_flip_time = 0;

      unsigned int i_struckscaler = 0;
      unsigned long struck_scaler_first_timestamp = 0;
      double dt_st_scaler=0;
      unsigned int n_rising_edge_last = 0;
      unsigned int n_falling_edge_last = 0;
      Helicity_Decoder_t last_helicity_decoder;
      unsigned int last_helicity_state = 0;
      unsigned int last_helicity_state_at_start = 0;
      unsigned int last_helicity_seed = 0;
      unsigned long last_sync_time = 0;
      unsigned long last_struck_scaler_time = 0;

      Tree_Struct_t Zero;

      while( evio->Next() == 0 ){
         if( evio->this_tag < 128) continue;  // Skip non-physics (i.e. scaler) events beyond this point.

         tree_store[i_tree_store] = Zero;  // Zero out all the data.

         tree_store[i_tree_store].event_num = evio->GetEventNumber();
         tree_store[i_tree_store].event_time = evio->GetTimeStamp();
         if(tree_store[i_tree_store].event_num == 90) run_start_time = evio->GetTimeStamp();
         tree_store[i_tree_store].hel_evt = evio->GetHelicity();
         tree_store[i_tree_store].flip_bit = evio->fStruckScaler->fTimeMarkerFlipped*1;

         hel_signal   = (evio->GetData(hel_i  ) < 2000 ? 0 : 1);
         sync_signal  = (evio->GetData(sync_i ) < 2000 ? 0 : 1);
         quad_signal = (evio->GetData(quart_i) < 2000 ? 0 : 1);
         tree_store[i_tree_store].raw_hel = hel_signal;
         tree_store[i_tree_store].raw_sync = sync_signal;
         tree_store[i_tree_store].raw_quad = quad_signal;

         if( hel_signal != last_hel && sync_signal == last_sync && iflipcount > 0 && evio->GetEventNumber() > 90) {
            if(quiet == 0)
               cout << std::setw(9) << evio->GetEventNumber() << "  !!!!!!HELICITY FLIP BUT THERE WAS NO SYNC!!!!!!\n";
            last_hel = hel_signal;
         }

         if( quad_signal != last_quad && sync_signal == last_sync && iflipcount > 0 ){
            if(quiet == 0)
               cout << std::setw(9) << evio->GetEventNumber() << "  !!!!!! QUAD FLIP BUT THERE WAS NO SYNC!!!!!!\n";
            last_quad = quad_signal;
         }

         if( sync_signal != last_sync && evio->GetTimeStamp() > 0) {
            ++iflipcount;
            tree_store[i_tree_store].raw_is_sync = 1;
            if(quad_signal == 0) {
               quad_phase = 0;
            }else {
               ++quad_phase;
            }
            if (last_sync != -1 && last_struck_scaler_time != 0) {
               if (abs(4.e-6 * ( long(evio->GetTimeStamp()) - last_struck_scaler_time)) <
                   abs(4.e-6 * ( last_sync_time - last_struck_scaler_time))) {
                  tree_store[i_tree_store].sync_delay = 4.e-6 * (last_struck_scaler_time - long(evio->GetTimeStamp()));
               } else {
                  tree_store[i_tree_store].sync_delay = 4.e-6 * (last_struck_scaler_time - last_sync_time);
               }
               h_sync_delay->Fill(tree_store[i_tree_store].sync_delay);
               tree_store[i_tree_store].dt_sync = 4.e-6 * (evio->GetTimeStamp() - last_sync_time);
               h_dt_sync->Fill(tree_store[i_tree_store].dt_sync);
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
               printf(" dt: %10.4f  ΔT: %8.4f", (tree_store[i_tree_store].event_time - run_start_time) * 4e-6, tree_store[i_tree_store].dt_sync);
               cout << "  Q: " << quad_signal << " S: " << sync_signal << " H: " << hel_signal << endl;
            }
            last_sync = sync_signal;
            last_hel = hel_signal;
            last_quad = quad_signal;
         }else{
            tree_store[i_tree_store].raw_is_sync = 0;
            tree_store[i_tree_store].dt_sync = 4.e-6 * (evio->GetTimeStamp() - last_sync_time);
         }

         tree_store[i_tree_store].raw_n_sync_flips = iflipcount;
         tree_store[i_tree_store].raw_quad_phase = quad_phase;

         if(evio->fStruckScaler->DataSize() > 0){
            tree_store[i_tree_store].n_channel_0 = 0;
            tree_store[i_tree_store].n_channel_2 = 0;
            for(int ii=0; ii< evio->fStruckScaler->fUngatedScalers->size(); ++ii){
               if(evio->fStruckScaler->fUngatedScalers->At(ii).channel_id == 0) ++tree_store[i_tree_store].n_channel_0;
               if(evio->fStruckScaler->fUngatedScalers->At(ii).channel_id == 2) ++tree_store[i_tree_store].n_channel_2;
            }
            h_stscaler_count->Fill(tree_store[i_tree_store].n_channel_2);
         }

         if( evio->fStruckScaler->fIntegrate->size() > 0){   // Struck scaler data is available in this event.
            tree_store[i_tree_store].is_struck_scaler = 1;
            dt_st_scaler = 4.e-6*( double(evio->GetTimeStamp()) - double(last_struck_scaler_time));

            h_dt_stscaler->Fill(dt_st_scaler);

            if(i_struckscaler == 0){
               struck_scaler_first_timestamp = evio->fStruckScaler->GetTimeStamp();
            }
            ScalerInfo_t &ptr = evio->fStruckScaler->fInfo[I_FCUP_UNGATED];
            tree_store[i_tree_store].fcup = ptr.value;
            tree_store[i_tree_store].helicity = ptr.helicity;
            tree_store[i_tree_store].helicity_valid = ptr.helicity_valid;
            tree_store[i_tree_store].helicity_raw = ptr.helicity_raw;
            tree_store[i_tree_store].fcup_plus_int = ptr.plus_integrated;
            tree_store[i_tree_store].fcup_min_int = ptr.minus_integrated;
            tree_store[i_tree_store].relative_time = (evio->fStruckScaler->GetTimeStamp() - struck_scaler_first_timestamp)*4.e-6;


            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               tree_store[i_tree_store].assym_fcup = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               tree_store[i_tree_store].assym_fcup = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_FCUP_GATED];
            tree_store[i_tree_store].fcup_gated = ptr.value;
//            printf("Evt: %9lu  R-time: %9.6f  fcup ga: %9.2f  H: %1d \n", event_num, relative_time, fcup_gated, helicity);
            tree_store[i_tree_store].fcup_plus_gated_int  = ptr.plus_integrated;
            tree_store[i_tree_store].fcup_min_gated_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               tree_store[i_tree_store].assym_fcup_gated = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               tree_store[i_tree_store].assym_fcup_gated = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_CLOCK_UNGATED];
            tree_store[i_tree_store].clock = ptr.value;
            tree_store[i_tree_store].clock_plus_int = ptr.plus_integrated;
            tree_store[i_tree_store].clock_min_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               tree_store[i_tree_store].assym_clock = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               tree_store[i_tree_store].assym_clock = 0;
            }

            ptr = evio->fStruckScaler->fInfo[I_CLOCK_GATED];
            tree_store[i_tree_store].clock_gated = ptr.value;
            tree_store[i_tree_store].clock_plus_gated_int = ptr.plus_integrated;
            tree_store[i_tree_store].clock_min_gated_int = ptr.minus_integrated;
            if( (ptr.plus_integrated + ptr.minus_integrated) > 0) {
               tree_store[i_tree_store].assym_clock_gated = 100.*(double(ptr.plus_integrated) - double(ptr.minus_integrated)) /
                       double(ptr.plus_integrated + ptr.minus_integrated);
            }else{
               tree_store[i_tree_store].assym_clock_gated = 0;
            }


            if( i_struckscaler%n_combine == 0 && i_struckscaler>0 ){
               times.push_back(tree_store[i_tree_store].relative_time);
               beam_charge_asymmetry.push_back(tree_store[i_tree_store].assym_fcup);
               beam_charge_asymmetry_gated.push_back(tree_store[i_tree_store].assym_fcup_gated);
               clock_asymmetry.push_back(tree_store[i_tree_store].assym_clock);
               clock_asymmetry_gated.push_back(tree_store[i_tree_store].assym_clock_gated);
            }

            last_struck_scaler_time = evio->GetTimeStamp();
            ++i_struckscaler;
            if(i_struckscaler > 1) tree_store[i_tree_store].dt_st_scaler = dt_st_scaler;
            else tree_store[i_tree_store].dt_st_scaler = 0;
         }else{
            if(i_struckscaler > 1) tree_store[i_tree_store].dt_st_scaler = 4.e-6*( double(evio->GetTimeStamp()) - double(last_struck_scaler_time));
            else tree_store[i_tree_store].dt_st_scaler = 0.;
         }
         tree_store[i_tree_store].i_struckscaler_store = i_struckscaler;

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
                     printf(" dt: %10.4f ", (tree_store[i_tree_store].event_time - run_start_time) * 4e-6);
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
                     printf(" dt: %10.4f ", (tree_store[i_tree_store].event_time - run_start_time) * 4e-6);
                     cout << " state: "
                          << std::bitset<32>(evio->fHelicity->fDecoder->At(idecode).helicity_state_at_start_of_pattern);
                     cout << " Count: " << evio->fHelicity->fDecoder->At(idecode).status.pattern_phase_count;
                     cout << " Hs:" << evio->fHelicity->fDecoder->At(idecode).status.helicity_at_pattern_start;
                     cout << " H: " << evio->fHelicity->fDecoder->At(idecode).status.helicity << endl;
                  }
                  //  cout << *evio->fHelicity->fDecoder.back();
                  last_helicity_state_at_start = evio->fHelicity->fDecoder->At(idecode).helicity_state_at_start_of_pattern;
               }

               // Store the decoder information with the event.
               tree_store[idecode].n_rising_edge = evio->fHelicity->fDecoder->At(idecode).n_rising_edge;
               tree_store[idecode].n_falling_edge = evio->fHelicity->fDecoder->At(idecode).n_falling_edge;
               tree_store[idecode].n_pattern_sync = evio->fHelicity->fDecoder->At(idecode).n_pattern_sync;
               tree_store[idecode].n_pair_sync = evio->fHelicity->fDecoder->At(idecode).n_pair_sync;
               tree_store[idecode].time_since_start_of_stable = evio->fHelicity->fDecoder->At(idecode).time_since_start_of_stable;
               tree_store[idecode].time_since_end_of_stable = evio->fHelicity->fDecoder->At(idecode).time_since_end_of_stable;
               tree_store[idecode].delta_t_last_settle = evio->fHelicity->fDecoder->At(idecode).delta_t_last_settle;
               tree_store[idecode].delta_t_last_stable = evio->fHelicity->fDecoder->At(idecode).delta_t_last_stable;
               tree_store[idecode].pattern_phase_count = evio->fHelicity->fDecoder->At(idecode).status.pattern_phase_count;
               tree_store[idecode].decoder_helicity_at_start = evio->fHelicity->fDecoder->At(idecode).status.helicity_at_pattern_start;

            }

            if( (i_tree_store+1 != evio->fHelicity->size()) ){
               std::cout << "  Event: " << std::setw(12) <<  evio->GetEventNumber() << "  Number of decoder events != number of events passed. ";
               std::cout << " Events passed: " << i_tree_store+1 << " != decode size: " << evio->fHelicity->size() << "\n";
            }

            // Now fill the tree with the last 10 or 11 events.
            for (int i = 0; i <= i_tree_store; ++i) {
               fill_tree = tree_store[i];
               tree->Fill();
            }

            i_tree_store = 0;
         }else{
            if( (++i_tree_store) > 11){
               std::cout << "Event: " << evio->GetEventNumber() << " More than 10 events without decoder in it: i_tree_store = " << i_tree_store << endl;
               i_tree_store = 0;
            }
         }

         if( evio->fHelicity->size() > 0){
            // std::cout << "Helicity decoder bank  : " << evio->GetEventNumber() << " time: " << evio->GetTimeStamp() << "\n";

         }


         if(evio->GetEventNumber()>0 && evio->GetTimeStamp() > 0) {
            ++icount;
            if(quiet == 0){
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