#ifndef VIEWCOMPRESSION_CC
#define VIEWCOMPRESSION_CC

#include "ViewCompression.hh"

namespace larlight {

  bool ViewCompression::initialize() {

    return true;
  }
  
  bool ViewCompression::analyze(storage_manager* storage) {

    //reset WF counter
    _currentWF = 0;

    // If no compression algorithm has been defined, skip
    if ( _compress_algo == 0 ){
      print(MSG::ERROR,__FUNCTION__,"Compression Algorithm Not Set! Exiting");
      return false;
    }

    // Otherwise Get RawDigits and execute compression
    larlight::event_tpcfifo *event_wf = (event_tpcfifo*)(storage->get_data(DATA::TPCFIFO));

    if(!event_wf) {
      print(MSG::ERROR,__FUNCTION__,"Data storage did not find associated waveforms!");
      return false;
    }

    _numWFs = event_wf->size();

    _current_event_wf = event_wf;
    
    return true;
  }
  
  bool ViewCompression::finalize() {
    
    return true;
  }
  
  
  void ViewCompression::processWF(){
    
    //get tpc_data
    larlight::tpcfifo* tpc_data = (&(_current_event_wf->at(_currentWF)));      
    
    //finally, apply compression..
    std::vector<unsigned short> ADCwaveform = getADCs(tpc_data);
    _compress_algo->ApplyCompression(ADCwaveform);
    std::vector<std::vector<unsigned short> > compressOutput = _compress_algo->GetOutputWFs();
    std::vector<int> outTimes = _compress_algo->GetOutputWFTimes();
    
    _NumOutWFs = compressOutput.size();
    
    //clear histograms
    ClearHistograms();
    //now fill histograms
    FillHistograms(ADCwaveform, compressOutput, outTimes);

    _currentWF += 1;
    
    return;
  }
  

  std::vector<unsigned short> ViewCompression::getADCs(larlight::tpcfifo* tpc_data){

    std::vector<unsigned short> ADCs;
    ADCs.clear();
    for (size_t i=0; i < tpc_data->size(); i++)
      ADCs.push_back((unsigned short)(tpc_data->at(i)));

    return ADCs;

  }


  void ViewCompression::FillHistograms(std::vector<unsigned short> ADCwaveform, std::vector<std::vector<unsigned short> >compressOutput,
				       std::vector<int> outputTimes){
    
    _hInWF = new TH1I("hInWF", "Input Waveform; ADCs", ADCwaveform.size(), 0, ADCwaveform.size());
    _hOutWF = new TH1I("hOutWF", "Output Waveform; ADCs", ADCwaveform.size(), 0, ADCwaveform.size());
    
    for (size_t n=0; n < ADCwaveform.size(); n++)
      _hInWF->SetBinContent(n+1, ADCwaveform.at(n));
    
    for (size_t j=0; j < compressOutput.size(); j++){
      for (size_t k=0; k < compressOutput.at(j).size(); k++){
	_hOutWF->SetBinContent( outputTimes.at(j)+k+1, compressOutput.at(j).at(k));
      }
    }

    _hInWF->SetAxisRange(_hInWF->GetMinimum(), _hInWF->GetMaximum(), "Y");
    _hOutWF->SetAxisRange(_hInWF->GetMinimum(), _hInWF->GetMaximum(), "Y");
    
    return;
  }
  
}
#endif
