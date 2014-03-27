import sys
from ROOT import *
gSystem.Load("libBase")
gSystem.Load("libLArUtil")
gSystem.Load("libClusterRecoUtil")
from ROOT import larlight as fmwk
from ROOT import cluster


import argparse

parser = argparse.ArgumentParser(description="Python script to process and merge showers.")
group = parser.add_mutually_exclusive_group()
group.add_argument("-v", "--verbose", help="Turn on verbose output",
                    action="store_true")
group.add_argument("-q", "--quiet", help="Turn off most output",
                    action="store_true")
parser.add_argument("-s","--source",help="Name of input file")
parser.add_argument("-o","--data-output",help="Output data file, if event is changed")
parser.add_argument("-a","--ana-output",help="Analysis output file")
parser.add_argument("-n","--num-events",help="Number of events to process")
parser.add_argument("-d","--display",help="Turn on the display to see each view before and after." )
args = parser.parse_args()

if len(sys.argv) == 1:
    print "\n-------You forgot to include a source file!-------\n"
    parser.print_help()

if args.verbose:
    print "Verbose mode turned on."
    if args.source != None:
        print "\tSource file is " + args.source
    if args.data_output != None:
        print "\tData output file is " + args.data_output
    if args.ana_output != None:
        print "\tAna output file is " + args.ana_output

if args.source == None:
    print "Error: please specificy an input file with -s or --source."
    quit()

if args.num_events == None:
    args.num_events = -1

if args.data_output == None:
    args.data_output = "default_event_output.root"
    print "No event output file selected.  If necessary, output will go to:"
    print "\t"+args.data_output

if args.ana_output == None:
    args.ana_output = "default_ana_output.root"
    print "No ana output file selected.  If necessary, output will go to:"
    print "\t"+args.ana_output


# ana_proc = larlight.ana_processor()

# if args.verbose:
    # ana_proc.set_verbosity(larlight.MSG.DEBUG)

# Not sure what this does
# ana_proc.set_io_mode(larlight.storage_manager.BOTH)

# Add the input file.  Not sure if the above takes multiple input files yet
# ana_proc.add_input_file(args.source)

mgr = fmwk.storage_manager()

mgr.set_io_mode(mgr.READ)

mgr.add_in_filename(args.source)
if len(sys.argv) > 2:
    mgr.set_in_rootdir("scanner")

mgr.open()

chit=TCanvas("chit","chit",600,500)
chit.SetGridx(1)
chit.SetGridy(1)
algo = cluster.ClusterParamsExecutor()

processed_events=0

while mgr.next_event():

    # Get event_cluster ... std::vector<larlight::cluster>
    cluster_v = mgr.get_data(fmwk.DATA.ShowerAngleCluster)

    # Get event_mctruth ... std::vector<larlight::mctruth>
    mctruth_v = mgr.get_data(fmwk.DATA.MCTruth)

    # Get the primary particl generator vtx position
    mct_vtx=None
    if mctruth_v and mctruth_v.size():
        if mctruth_v.size()>1:
            print "Found more than 1 MCTruth. Only use the 1st one..."
        mct_vtx = mctruth_v.at(0).GetParticles().at(0).Trajectory().at(0).Position()

    if args.num_events == processed_events:
        exit()
        
    if not cluster_v:
        continue

    print "Event:",cluster_v.event_id()
    for x in xrange(cluster_v.size()):
        
        print "  Cluster ID:",cluster_v.at(x).ID()

        algo.LoadCluster(cluster_v.at(x),
                         mgr.get_data(cluster_v.get_hit_type()))
        # algo.FillParams(True,True,True,True,True)
        algo.GetAverages(True)
        algo.GetRoughAxis(True)
        algo.GetProfileInfo(True)
#       algo.RefineStartPoints(True)
        # algo.GetFinalSlope(True)
        algo.Report()
        print "(%g,%g) => (%g,%g)" % (algo.StartPoint().w,
                                      algo.StartPoint().t,
                                      algo.EndPoint().w,
                                      algo.EndPoint().t)
        if(mct_vtx):
            print "MC Particle Start Point: (%g,%g,%g)" % (mct_vtx[0],mct_vtx[1],mct_vtx[2])
	
	#Add black star to mark begin point and black square to mark end point
#	begin = TGraph(1)
#	end = TGraph(1)
#	begin.SetPoint(0,algo.StartPoint().w, algo.StartPoint().t)
#	end.SetPoint(0,algo.EndPoint().w, algo.EndPoint().t)

        chit.cd()
        algo.GetHitView().Draw("COLZ")
#	begin.SetLineColor(1)
#	begin.SetMarkerStyle(29)
#	begin.SetMarkerColor(1)
#	begin.Draw("P same")
#	end.SetLineColor(1)
#	end.SetMarkerStyle(21)
#	end.SetMarkerColor(1)
#	end.Draw("P same")
        chit.Update()
        sys.stdin.readline()



