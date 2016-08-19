#include "ProduceFile.C"
void runProduceFile()
{
	TChain * chain = new TChain("ntupler/tree","");
	//chain->Add("Neutrino_E2023TTI_PU200_ntuple.8BX.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU200/TTbarTTbar_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU200/Neutrino_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/Neutrino_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU0/SingleMu_E2023TTI_ntuple.2.root");
	//chain->Add("/uscms_data/d2/jiafu/L1TrackTrigger/CMSSW_6_2_0_SLHC12_patch1/src/SLHCL1TrackTriggerSimulations/Configuration/test/control/SingleMu_Barrel_pt5_E2023TTI_ntuple.root");
	//chain->Add("~zhenhu/FNAL/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/SingleMuon_tt27_board00_ntuple.root");
	//chain->Add("~zhenhu/FNAL/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/SingleMuon_tt27_board00_fixedAngle_ntuple.root");
	//chain->Add("/uscms/home/zhenhu/Public/TTdemo_integration/SingleMuon_tt27_PU0_ntuple_50ev.root");
	chain->Add("/uscms/home/zhenhu/Public/TTdemo_integration/MultiMuons_tt27_PU0_ntuple_10ev.root");

   //TString name;
	//for (int i=1; i<51; i++) {
	//	chain->Add(TString::Format("root://cmsxrootd.fnal.gov//store/user/l1upgrades/noreplica/zhen/Neutrino/trackTrigger-neutrino-tt27-v2/151023_160934/0000/Neutrino_tt27_board00_PU200_ntuple_%d.root",i));
	//}  
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/noreplica/zhen/Neutrino/trackTrigger-neutrino-tt27-v2/151023_160934/0000/Neutrino_tt27_board00_PU200_ntuple_100.root");
	//chain->Add("root://cmsxrootd.fnal.gov//store/user/l1upgrades/noreplica/zhen/Neutrino/trackTrigger-neutrino-tt27-v2/151023_160934/0000/Neutrino_tt27_board00_PU200_ntuple_123.root");

	//chain->Add("~zhenhu/FNAL/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/MonoJet_pythia6Ptgun_ntuple_50GeVb.root");

	ProduceFile a(chain);
	a.Loop();
}
