#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : 00_gen_dataset.py
 #  Description : Generating Dataset list from input json file
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import optparse
import json
import ManagerUtils.EDMUtils.EDMFileList as my_list

def GetDatasetList( file_name ):
    setting_file = open( file_name , 'r' )
    setting = json.load( setting_file )
    setting_file.close()

    data_set_list = []
    for primary_tag, process_tag in [(x,y) for x in setting["Primary Tags"] for y in setting["Process Tags"] ]:
        process = process_tag.encode('ascii','ignore')
        primary = primary_tag.encode('ascii','ignore')
        data_set_list.extend( my_list.ListEDMDatasets(primary, process, 'MINIAOD*') )

    return data_set_list

def main():
    """
    Generates a file listing all globbing results from a given json file
    """
    parser = optparse.OptionParser()
    parser.add_option( '-i' , '--inputjson', dest='input', help='json file to process', default=None, type='string')
    parser.add_option( '-o' , '--outputfile', dest='output', help='output file' , default='test.txt', type='string')

    (opt, args) = parser.parse_args()

    if not opt.input:
        print "No Input found!"
        parser.print_help()
        return

    my_data_setlist = GetDatasetList( opt.input )

    output = open( opt.output , 'w' )
    for data_set in my_data_setlist :
        output.write( data_set + '\n' )
    output.close()
    return


if __name__ == "__main__":
    main()
