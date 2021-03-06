#ifndef SAVEWF_CC
#define SAVEWF_CC

#include "SaveWF.hh"
#include "math.h"

namespace larlight {

  bool SaveWF::initialize() {


    noise = new TH1D("noise", "Noise RMS; ADC-baseline; freq." , 10, -5, 5);

    _baseline      =     0;
    event_num      =     0;
    _PedInduction  =  2048;
    _PedCollection =   400;

    return true;
  }
  
  bool SaveWF::analyze(storage_manager* storage) {
  
    //Count events (since variable seems set to 0)
    event_num += 1;
    if ( event_num%100 == 0 ){

    const event_tpcfifo *event_wf = (event_tpcfifo*)(storage->get_data(DATA::TPCFIFO));
    //waveform counter
    int wfnum = 0;
    int multiplicity = 0;


    //Loop over all waveforms
    for (size_t i=0; i<event_wf->size(); i++){
      wfnum += 1;
      //Check for empty waveforms!
      const tpcfifo* tpc_data = (&(event_wf->at(i)));
      if(tpc_data->size()<1){
	Message::send(MSG::ERROR,__FUNCTION__,
		      Form("Found 0-length waveform: Event %d ... Ch. %d",event_wf->event_number(),tpc_data->channel_number()));
	continue;
      }

      bool interesting = false;
      UInt_t chan = tpc_data->channel_number();
      //determine  plane type
      if ( larutil::Geometry::GetME()->SignalType(chan) == larlight::GEO::kInduction)
	_baseline = _PedInduction;
      else if ( larutil::Geometry::GetME()->SignalType(chan) == larlight::GEO::kCollection )
	_baseline = _PedCollection;
      else 
	_baseline = 1000;

      if ( i > 0 ){
	if ( chan == (&event_wf->at(i-1))->channel_number() ) {multiplicity += 1; }
	else { multiplicity = 0; }
      }

      //Here allow for selection of 1 channel
      if ( chan == 137 ){
	
	//create temporary histogram
	
	char ADCName[25];
	sprintf(ADCName,"WF_ev_%d_wf_%d_%d",event_num,tpc_data->channel_number(),multiplicity);
	TH1D* ADChist = new TH1D(ADCName,"pulse",tpc_data->size(),0,tpc_data->size());
	
	//Get Waveform
	for ( unsigned int  adc_index=0; adc_index<tpc_data->size(); adc_index++){
	  int adcs = tpc_data->at(adc_index);
	  //determine if something interesting happens
	  if ( (adcs-_baseline > 6) or ( adcs-_baseline < -6) ){
	    interesting = true;
	  }
	  else {
	    noise->Fill(adcs-_baseline);
	  }
	  ADChist->SetBinContent(adc_index+1,adcs);
	}
	
	if (interesting)
	  ADChist->Write();

      }//End IF correct channel #
      
    }//End for each waveform
    
    }

    return true;
    
  }//End Analyze Loop
  
  
  bool SaveWF::finalize() {
    
    noise->Write();
    rms->Write();
    return true;
  }
}
#endif
