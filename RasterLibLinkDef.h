#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all typedef;

#pragma link C++ class RasterMonGui+;
#pragma link C++ class RasterEvioTool+;
#pragma link C++ struct EvioBank_t;
#pragma link C++ struct EvioSlot_t;
#pragma link C++ class RasterHists+;
#pragma link C++ class RasterMonEventInfo+;
#pragma link C++ class RasterLogBookEntry+;
#pragma link C++ class RasterMonConfigInfo+;
#pragma link C++ class RasterScalerBank+;
#pragma link C++ struct Scalar_t;
#pragma link C++ struct Graph_t;
#pragma link C++ struct Histogram_t;
#pragma link C++ struct TabSpace_t;
#pragma link C++ function Default_Setup_Raster_Tab;
#pragma link C++ function Default_Setup_Raw_Raster_Tab;
#pragma link C++ function Default_Setup_Raw_Raster_Tab2;
#pragma link C++ function Default_Setup_Raw_Raster_Tab3;
#pragma link C++ function Default_Setup_1_Channel_Scope;
#pragma link C++ function Default_Setup_2_Channel_Scope;
#pragma link C++ function Default_Setup_3_Channel_Scope;
#pragma link C++ function Default_Setup_Helicity;
#pragma link C++ function Default_Initialize_Histograms;

#pragma link C++ function Bank::Add_Leaf<Scalar_t>;
#pragma link C++ function Bank::Push_data<Scalar_t>;
#pragma link C++ function Bank::Push_data_vector<Scalar_t>;
#pragma link C++ function Bank::Get_data<Scalar_t>(int,int);
#pragma link C++ function Bank::Get_data<Scalar_t>(string,int);
#pragma link C++ function Bank::Get_data_vector<Scalar_t>;
#pragma link C++ class Leaf<Scalar_t>+;
#pragma link C++ class vector<Leaf<Scalar_t> >;

