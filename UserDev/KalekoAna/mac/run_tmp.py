import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from ROOT import gSystem
gSystem.Load("libKalekoAna")
from ROOT import larlight as fmwk
from ROOT import kaleko
# Create ana_processor instance
my_proc = fmwk.ana_processor()
my_proc.set_verbosity(fmwk.MSG.NORMAL)

# Set input root file
my_proc.add_input_file(sys.argv[1])

# Have it find the scanner directory
#fmwk.storage_manager.get().set_in_rootdir("scanner")
fmwk.storage_manager.get().set_data_to_read(fmwk.DATA.MCTruth,False)
fmwk.storage_manager.get().set_data_to_read(fmwk.DATA.Calorimetry,False)
fmwk.storage_manager.get().set_data_to_read(fmwk.DATA.Shower,False)
#fmwk.storage_manager.get().set_data_to_read(fmwk.DATA.ShowerAngleCluster,False)


# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.READ)

# Specify input TDirectory name if given
if len(sys.argv) > 2:

    my_proc.set_input_rootdir(sys.argv[2])

# Specify output root file name
my_proc.set_ana_output_file("out.root");

# Attach a template process
my_ana= kaleko.tmpClusterAna()
my_proc.add_process(my_ana)

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

