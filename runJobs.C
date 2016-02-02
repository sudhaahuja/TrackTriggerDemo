 #include "ReadTree.C"
 void runJobs()
 {
	 TChain * chain = new TChain("ntupler/tree","");
	 //chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
	 //chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU200/TTbarTTbar_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.root");
	 //chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/Neutrino_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
	 chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU200/Neutrino_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.root");
	 //chain->Add("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU0/SingleMu_E2023TTI_ntuple.2.root");
	 //chain->Add("/uscms_data/d3/lpcbphy/zhenhu/AMTest/CMSSW_6_2_0_SLHC25_patch3/src/SLHCL1TrackTriggerSimulations/Configuration/test/MonoJet_pythia6Ptgun_ntuple_200GeV_1000evts_d.root");
	 ReadTree a(chain);
	 a.Loop();
 }
