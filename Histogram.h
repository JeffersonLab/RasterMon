//
// Created by Maurik Holtrop on 5/17/22.
//

#ifndef RASTERMON_HISTOGRAM_H
#define RASTERMON_HISTOGRAM_H
#include "TH1D.h"
#include "TH2D.h"
#include <string>

struct Histogram_t {  // Object to hold the information for each histogram channel.
   unsigned char bank_tag;
   unsigned char slot;
   unsigned char adc_chan;  // Channel for the slot of the FADC module.
   int data_index = -1;
   unsigned char bank_tag2 = 0; // For 2D histos, second channel info.
   unsigned char slot2 = 0;
   unsigned char adc_chan2 = 0;  // Channel for the slot of the FADC module.
   int data_index2 = -1;
   unsigned char tab_number; // Number of the tab where graphs are to be shown.
   unsigned char pad_number; // Number of the pad in the canvas (tab). 0 = canvas, 1 is first pad etc.
   bool show;      // Set false to not draw the THist, but still accumulate the data.
   double scale_x = 1.;  // Conversion from ADC integer to real
   double offset_x = 0.; // Offset for conversion to real.
   double scale_y = 1.;
   double offset_y = 0.;
   int special_fill = 0;  // Special calculation for filling this histogram, i.e. exceptions. -1 = no fill.
   int special_draw = 0;  // Special way of drawing this histogram.
   std::string draw_opt;  // Drawing option.
   std::string legend;      // Legend entry. -- Usually blank, so no legend.
   TH1 *hist = nullptr;  // Histogram. -- Note: Must be either a unique_ptr, OR we need to be really careful with copy and move constructors.
   Histogram_t(unsigned char tab_number, unsigned char pad_number,
               unsigned int bank_tag, unsigned char slot, unsigned char adc_chan,
               const std::string &name, const std::string &title, int nx, double x_min, double x_max) :
         bank_tag(bank_tag), slot(slot), adc_chan(adc_chan), tab_number(tab_number), pad_number(pad_number){
      hist = new TH1D(name.c_str(), title.c_str(), nx, x_min, x_max);
      show = true;
   };

   Histogram_t(unsigned char tab_number, unsigned char pad_number,
               unsigned int bank_tag_x, unsigned char slot_x, unsigned char adc_chan_x,
               unsigned int bank_tag_y, unsigned char slot_y, unsigned char adc_chan_y,
               const std::string &name, const std::string &title, int nx, double x_min, double x_max, int ny, double y_min, double y_max) :
         bank_tag(bank_tag_x), slot(slot_x), adc_chan(adc_chan_x),
         bank_tag2(bank_tag_y), slot2(slot_y), adc_chan2(adc_chan_y),
         tab_number(tab_number), pad_number(pad_number) {
      hist = new TH2D(name.c_str(), title.c_str(), nx, x_min, x_max, ny, y_min, y_max);
      show = true;
   }
   //Add the 5 from the Rule of 5.
   //
   // Note on *DESTRUCTOR*  == The Rule of 5.
   // IF you choose to have a non-defaulted destructor (i.e. include a ~Histogram_t()), then you *MUST* also include
   // a copy constructor, a move constructor, a copy assignment and a move assignment, because none of these will be default.
   ~Histogram_t(){
      delete hist;
   };
   // Copy constructor - Explicit, because we do not want to copy the pointer to the histogram, but the histogram itself.
   // Currently, only two types of histograms that may need copying, TH1D and TH2D.
   Histogram_t(const Histogram_t &that): bank_tag(that.bank_tag), slot(that.slot), adc_chan(that.adc_chan), data_index(that.data_index),
                                         bank_tag2(that.bank_tag2), slot2(that.slot2), adc_chan2(that.adc_chan2), data_index2(that.data_index2), tab_number(that.tab_number),
                                         pad_number(that.pad_number), show(that.show), scale_x(that.scale_x), offset_x(that.offset_x), scale_y(that.scale_y), offset_y(that.offset_x),
                                         special_fill(that.special_fill), special_draw(that.special_draw), draw_opt(that.draw_opt), legend(that.legend)
   {
      if(strncmp(that.hist->ClassName(),"TH1D",4) == 0) hist = new TH1D( *(TH1D *)that.hist);
      if(strncmp(that.hist->ClassName(),"TH2D",4) == 0) hist = new TH2D( *(TH2D *)that.hist);
   }
   Histogram_t(Histogram_t&& that)  noexcept {    // move constructor -- used for emplace_back. Needs to be explicit because of the pointer.
      bank_tag = that.bank_tag;
      slot = that.slot;
      adc_chan = that.adc_chan;
      data_index = that.data_index;
      bank_tag2 = that.bank_tag2;
      slot2 = that.slot2;
      adc_chan2 = that.adc_chan2;
      data_index2 = that.data_index2;
      tab_number = that.tab_number;
      pad_number = that.pad_number;
      show = that.show;
      scale_x = that.scale_x;
      offset_x = that.offset_x;
      scale_y = that.scale_y;
      offset_y = that.offset_y;
      special_fill = that.special_fill;
      special_draw = that.special_draw;
      draw_opt = std::move(that.draw_opt);
      legend= std::move(that.legend);
      hist = std::exchange(that.hist, nullptr);
   }
   Histogram_t& operator=(Histogram_t&& that){
      bank_tag = that.bank_tag;
      slot = that.slot;
      adc_chan = that.adc_chan;
      data_index = that.data_index;
      bank_tag2 = that.bank_tag2;
      slot2 = that.slot2;
      adc_chan2 = that.adc_chan2;
      data_index2 = that.data_index2;
      tab_number = that.tab_number;
      pad_number = that.pad_number;
      show = that.show;
      scale_x = that.scale_x;
      offset_x = that.offset_x;
      scale_y = that.scale_y;
      offset_y = that.offset_y;
      special_fill = that.special_fill;
      special_draw = that.special_draw;
      draw_opt = std::move(that.draw_opt);
      legend= std::move(that.legend);
      hist = std::exchange(that.hist, nullptr);
      return *this;
   }
   Histogram_t& operator=(const Histogram_t& that){  // copy assignment needs to make a proper copy of the histogram.
      if (&that == this) return *this; // If self assignment.
      delete hist;            // Destroy existing histogram.
      // Now copy everything.
      bank_tag = that.bank_tag;
      slot = that.slot;
      adc_chan = that.adc_chan;
      data_index = that.data_index;
      bank_tag2 = that.bank_tag2;
      slot2 = that.slot2;
      adc_chan2 = that.adc_chan2;
      data_index2 = that.data_index2;
      tab_number = that.tab_number;
      pad_number = that.pad_number;
      show = that.show;
      scale_x = that.scale_x;
      offset_x = that.offset_x;
      scale_y = that.scale_y;
      offset_y = that.offset_y;
      special_fill = that.special_fill;
      special_draw = that.special_draw;
      draw_opt = that.draw_opt;
      legend= that.legend;
      if(strncmp(that.hist->ClassName(),"TH1D",4) == 0) hist = new TH1D( *(TH1D *) that.hist);
      if(strncmp(that.hist->ClassName(),"TH2D",4) == 0) hist = new TH2D( *(TH2D *) that.hist);
      return *this;
   };
   //   ~Histogram_t();                              // destructor


   [[nodiscard]] bool Is2D() const{ return bank_tag2>0; }

   [[nodiscard]] TH1 *GetTH1() const{
      // Return a TH1D histogram pointer. No checks are made that the histogram is there.
      return hist;
   }

   [[nodiscard]] TH1D *GetHist() const{
      // Return a TH1D histogram pointer. No checks are made that the histogram is there.
      return dynamic_cast<TH1D *>(hist);
   }
   [[nodiscard]] TH2D *GetHist2D() const{
      // Return a TH2D histogram pointer. Note: No check is made that this is indeed a 2D histogram!
      return dynamic_cast<TH2D *>(hist);
   }

};


#endif //RASTERMON_HISTOGRAM_H