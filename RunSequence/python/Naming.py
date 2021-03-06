#*************************************************************************
#
#  Filename    : Naming.py
#  Description : Naming conventions for this analysis
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#   Additional comments
#
#*************************************************************************
import os
import re  # Importing regular expression

import ManagerUtils.SysUtils.EOSUtils as myeos
import TstarAnalysis.RunSequence.Settings as mysetting
import ManagerUtils.SysUtils.pluginPathUtils as mypath


#-------------------------------------------------------------------------
#   Dataset naming
#-------------------------------------------------------------------------
def IsData(dataset):
    if re.match( r'/.*/.*Run[0-9]{4}.*/.*', dataset):
        return True
    else:
        return False

def GetName(dataset):
    data_part = dataset.split('/')
    if IsData(dataset):
        return data_part[1] + '_' + data_part[2]
    else:
        primtag = data_part[1]
        primtagpart = primtag.split('_')
        retname = ""
        idx = 1
        while idx <= len(primtagpart):  # limiting to 75 characters long
            if(len("_".join(primtagpart[0:idx])) < 70):
                retname = "_".join(primtagpart[0:idx])
            idx = idx + 1
        return retname

def GetPrimary(dataset):
    data_part = dataset.split('/')
    return data_part[1]

def GetSecondary(dataset):
    datapart = dataset.split('/')
    return datapart[2]

def GetHLT(dataset):
    if IsData(dataset):
        return 'TriggerResults::HLT'
    else:
        # ONLY Apply HLT selection to Data samples
        return ''


def GetTaskName(tag, dataset, mode):
    return "{}_{}_{}".format(tag, GetName(dataset), mode)


def GetCrabFile(tag, dataset, mode):
    return "{}/{}.py".format(mysetting.crab_cfg_dir, GetTaskName(tag, dataset, mode))


def GetCrabDir(tag, dataset, mode):
    return "{}/crab_{}/".format(mysetting.crab_work_dir, GetTaskName(tag, dataset, mode))


def GetScriptFile(tag, dataset, mode, index):
    return "{}/{}_{}.sh".format(mysetting.script_dir, GetTaskName(tag, dataset, mode), index)

def GetTempOutput( tag, dataset,mode,index):
    return "/tmp/{}_{}.root".format(GetTaskName(tag,dataset,mode),index)

#-------------------------------------------------------------------------
#   EDM Files will be stored in defined EOS space
#-------------------------------------------------------------------------
def GetCrabOutputFileList(tag, dataset, mode):
    "Returns the directory at remote path"
    # Getting variables
    siteurl = mysetting.crab_siteurl
    crabbase = mysetting.xrd_default_path
    query = crabbase + '/' + GetPrimary(dataset) + '/' + 'crab_' + GetTaskName(tag, dataset, mode)

    #
    timeddirlist = myeos.listremote(siteurl, query)
    timestamp = max([os.path.basename(x) for x in timeddirlist])
    querylist = myeos.listremote(siteurl, query + '/' + timestamp)
    ans = []
    for query in querylist:
        filelist = myeos.listremote(siteurl, query)
        ans.extend([x for x in filelist if x.endswith('.root')])

    return ans


def GetEDMStoreDir(tag, dataset, mode):
    "Returns the directory at the remote path"
    return "{}/{}/{}/".format(mysetting.edmstorage_dir, tag, mode)


def GetEDMStoreFile(tag, dataset, mode, index):
    return "{}/{}_{}.root".format(GetEDMStoreDir(tag, dataset, mode), GetName(dataset), index)


def GetEDMStoreGlob(tag, dataset, mode):
    return "{}/{}*.root".format(GetEDMStoreDir(tag, dataset, mode), GetName(dataset))

def GetEDMStoreRegex(tag, dataset, mode):
    return "{}/{}_[0-9]*\.root".format(GetEDMStoreDir(tag, dataset, mode), GetName(dataset))

#-------------------------------------------------------------------------------
#   Global tag
#-------------------------------------------------------------------------------
def GetGlobalTag( dataset ):
    if re.match( r'/.*/.*Run2016[B-G]-03Feb2017.*/.*', dataset):
        return '80X_dataRun2_2016SeptRepro_v7'
    elif re.match( r'/.*/.*Run2016H-03Feb2017.*/.*', dataset):
        return '80X_dataRun2_Prompt_v16'
    elif re.match( r'/.*/RunIISummer16MiniAODv2.*PUMoriond17.*/MINIAODSIM', dataset ):
        return '80X_mcRun2_asymptotic_2016_TrancheIV_v6'
    else:
        raise Execption("Data set not recognized!")

#-------------------------------------------------------------------------------
#   JEC/JER naming options
#-------------------------------------------------------------------------------
def JECDBName( jecversion ):
    jecfile   = "JEC_" + jecversion + ".db"
    return "sqlite:" + jecfile

def JECTagName( jecversion, jettype ):
    return 'JetCorrectorParametersCollection_' + jecversion + '_' + jettype

def GetJECFromGT( globaltag ):
    if( globaltag == mysetting.data_global_tag ):
        return "Summer16_23Sep2016AllV3_DATA"
    else:
        return  "Summer16_23Sep2016V3_MC"

def JERDBName( jerversion ):
    jerfile   = "JER_" + jerversion + ".db"
    return "sqlite:" + jerfile

def JERTagName( jerversion, jettype ):
    return 'JR_' + jerversion + '_PtResolution_' + jettype

def JERSFTagName( jerversion, jettype ):
    return 'JR_' + jerversion + '_SF_' + jettype

def GetJERFromGT( globaltag ):
    if( globaltag == mysetting.data_global_tag ):
        return "Spring16_25nsV6_DATA"
    else:
        return  "Spring16_25nsV6_MC"
