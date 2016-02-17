##
## For use with plot.py or plottrees.py,
## instead of specifying --variables and --cuts by command-line, you can put them
## in a file using the syntax below. Then in your command line you specify "--config ThisFile.py"
##
## The "cuts" list is only relevant for plottrees.py (you cannot apply cuts to histograms...). 
##
## The "variables" list replaces --variables.
##
## "histformat" allows you to change the binning of your histograms (only for plottrees.py),
## unfortunately. BUT you can rebin your histograms in plot.py, see "rebin" below.
##
## "rebin" allows you to have variable bin widths, or to Rebin. This works differently for 
## the two scripts:
##  - plottrees.py: Give a list specifying all the bin edges (including first and last). Make sure
##    that the bin edges agree with what you may have specified in "histformat"! (Basically the
##    bins you specify in "rebin" must have common edges with the one you specify in "histformat".)
##  - plot.py: You can either give a list specifying all the bin edges as above, or you can specify
##    the number (e.g. 2) that you want as the argument to a TH1::Rebin function. So either a list []
##    or a number.
##

cuts = [
#     'lepFlavor == 1', # electrons
    'lepFlavor == 2', # muons
    '60 > Mll || Mll > 110',
#     'HLT_e17_lhvloose == 1', # trigger
    ]

variables = [
    'Mll',
    'lepPt[lep0_index]',
    ]

histformat = {
    # In case you want to change the binning, you can do it here. Syntax:
    #'Variable':[nbins,xlow,xup,'My x-axis label (not necessary)'],
    'Mll':[100,0,200],
    'lepPt[lep0_index]':[100,0,100,'Leading lepton p_{T}'],
    }

rebin = {
    'lepPt':[0,9,15,20,25,30,40,50,60,80,100],
    }
