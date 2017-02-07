# BaseLineSelector

Baseline selections and variable caching to reduce the MINIAOD files to a size that could be handled by single user. For the reasoning and details for what the selection are, read the [documentation](http://cms.cern.ch/iCMS/analysisadmin/cadilines?id=1806&ancode=B2G-16-025&line=B2G-16-025). For object selections see the soruce code in [`src`](src).

## Selection and variable cached

### Muon

* Selection :
  1. Looser pt and eta selection
  3. ID/ISO selection
  2. Veto loose muons selection
* Cached variable:
  1. Mini isolation value.
  2. All Muon ID values.

### Electron

* Selection:
  1. Looser pt and eta selection
  2. ID selection
  3. Veto loose electron selection

* Cached variables:
  1. all ID selection values.
  2. Mini isolation value

### Jet

* Selection:
  1. pt and eta selection ( using smeared jet for Jets )
  2. Loose Jet ID selection

* Cached variables:
  1. JEC/JER value of jets.
  2. Smeared jet momentums (generated using phi as random number seed).

### Lepton seperator:

* Ensuring the output files is seperated according to Muon and electron channel selection.

### MET filter:

* Reading the results of MET filters

----


## Package Structure

* [`interface`](interface): Forwards declaration of EDM plugins.
* [`plugins`](plugins): EDM object interaction and main control flow defined here.
* [`src`](src): Object selection of variable caching.
* [`python`](python): Default settings for the plugins.
