#*******************************************************************************
 #
 #  Filename    : Naming.py
 #  Description : Naming conventions for this analysis
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
 #   Additional comments
 #
#*******************************************************************************
import TstarAnalysis.RunSequence.Settings as my_settings
import re ## Importing regular expression

def IsData( data_set ):
    if re.match( '/.*/.*Run[0-9]{4}.*/.*' , data_set ):
        return True
    else :
        return False

def GetName( data_set ):
    data_part = data_set.split('/')
    if IsData( data_set ):
        return data_part[1]+'_'+data_part[2]
    else:
        return data_part[1]

def GetTaskName( tag, data_set, mode ):
    return "{}_{}_{}".format( tag , GetName(data_set), mode )

def GetCrabFile( tag, data_set, mode):
    return "{}/{}.py".format( my_settings.crab_cfg_dir , GetTaskName(tag,data_set,mode))

def GetCrabDir( tag, data_set, mode ):
    return "{}/crab_{}/".format( my_settings.crab_work_dir, GetTaskName(tag,data_set,mode) )

def GetCrabOutputDir( tag, data_set, mode ):
    return "{}/results/".format( GetCrabDir(tag,data_set,mode))

def GetStoreDir( tag, data_set, mode ):
    return "{}/EDM_Files/{}/{}/".format( my_settings.storage_dir , tag, mode )

def GetStoreFile( tag, data_set, mode, index ):
    return "{}/{}_{}.root".format( GetStoreDir(tag,data_set,mode) , GetName(data_set), index )

def GetStoreGlob( tag, data_set, mode ):
    return "{}/{}*.root".format( GetStoreDir(tag,data_set,mode), GetName(data_set) )

def GetScriptFile( tag, data_set, mode , index ):
    return "{}/{}_{}.sh".format( my_settings.script_dir, GetTaskName(tag,data_set,mode), index)