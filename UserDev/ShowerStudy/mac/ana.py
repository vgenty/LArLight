import sys,array
from ROOT import *

# Tree variables
# _motherEnergy
# _daughterEnergy
# _dist_ToWall
# _dist_AlongTraj

ch=TChain("ana_tree")
ch.AddFile(sys.argv[1])

#
# We find numbers as a function of cut values.
# We define a set of cut values + container for numbers (produced per cut value)
#

# Cut value arrays
fid_cuts    = [ x*2   for x in xrange(30) ] # 5cm steps, spans from 0=>60 cm
length_cuts = [ x*2 for x in xrange(50)   ] # 5cm steps, spans from 0=>100 cm

# Efficiency arrays for first daughter
fid_eff    = [0 for x in fid_cuts]
length_eff = [0 for x in length_cuts]

# Energy containment arrays for first daughter
fid_ec    = [0 for x in fid_cuts]
length_ec = [0 for x in length_cuts]

# Calculate error bars for efficiency
fid_eff_error 	 	     = [0 for x in fid_cuts]
fid_eff_error_empty 	 = [0 for x in fid_cuts]
 
length_eff_error 		 = [0 for x in length_cuts]
length_eff_error_empty   = [0 for x in length_cuts]

hEC_Fid_vs_Length = TH2D("hEC_Fid_vs_Length",
                         "Energy Containment*Eff vs. Fid. vs. Length; Fid. Volume; Length Along Trunk",
                         30,0,60,50,0,100)

hTempEC = TH1D("hTempEC","Temporary EC histogram",100,0,1)
c=TCanvas("c","",600,500)

for x in xrange(len(fid_cuts)):
    
    hTempEC.Reset()
    fid_eff[x] = float(ch.GetEntries("_dist_ToWall>=%g" % fid_cuts[x])) / ch.GetEntries()
    ch.Draw("_daughterEnergy / _motherEnergy >> hTempEC","_dist_ToWall>=%g" % fid_cuts[x])
    fid_ec[x]  = hTempEC.GetMean()
    fid_eff_error[x] = sqrt( float(fid_eff[x] * (1-fid_eff[x])) / len(fid_cuts)) 

for x in xrange(len(length_cuts)):

    hTempEC.Reset()
    length_eff[x] = float(ch.GetEntries("_dist_AlongTraj>=%g" % length_cuts[x])) / ch.GetEntries()
    ch.Draw("_daughterEnergy / _motherEnergy >> hTempEC","_dist_AlongTraj>=%g" % length_cuts[x])
    length_ec[x] = hTempEC.GetMean()
    length_eff_error[x] = sqrt(float(length_eff[x] * (1 - length_eff[x]) ) /len(length_cuts))

for x in xrange(len(length_cuts)):

    my_length_cut = length_cuts[x]

    for y in xrange(len(fid_cuts)):

        my_fid_cut = fid_cuts[y]

        my_eff = float(ch.GetEntries("_dist_AlongTraj>=%g && _dist_ToWall>=%g" % (my_length_cut,my_fid_cut)))
        my_eff /= ch.GetEntries()

        hTempEC.Reset()
        ch.Draw("_daughterEnergy / _motherEnergy >> hTempEC",
                "_dist_AlongTraj>=%g && _dist_ToWall>=%g" % (my_length_cut,my_fid_cut))
        my_ec = hTempEC.GetMean()

        hEC_Fid_vs_Length.SetBinContent(y,x,my_ec*my_eff)

#for x in xrange(len(fid_cuts)):
#
#    print fid_cuts[x], fid_eff[x], fid_ec[x],'...',fid_eff[x]*fid_ec[x]
#    print fid_eff_error[x]
#
#print

fid_ec_xvalues = array.array('d', fid_cuts)
fid_ec_yvalues = array.array('d', fid_ec)
fid_ec_graph = TGraph(len(fid_cuts), fid_ec_xvalues, fid_ec_yvalues)
fid_ec_graph.SetMarkerStyle(20)
fid_ec_graph.SetMarkerColor(kRed)

  
fid_eff_xvalues = array.array('d', fid_cuts)
fid_eff_yvalues = array.array('d', fid_eff)
fid_eff_xerrors = array.array('d', fid_eff_error_empty)
fid_eff_yerrors = array.array('d', fid_eff_error)
fid_eff_graph = TGraphErrors(len(fid_cuts), fid_eff_xvalues, fid_eff_yvalues,fid_eff_xerrors,fid_eff_yerrors)
fid_eff_graph.SetMarkerStyle(20)
fid_eff_graph.SetMarkerColor(kBlue)

fid_conv_xvalues = array.array('d', fid_cuts)
fid_conv_yvalues = array.array('d', [fid_ec[x] * fid_eff[x] for x in xrange(len(fid_cuts))])
fid_conv_graph = TGraph(len(fid_cuts), fid_conv_xvalues, fid_conv_yvalues)
fid_conv_graph.SetMarkerStyle(20)
fid_conv_graph.SetMarkerColor(kBlack)

fid_ec_graph.SetMaximum(1)
fid_ec_graph.SetMinimum(0)

fid_ec_graph.Draw("AP")
fid_eff_graph.Draw("P")
fid_conv_graph.Draw("P")
c.Update()
sys.stdin.readline()


for x in xrange(len(length_cuts)):

    print length_cuts[x], length_eff[x], length_ec[x],'...',length_eff[x]*length_ec[x]

print

length_ec_xvalues = array.array('d', length_cuts)
length_ec_yvalues = array.array('d', length_ec)
length_ec_graph = TGraph(len(length_cuts), length_ec_xvalues, length_ec_yvalues)
length_ec_graph.SetMarkerStyle(20)
length_ec_graph.SetMarkerColor(kRed)

length_eff_xvalues = array.array('d', length_cuts)
length_eff_yvalues = array.array('d', length_eff)
length_eff_xerrors = array.array('d', length_eff_error_empty)
length_eff_yerrors = array.array('d', length_eff_error)
length_eff_graph = TGraphErrors(len(length_cuts), length_eff_xvalues, length_eff_yvalues,length_eff_xerrors,length_eff_yerrors)
length_eff_graph.SetMarkerStyle(20)
length_eff_graph.SetMarkerColor(kBlue)

length_conv_xvalues = array.array('d', length_cuts)
length_conv_yvalues = array.array('d', [length_ec[x] * length_eff[x] for x in xrange(len(length_cuts))])
length_conv_graph = TGraph(len(length_cuts), length_conv_xvalues, length_conv_yvalues)
length_conv_graph.SetMarkerStyle(20)
length_conv_graph.SetMarkerColor(kBlack)

length_ec_graph.SetMaximum(1)
length_ec_graph.SetMinimum(0)

length_ec_graph.Draw("AP")
length_eff_graph.Draw("P")
length_conv_graph.Draw("P")
c.Update()
sys.stdin.readline()

hEC_Fid_vs_Length.Draw("COLZ")
c.Update()
c.SaveAs("c.C")
sys.stdin.readline()
