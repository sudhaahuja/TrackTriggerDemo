#include "ReadTree.C"
void runReadTree()
{
	TChain * chain = new TChain("ntupler/tree","");
	//chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU200/Neutrino_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/Neutrino_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU0/SingleMu_E2023TTI_ntuple.2.root");
	//chain->Add("/uscms_data/d2/jiafu/L1TrackTrigger/CMSSW_6_2_0_SLHC12_patch1/src/SLHCL1TrackTriggerSimulations/Configuration/test/control/SingleMu_Barrel_pt5_E2023TTI_ntuple.root");
	TString name;
	for (int i=1; i<17; i++) {
		chain->Add(TString::Format("/eos/uscms/store/user/l1upgrades/noreplica/zhen/Neutrino/trackTrigger-neutrino-tt27-v2/151023_160934/0000/Neutrino_tt27_board00_PU200_ntuple_%d.root",i));
	}
	ReadTree a(chain);
	a.Loop();
}
