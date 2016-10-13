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

def IsData( dataset ):
    if re.match( '/.*/.*Run[0-9]{4}.*/.*' , dataset ):
        return True
    else :
        return False

def GetName( dataset ):
    data_part = dataset.split('/')
    if IsData( dataset ):
        return data_part[1]+'_'+data_part[2]
    else:
        primtag = data_part[1]
        primtagpart = primtag.split('_')
        retname = ""
        idx = 1
        while idx <= len(primtagpart): # limiting to 75 characters long
            if( len("_".join(primtagpart[0:idx])) < 70 ):
                retname = "_".join(primtagpart[0:idx])
            idx = idx + 1
        return retname

def GetPrimary( dataset ):
    data_part = dataset.split('/')
    return data_part[1]

def GetTaskName( tag, dataset, mode ):
    return "{}_{}_{}".format( tag , GetName(dataset), mode )

def GetCrabFile( tag, dataset, mode):
    return "{}/{}.py".format( my_settings.crab_cfg_dir , GetTaskName(tag,dataset,mode))

def GetCrabDir( tag, dataset, mode ):
    return "{}/crab_{}/".format( my_settings.crab_work_dir, GetTaskName(tag,dataset,mode) )

def GetCrabOutputDir( tag, dataset, mode ):
    return "{}/results/".format( GetCrabDir(tag,dataset,mode))

def GetStoreDir( tag, dataset, mode ):
    return "{}/EDM_Files/{}/{}/".format( my_settings.storage_dir , tag, mode )

def GetStoreFile( tag, dataset, mode, index ):
    return "{}/{}_{}.root".format( GetStoreDir(tag,dataset,mode) , GetName(dataset), index )

def GetStoreGlob( tag, dataset, mode ):
    return "{}/{}*.root".format( GetStoreDir(tag,dataset,mode), GetName(dataset) )

def GetScriptFile( tag, dataset, mode , index ):
    return "{}/{}_{}.sh".format( my_settings.script_dir, GetTaskName(tag,dataset,mode), index)

def GetHLT( dataset ):
    if IsData( dataset ):
        return 'TriggerResults::HLT'
    else:
        # ONLY Apply HLT selection to Data samples
        return ''
