import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from ROOT import gSystem
gSystem.Load("libCMergePerformance")
from ROOT import larlight, cluster

# Create ana_processor instance
my_proc = larlight.ana_processor()

# Set input root file
my_proc.add_input_file(sys.argv[1])

# Specify IO mode
my_proc.set_io_mode(larlight.storage_manager.READ)

# Specify input TDirectory name
#my_proc.set_input_rootdir("scanner")

# Specify output root file name
my_proc.set_ana_output_file("calorimetry.root");

#attach the CMergePerformance process
calo = larlight.Calorimetry()
my_proc.add_process(calo)

calo.SetClusterType(larlight.DATA.Cluster)
calo.SetDebug(False)

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run();

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)

