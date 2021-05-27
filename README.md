# ExternalCMSSW
### Private Modules that are compatible with CMSSW for the BMTF project
- Validator
- PatternGenerator

### Pending changes
- tag the mismatches files with the run number (now mismatches files are overwritten)
- cli args for cosmic run cfg
- remove old leftover files
- clean up the code

### Installation
Assuming that you want to run these modules on top of the CMSSW release `CMSSW_X_Y_Z` the following steps are required
```bash
##
## make your working dir and prepare CMSSW
ssh lxplus.cern.ch
cd <workdir>
cmsrel CMSSW_X_Y_Z && cd CMSSW_X_Y_Z/src
cmsenv
##
## initialize extra repos
git cms-init --upstream-only
git clone https://github.com/panoskatsoulis/ExternalCMSSW.git
##
## fetch deps and compile
git cms-checkdeps -A -a
scram b -j 8
```

### Run Validation
The Validation code is located in the `MuonStudy` pkg. There are 2 different configuration files, one for Cosmic Runs and one for Collision Runs.
- Cosmic Runs    : [MuonStudy/test/validate_bothAlgos_Cosmics.py](../CMSSW_11_2_X/MuonStudy/test/validate_bothAlgos_Cosmics.py)
- Collision Runs : [MuonStudy/test/validate_bothAlgos_Collisions.py](../CMSSW_11_2_X/MuonStudy/test/validate_bothAlgos_Collisions.py)

**__For a Cosmics Run__**, one needs to modify the lines in the config file here [1] and then to run the following command
`cmsRun ExternalCMSSW/MuonStudy/test/validate_bothAlgos_Cosmics.py`

[1] https://github.com/panoskatsoulis/ExternalCMSSW/blob/CMSSW_11_2_X/MuonStudy/test/validate_bothAlgos_Cosmics.py#L20-L27

**__For a Collisions Run__**, modification is not needed. The parameters are exposed to cli arguments. An example is following
:warning: The combination of arguments are dummy, they cannot be used to run any example, please use OMS and DQM to find a good run and args :warning:
`cmsRun ExternalCMSSW/MuonStudy/test/validate_bothAlgos_Collisions.py run=325918 events=10000 dataset=/ExpressPhysics/Run2018D-Express-v1/FEVT gTag=112X_dataRun3_HLT_v3 `
