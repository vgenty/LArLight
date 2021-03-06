import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from ROOT import gSystem
gSystem.Load("libShowerStudy")
from ROOT import larlight as fmwk

# Create ana_processor instance
my_proc = fmwk.ana_processor()

for x in xrange(len(sys.argv)-1):
	my_proc.add_input_file(sys.argv[x+1])


# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.READ)

my_proc.set_rootdir("scanner")

# Specify input TDirectory name if given
#if len(sys.argv) > 2:

 #   my_proc.set_input_rootdir(sys.argv[2])

# Specify output root file name
my_proc.set_ana_output_file("distance.root");

# Attach a template process
ana_unit_1 = fmwk.PDGSelection()
ana_unit_0 = fmwk.ShowerDistanceStudy()
ana_unit_1.Select(11,ana_unit_1.kGENERATOR,1)
my_proc.enable_filter(True)
my_proc.add_process(ana_unit_1)
my_proc.add_process(ana_unit_0)

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

