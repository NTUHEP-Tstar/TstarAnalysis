#!/bin/env python
#*************************************************************************
#
#  Filename    : submit_cmsrun.py
#  Description : One Line descrption of file contents
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#   Additional comments
#
#*************************************************************************
import argparse
import os
import sys

import ManagerUtils.SysUtils.pluginProcessUtils as myproc
import ManagerUtils.SysUtils.pluginTimeUtils as mytime


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('scriptlist', type=str, nargs='+',
                        help='script files to process',
                        )
    parser.add_argument('-c', '--command', type=str, default='cmsRun',
                        help='Command to skip',
                        )
    parser.add_argument('-m', '--max', type=int, default=myproc.NumOfThreads() / 3,
                        help='Maximum number of parallel jobs'
                        )

    args = parser.parse_args()

    print args.scriptlist

    for script in args.scriptlist:
        while myproc.HasProcess(args.command) >= args.max:
            mytime.SleepMillSec(100)
            print "\rCurrently {0} instances of command [{1}] running [{2}]".format(
                myproc.HasProcess(args.command),
                args.command,
                mytime.CurrentDateTime()),
        cmd = script + " &> /dev/null &"
        os.system(cmd)
        # print cmd
        print "\rSubmitting script ", script
        mytime.SleepMillSec(1000)

    myproc.WaitProcess('cmsRun')

if __name__ == "__main__":
    main()
