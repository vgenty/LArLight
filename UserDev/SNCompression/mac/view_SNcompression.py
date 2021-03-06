# Load libraries
import ROOT, sys, os
from ROOT import *
from ROOT import gSystem
from ROOT import compress

# Now import ana_processor & your class. For this example, ana_base.
gSystem.Load("libAnalysis")
gSystem.Load("libLArUtil")
gSystem.Load("libSNCompression")

from ROOT import larlight as fmwk

filename = sys.argv[1]

my_proc = fmwk.ana_processor()

my_proc.set_io_mode(fmwk.storage_manager.READ)

my_proc.set_data_to_read(fmwk.DATA.TPCFIFO)

my_proc.add_input_file(filename)

my_proc.set_input_rootdir("scanner")

#is there a way to disable ana_proc from creating an output file at all?
my_proc.set_ana_output_file("")

compAna=fmwk.ViewCompression()
compAna.suppressBaseline(True)
#add Compression Algorithm
compAlgo = compress.CompressionAlgosncompress()
compAlgo.SetDebug(False)
compAlgo.SetVerbose(True)
compAlgo.SetFillTree(False)
compAlgo.SetBlockSize(64)
compAlgo.SetBaselineThresh(0.75)
compAlgo.SetVarianceThresh(1)
f = 3./4
compAlgo.SetCompressThresh(-5,5,f*5)
compAlgo.SetMaxADC(4095)
compAlgo.SetUVYplaneBuffer(30,55,15,20,15,10);
#compAlgo.SetUVYplaneBuffer(15,20,15,15,15,15);
compAna.SetCompressAlgo(compAlgo)

my_proc.add_process(compAna)

gStyle.SetOptStat(0)
gStyle.SetTitleFontSize(0.06)
gStyle.SetTitleOffset(0.4,"X")
gStyle.SetTitleSize(0.06,"X")
gStyle.SetTitleOffset(0.8,"Y")
gStyle.SetTitleSize(0.06,"Y")
gStyle.SetLabelSize(0.06,"X")
gStyle.SetLabelSize(0.06,"Y")

c1 = TCanvas("c1","c1",900,900)
c1.Divide(1,2)

while my_proc.process_event():

    for x in range(compAna.GetNumWFs()):
        compAna.processWF()
        if (compAna.GetNumOutWFs() >= 1):
            print "%i Waveforms found"%compAna.GetNumOutWFs()
            for pad in xrange(1,3,1):
                c1.cd(pad)
                compAna.GetHistos(pad).Draw()
                basehisto = compAna.GetBaseHisto()
                basehisto.SetLineColor(2)
                basehisto.Draw("same")
                varhisto = compAna.GetVarHisto()
                varhisto.SetLineColor(3)
                varhisto.Draw("same")
            c1.Update()

            usrinput = raw_input("Hit Enter: next evt  ||  q: exit viewer\n")
            if ( usrinput == "q" ):
                sys.exit(0)


    

# done!
