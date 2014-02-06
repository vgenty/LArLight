#ifndef DAVIDANA_CC
#define DAVIDANA_CC

#include "DavidAna.hh"

namespace larlight {

  bool DavidAna::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you need, you have an output root file pointer "_fout".

    tpc_evno =       new TH1D("tpc_evno",     "Event number",    100, 0, 100);

    return true;
  }
  
  bool DavidAna::analyze(storage_manager* storage) {
  
    //
    // Do your event-by-event analysis here. This function is called for 
    // each event in the loop. You have "storage" pointer which contains 
    // event-wise data. For a reference of pmt_wf_collection class instance, 
    // see the class index in the documentation.

    const event_fifo *event_wf = (event_fifo*)(storage->get_data(DATA::TPCFIFO));
    tpc_evno->Fill(event_wf->event_number());
  
    return true;
  }

  bool DavidAna::finalize() {
  
    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.

    tpc_evno->Write();
  
    return true;
  }
}
#endif
