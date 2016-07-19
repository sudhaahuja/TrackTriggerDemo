#include "MakeBank.C"
void runMakeBank()
{
   TChain * chain = new TChain("ntupler/tree","");
   //chain->Add("/uscmst1b_scratch/lpc1/lpcmuon/zhenhu/FNAL/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/SingleMuon_tt27_board00_ntuple.root");
   //chain->Add("/uscmst1b_scratch/lpc1/lpcmuon/zhenhu/FNAL/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/SingleMuon_tt27_board00_fixedAngle_ntuple.root");
   //chain->Add("/uscms_data/d3/lpcbphy/zhenhu/AMTest/demo/CMSSW_6_2_0_SLHC25_patch3/src/results.root");
	chain->Add("/uscms/home/zhenhu/Public/TTdemo_integration/roads.root");
	MakeBank a(chain);
   a.Loop();
}
