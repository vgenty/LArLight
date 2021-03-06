import sys
from array import array
from ROOT import *
import ROOT
gSystem.Load("libFANN")

from ROOT import larlight as fmwk
from ROOT import cluster


import argparse

parser = argparse.ArgumentParser(description="Python script to process and merge showers.")
group = parser.add_mutually_exclusive_group()
group.add_argument("-v", "--verbose", help="Turn on verbose output",
                    action="store_true")
group.add_argument("-q", "--quiet", help="Turn off most output",
                    action="store_true")
parser.add_argument("-s","--source",nargs='*',help="Name of input file")
parser.add_argument("-o","--data-output",help="Output data file, if event is changed")
parser.add_argument("-a","--ana-output",help="Analysis output file")
parser.add_argument("-n","--num-events",help="Number of events to process")
parser.add_argument("-d","--display",help="Turn on the display to see each view before and after." )
parser.add_argument("-A","--argoneut",action='store_true',help="Set geometry to argonuet.")
parser.add_argument("--nskip", help="Number of events to skip before processing.")
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
    args.num_events = 999999

if args.data_output == None:
    args.data_output = "default_event_output.root"
    print "No event output file selected.  If necessary, output will go to:"
    print "\t"+args.data_output

if args.ana_output == None:
    args.ana_output = "default_ana_output.root"
    print "No ana output file selected.  If necessary, output will go to:"
    print "\t"+args.ana_output



mgr = fmwk.storage_manager()
mgr.set_io_mode(mgr.READ)
for source in args.source:
    mgr.add_in_filename(source)

if len(sys.argv) > 2:
    mgr.set_in_rootdir("scanner")

mgr.open()

algo = cluster.ClusterParamsExecutor()
algo.SetUseHitBlurring(false)
algo.DisableFANN()

if args.argoneut != None:
    algo.SetArgoneutGeometry()

fann = cluster.TrainingModule()
fann.setFeatureVectorLength(13)
fann.setOutputVectorLength(2)
fann.setOutputFileName("track_shower.net")
fann.init()

# Here is the neural network that has been more rigorously trained:
cascadeFANN = cluster.FANNModule()
cascadeFANN.LoadFromFile("cascade_net.net")

# Output file for FANN vector:
outputfile = open('training.dat','w')
# Keep track of how many events were processed
nWrittenEvents = 0

processed_events=0
fGSer = larutil.GeometryUtilities.GetME()
num_events = int(args.num_events)

display=TCanvas("EventDisplay","EventDisplay",600,500)
display.SetGridx(1)
display.SetGridy(1)

while mgr.next_event():

    # Get event_mctruth ... std::vector<larlight::mctruth>
    mctruth_v = mgr.get_data(fmwk.DATA.MCTruth)
    # Get event_cluster ... std::vector<larlight::cluster>
    cluster_v = mgr.get_data(fmwk.DATA.FuzzyCluster)
    print "Number of clusters in this event is %d" % cluster_v.size()
    # Get the primary particl generator vtx position
    mct_vtx=None
    if mctruth_v and mctruth_v.size():
        if mctruth_v.size()>1:
            print "Found more than 1 MCTruth. Only use the 1st one... \n \n"
        if mctruth_v.at(0).GetParticles().at(0).PdgCode() == 11:      ## electron    
            mct_vtx = mctruth_v.at(0).GetParticles().at(0).Trajectory().at(0).Position()
            print "\n electron \n"
            truth=ROOT.std.vector(float)(2,0)
            truth[0] = 1
        elif mctruth_v.at(0).GetParticles().at(0).PdgCode() == 22:    
            trajsize= mctruth_v.at(0).GetParticles().at(0).Trajectory().size()
            mct_vtx = mctruth_v.at(0).GetParticles().at(0).Trajectory().at(trajsize-1).Position()
            print "\n gamma \n"
            continue
        elif mctruth_v.at(0).GetParticles().at(0).PdgCode() == 13:    
            trajsize= mctruth_v.at(0).GetParticles().at(0).Trajectory().size()
            mct_vtx = mctruth_v.at(0).GetParticles().at(0).Trajectory().at(0).Position()
            print "\n muon \n"
            truth=ROOT.std.vector(float)(2,0)
            truth[1] = 1
        elif mctruth_v.at(0).GetParticles().at(0).PdgCode() == 2212:    
            trajsize= mctruth_v.at(0).GetParticles().at(0).Trajectory().size()
            mct_vtx = mctruth_v.at(0).GetParticles().at(0).Trajectory().at(0).Position()
            print "\n proton \n"
            truth=ROOT.std.vector(float)(2,0)
            truth[1] = 1

        #PdgCode
    if num_events < processed_events:
        break

    if not cluster_v:
        continue

    print "Event:",cluster_v.event_id()

    for clust in xrange(cluster_v.size()):

        print "  Cluster ID:",cluster_v.at(clust).ID()
        algo.LoadCluster(cluster_v.at(clust),
                         mgr.get_data(cluster_v.get_hit_type()))
        if algo.GetNHits() < 30:
            continue


        # print "Number of hits in this cluster is %d" % (algo.GetNHits() )
        # if (algo.GetNHits() < 30 ):
            # continue
        algo.FillParams()
        algo.Report()
        algo.PrintFANNVector()
        result = algo.GetParams()

        featureVec=ROOT.std.vector(float)(10,0)
        algo.GetFANNVector(featureVec)
        
        # Print the training to the file:
        for val in featureVec:
            s = str(val)
            outputfile.write(s)
            outputfile.write(' ')
        outputfile.write('\n')
        for val in truth:
            s = str(val)
            outputfile.write(s)
            outputfile.write(' ')           

        # Write a space into the output file:
        outputfile.write('\n')
        outputfile.write('\n')

        nWrittenEvents += 1

        # Run the training:
        fann.trainOnData(featureVec, truth)
        print "Truth is (%g, %g)" % (truth[0],truth[1])
        fann.run(featureVec)
        fann.print_error()
        print "Cascade fann results:"
        cascadeFANN.run(featureVec)
        cascadeFANN.print_error()

        mc_begin=None
        if(mct_vtx):
            print "MC Particle Start Point: (%g,%g,%g)" % (mct_vtx[0],mct_vtx[1],mct_vtx[2])

            # Usage example 2: double[3] C-array like object in python
            # 'd' specifies "double precision", 2nd argument specifies the array (length & values)
            my_vec = array('d',[0,0,0]) 
            my_vec[0] = mct_vtx[0]
            my_vec[1] = mct_vtx[1]
            my_vec[2] = mct_vtx[2]
            mcpoint=fGSer.Get2DPointProjectionCM(my_vec,result.start_point.plane)

            # Example 1 & 2 should have the same return here (checked)
            # print " Start point in w,t  (%g,%g)" % (mcpoint.w,mcpoint.t)   

            mc_begin = TGraph(1)
            mc_begin.SetPoint(0,mcpoint.w,mcpoint.t)
            mc_begin.SetMarkerStyle(29)
            mc_begin.SetMarkerColor(ROOT.kRed)
            mc_begin.SetMarkerSize(3)

        #Add black star to mark begin point and black square to mark end point
        begin = TGraph(1)
        end = TGraph(1)
        begin.SetPoint(0,result.start_point.w, result.start_point.t)
        end.SetPoint(0,result.end_point.w, result.end_point.t)
        begin.SetLineWidth(0)
        end.SetLineWidth(0)
        # Set Start & End point TGraph
        begin.SetLineColor(1)
        begin.SetMarkerStyle(29)
        begin.SetMarkerColor(ROOT.kBlack)
        begin.SetMarkerSize(3)
        end.SetLineColor(1)
        end.SetMarkerStyle(21)
        end.SetMarkerColor(ROOT.kBlack)
        end.SetMarkerSize(2)

        func=TF1("nf","[1]*x+[0]",result.start_point.w-50,result.end_point.w+50);
        func.SetParameter(0,algo.RoughIntercept());     
        func.SetParameter(1,algo.RoughSlope());   
        func.SetLineWidth(1);   
        if result.start_point.w > result.end_point.w:
            func.SetRange(result.end_point.w-50,result.start_point.w+50);
     
        dwire=30;
        dtime=30*TMath.Tan(result.angle_2d*TMath.Pi()/180);
        if result.angle_2d > -90 and  result.angle_2d <0 :
            dwire=30;
            dtime=30*TMath.Tan(result.angle_2d*TMath.Pi()/180);
        elif result.angle_2d < -90. and  result.angle_2d > -180 :
            dwire=-30;
            dtime=30*TMath.Tan(-1*result.angle_2d*TMath.Pi()/180); 
        elif result.angle_2d > 90. and  result.angle_2d < 180 :
            dwire=-30;
            dtime=30*TMath.Tan(-1*result.angle_2d*TMath.Pi()/180);     
         
         
        angleline=TLine(result.start_point.w,result.start_point.t,result.start_point.w+dwire,result.start_point.t+dtime);
        angleline.SetLineColor(kBlack);
        angleline.SetLineWidth(3);

        # Draw Hit 2D View & points
        display.cd()
        hHits = algo.GetHitView()
        hHits.Draw("COLZ")
        begin.Draw("P same")
        end.Draw("P same")
        func.Draw("same");
        angleline.Draw("same");

        leg = TLegend(.7,.7,.95,.95)
        leg.AddEntry(begin, "Start Point (reco)","P")
        leg.AddEntry(end, "End Point (reco)","P")
        leg.AddEntry(angleline, "2D Angle")
        

        if(mc_begin):
            leg.AddEntry(mc_begin, "Start Point (mc)","P")
            mc_begin.Draw("P same")
        leg.Draw("same")
        # Update canvas
        display.Update()

        # Give an update on current status of ANN:
        

        # sys.stdin.readline()
        processed_events +=1
        print ("So far, processed_events is %g") % processed_events


s = str(nWrittenEvents)
outputfile.write(s)
outputfile.write(' ')
outputfile.write('13 2 \n')

fann.saveFANNToFile()
