from ROOT import gROOT
gROOT.ProcessLine (".x $ROOTCOREDIR/scripts/load_packages.C")
from ROOT import SH
print 'SampleHandler imported.',SH,SH.SampleHandler

myhandler = SH.SampleHandler()

