#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : dumpsetlist.py
 #  Description : Dumping the list in the python
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import argparse

import TstarAnalysis.RunSequence.Naming as myname

def unique( seq ):
    seen = set()
    seen_add = seen.add
    return [x for x in seq if not (x in seen or seen_add(x))]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i','--input',type=str,help='input file of data set list')

    arg = parser.parse_args()

    primlist = []
    seclist  = []
    with open(arg.input) as f:
        datasetlist = f.readlines();
        for dataset in datasetlist:
            dataset = dataset.strip()

            primlist.append( myname.GetPrimary(dataset) )
            seclist.append( myname.GetSecondary(dataset) )

    primlist = unique(primlist)
    seclist  = unique(seclist)

    primlist = [ '\item \\texttt{{{0}}}'.format(x.replace('_', '\_')) for x in primlist ]
    seclist   = [ '\item \\texttt{{{0}}}'.format(x.replace('_', '\_')) for x in seclist ]

    for item in primlist :
        print item
    print "\n>>>\n"
    for item in  seclist:
        print item

if __name__ == '__main__':
    main()
