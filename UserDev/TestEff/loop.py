import sys
from ROOT import *
gSystem.Load("libTestEff");
from ROOT import larlight

proc = larlight.ana_processor()

# //Specify Io mode

proc.set_io_mode(larlight.storage_manager.READ);
proc.set_input_rootdir("scanner");
proc.set_ana_output_file("ana.root");

proc.add_input_file("~/data/larlight_tracks_showers.root");  
# //proc.add_input_file("testing.root") ; //~/data/combined_00.root");    //mac/larlight_tracks.root");

# //proc.add_input_file("testing.root") ; //~/data/combined_00.root");    //mac/larlight_tracks.root");


# //proc.add_input_file("../../combined_02.root");    //mac/larlight_tracks.root");
# //proc.set_verbosity(larlight.MSG.DEBUG);
proc.add_process(larlight.TestEff())
proc.run(500, 200)

sys.stdin.readline()


