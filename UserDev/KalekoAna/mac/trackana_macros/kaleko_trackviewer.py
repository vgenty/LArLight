# Load libraries
import sys, os
try:
    import ROOT
except AttributeError:
    import ROOT
from ROOT import *
gSystem.Load("libKalekoAna")
gSystem.Load("libAnalysis")


# Now import ana_processor & your class. For this example, ana_base.
from ROOT import *

if len(sys.argv) != 3:
    print
    print "*** Improper usage. Usage: python kaleko_trackviewer.py /path/to/input/file.root track_reco_algorithm_name ***"
    print

# Create ana_processor instance
my_proc=larlight.ana_processor()

# Specify IO mode
my_proc.set_io_mode(larlight.storage_manager.READ)
#my_proc.set_io_mode(storage_manager.WRITE)
#my_proc.set_io_mode(storage_manager.BOTH)

# Set input root file: this is decoder output root file.
# This time, we use a sample file prepared.
my_proc.add_input_file(sys.argv[1])

larlight.storage_manager.get().set_in_rootdir("scanner")

# Set output root file: this is a separate root file in which your
# analysis module can store anything such as histograms, your own TTree, etc.
my_proc.set_ana_output_file("my_empty_trackviewer_output.root")

# Add analysis unit to process
my_ana = larlight.TrackViewer()

if sys.argv[2] == "Bezier":
    my_ana.set_data_type(larlight.DATA.Bezier)
elif sys.argv[2] == "Kalman3DSPS":
    my_ana.set_data_type(larlight.DATA.Kalman3DSPS)
elif sys.argv[2] == "Kalman3DHit":
    my_ana.set_data_type(larlight.DATA.Kalman3DHit)
else:
    print "Track reco algorithm must be Bezier, Kalman3DSPS, or Kalman3DHit. Try again."

my_proc.add_process(my_ana)

# Set TDirectory name in the ROOT file where TTrees can be found
# If no TDirectory, comment out.

ctr=0
c=TCanvas("c","",600,500)
gStyle.SetOptStat(0)

while my_proc.process_event():
    totalptctr = 0

    hMC   = my_ana.GetHisto_MC()
    hSPS  = my_ana.GetHisto_SPS()
    vReco = my_ana.GetHisto_Reco()
    #vData is a vector of <track> objects
    vData = my_ana.GetData_Reco()

    if hMC and hMC.GetEntries() > 0:
        hMC_zx = hMC.Project3D("zx")
        hMC_zx.SetLineColor(kGreen+3)
        hMC_zx.SetTitle("Event YZ Projection")
    else:
        hMC_zx = 0
    
    if hSPS and hSPS.GetEntries() > 0:
        hSPS_zx = hSPS.Project3D("zx")
        hSPS_zx.SetLineColor(kBlue-9)
        hSPS_zx.SetTitle("Event YZ Projection")
    else:
        hSPS_zx = 0
    
    vReco_zx = []; myline_zx = [];
    for x in xrange(vReco.size()):
        hReco_zx = vReco[x].Project3D("zx")
        hReco_zx.SetTitle("Event YZ Projection")
        vReco_zx.append(hReco_zx)

    first_draw=True

    for x in xrange(vData.size()):
        #if you want to look at angle b/t first two reco track points....
        reco_theta = vData.at(x).direction_at(0).Theta() * kaleko.UtilFunctions().DegreesPerRadian;
        #if you want to look at angle b/t first and last reco track points...
        #reco_theta = (vData.at(x).direction_at(vData.at(x).n_point()-1) - vData.at(x).direction_at(0)).Theta()*UtilFunctions().DegreesPerRadian;
        #if you want to look at a weighted average angle (weighted by distance b/t points)...
        #reco_theta = UtilFunctions().CalculateWeightedAvgTheta(vData.at(x)) * UtilFunctions().DegreesPerRadian;
        
        #print "Entry %d ... found %d spacepoints & %d reco-ed track" % (ctr,hSPS.GetEntries(),vReco.size())
        print "Plotting track_id %d. Number of track points: %d. reco_theta = %f degrees." % (vData[x].track_id(),vData[x].n_point(),reco_theta)
        if reco_theta < 95 and reco_theta > 85:
            print "reco_theta is bt 85 and 95"

        
        if hSPS_zx:
            if first_draw:
                hSPS_zx.Draw("BOX")
                first_draw=False
            
        if hMC_zx:
            if first_draw: 
                hMC_zx.Draw("BOX")
                first_draw=False
            else:
                hMC_zx.Draw("BOX sames")
                
        vReco_zx[x].SetLineColor(kRed)
        if first_draw:
            vReco_zx[x].Draw("BOX")
            first_draw=False
        else:
            vReco_zx[x].Draw("BOX sames")

        for pt in xrange(vData[x].n_point()-1):
            myline_zx.append(TLine(vData.at(x).vertex_at(pt).Z(),vData.at(x).vertex_at(pt).Y(),vData.at(x).vertex_at(pt+1).Z(),vData.at(x).vertex_at(pt+1).Y()))
                
            myline_zx[totalptctr].SetLineWidth(2)
            myline_zx[totalptctr].SetLineColor(kMagenta)
            myline_zx[totalptctr].Draw("sames")
            totalptctr+=1
                
            c.Update()
                
        sys.stdin.readline()

    print "continuing to next evt..."
    ctr+=1
# done!
