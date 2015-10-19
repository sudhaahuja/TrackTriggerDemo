 #include "MakeHists.C"
 void runMakeHists()
 {
	 TChain * chain = new TChain("newtree","");
	 //chain->Add("Neutrino_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.8BX.root");
    //chain->Add("Neutrino_E2023TTI_PU200_TuneCUEP6S1_ntuple.2.8BX.root");
	 //chain->Add("TTbarTTbarPlus7PU140.root");
	 chain->Add("TTbarTTbarPlus7PU200.root");
	 //chain->Add("TTbarTTbarPlus15PU200.root");
	 //chain->Add("TTbarTTbarPlus11PU200.root");

	 MakeHists a(chain);
	 a.Loop(false);
 }
