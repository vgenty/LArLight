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
my_proc.set_input_rootdir("scanner")

# Specify output root file name
my_proc.set_ana_output_file("CMP_ana_out.root");

#attach the CMergePerformance process
my_CMP = larlight.CMergePerformance()
my_proc.add_process(my_CMP)

#For now, CMergePerformance only works with one merging iteration, will have to change this later
my_CMP.GetManager().MergeTillConverge(False)
my_CMP.GetManager().DebugMode(cluster.CMergeManager.kPerAlgoSet)

#Configure the merge algos you want to use here
short_dist_algo = cluster.CMAlgoShortestDist()
short_dist_algo.SetSquaredDistanceCut(200.)
short_dist_algo.SetVerbose(False)
my_CMP.GetManager().AddMergeAlgo(short_dist_algo)

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run(1,5);

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
