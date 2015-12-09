#define ReadTree_cxx
#include "ReadTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream> 

void ReadTree::Loop()
{
	TFile *newfile = new TFile("Neutrino_E2023TTI_PU200_ntuple.8BX.root", "RECREATE");
	newfile->mkdir("ntupler");
	newfile->cd("ntupler");
	TTree newtree("tree","tree");

	Int_t TTStubs_size_new = 0;
	std::vector<unsigned int> TTStubs_bxId_new;
	std::vector<float>   TTStubs_coordx_new;
	std::vector<float>   TTStubs_coordy_new;
	std::vector<float>   TTStubs_trigBend_new;
	std::vector<unsigned int> TTStubs_modId_new;
	std::vector<float>   TTStubs_simEta_new;
	std::vector<float>   TTStubs_simPhi_new;
	std::vector<float>   TTStubs_simPt_new;
	std::vector<float>   TTStubs_eta_new;
	std::vector<float>   TTStubs_phi_new;
	std::vector<float>   TTStubs_r_new;
	std::vector<float>   TTStubs_x_new;
	std::vector<float>   TTStubs_y_new;
	std::vector<float>   TTStubs_z_new;

	Int_t genParts_size_new = 0;
	std::vector<unsigned int> genParts_bxId_new;
	std::vector<int>     genParts_charge_new;
	std::vector<int>     genParts_pdgId_new;
	std::vector<float>   genParts_eta_new;
	std::vector<float>   genParts_phi_new;
	std::vector<float>   genParts_pt_new;
	std::vector<float>   genParts_px_new;
	std::vector<float>   genParts_py_new;
	std::vector<float>   genParts_pz_new;
	std::vector<float>   genParts_vx_new;
	std::vector<float>   genParts_vy_new;
	std::vector<float>   genParts_vz_new;
	std::vector<float>   genParts_cotTheta_new;
	std::vector<float>   genParts_d0_new;
	std::vector<float>   genParts_dz_new;
	std::vector<float>   genParts_invPt_new;


	newtree.Branch("TTStubs_size",&TTStubs_size_new,"TTStubs_size/I");
	newtree.Branch("TTStubs_bxId", &TTStubs_bxId_new);
	newtree.Branch("TTStubs_coordx", &TTStubs_coordx_new);
	newtree.Branch("TTStubs_coordy", &TTStubs_coordy_new);
	newtree.Branch("TTStubs_trigBend", &TTStubs_trigBend_new);
	newtree.Branch("TTStubs_modId", &TTStubs_modId_new);
	newtree.Branch("TTStubs_simEta", &TTStubs_simEta_new);
	newtree.Branch("TTStubs_simPhi", &TTStubs_simPhi_new);
	newtree.Branch("TTStubs_simPt", &TTStubs_simPt_new);
	newtree.Branch("TTStubs_eta", &TTStubs_eta_new);
	newtree.Branch("TTStubs_phi", &TTStubs_phi_new);
	newtree.Branch("TTStubs_r", &TTStubs_r_new);
	newtree.Branch("TTStubs_x", &TTStubs_x_new);
	newtree.Branch("TTStubs_y", &TTStubs_y_new);
	newtree.Branch("TTStubs_z", &TTStubs_z_new);

	newtree.Branch("genParts_size",&genParts_size_new,"genParts_size/I");
	newtree.Branch("genParts_bxId", &genParts_bxId_new);
	newtree.Branch("genParts_charge", &genParts_charge_new);
	newtree.Branch("genParts_pdgId", &genParts_pdgId_new);
	newtree.Branch("genParts_eta", &genParts_eta_new);
	newtree.Branch("genParts_phi", &genParts_phi_new);
	newtree.Branch("genParts_pt", &genParts_pt_new);
	newtree.Branch("genParts_px", &genParts_px_new);
	newtree.Branch("genParts_py", &genParts_py_new);
	newtree.Branch("genParts_pz", &genParts_pz_new);
	newtree.Branch("genParts_vx", &genParts_vx_new);
	newtree.Branch("genParts_vy", &genParts_vy_new);
	newtree.Branch("genParts_vz", &genParts_vz_new);
	newtree.Branch("genParts_cotTheta", &genParts_cotTheta_new);
	newtree.Branch("genParts_d0", &genParts_d0_new);
	newtree.Branch("genParts_dz", &genParts_dz_new);
	newtree.Branch("genParts_invPt", &genParts_invPt_new);


	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;

	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		const unsigned nstubs    = TTStubs_modId->size();
		const unsigned ngenParts = genParts_pdgId->size();
		if (jentry%1==0) 
			std::cout<<"Processing event #"<<jentry<<", nstubs="<<nstubs<<std::endl;
		if (!nstubs) { // skip if no stub
			continue;
		}    
		if (nstubs > 100000) {
			std::cout << "Way too many stubs: " << nstubs << std::endl;
			return;
		} 		

		TTStubs_size_new  = TTStubs_size_new  + nstubs;
		genParts_size_new = genParts_size_new + ngenParts;
		//std::cout<<"n_new="<<TTStubs_size_new<<std::endl;

		for (unsigned l=0; l<nstubs; ++l) {
			TTStubs_bxId_new.push_back(jentry%8);
			TTStubs_coordx_new.push_back(TTStubs_coordx->at(l));
			TTStubs_coordy_new.push_back(TTStubs_coordy->at(l));
			TTStubs_trigBend_new.push_back(TTStubs_trigBend->at(l));
			TTStubs_modId_new.push_back(TTStubs_modId->at(l));
			TTStubs_simEta_new.push_back(TTStubs_simEta->at(l));
			TTStubs_simPhi_new.push_back(TTStubs_simPhi->at(l));
			TTStubs_simPt_new.push_back(TTStubs_simPt->at(l));
			TTStubs_eta_new.push_back(TTStubs_eta->at(l));
			TTStubs_phi_new.push_back(TTStubs_phi->at(l));
			TTStubs_r_new.push_back(TTStubs_r->at(l));
			TTStubs_x_new.push_back(TTStubs_x->at(l));
			TTStubs_y_new.push_back(TTStubs_y->at(l));
			TTStubs_z_new.push_back(TTStubs_z->at(l));
			//std::cout<<jentry<<" "<<jentry%8<<std::endl;
		}

		for (unsigned l=0; l<ngenParts; ++l) {
			genParts_bxId_new.push_back(jentry%8);
			genParts_charge_new.push_back(genParts_charge->at(l));
			genParts_pdgId_new.push_back(genParts_pdgId->at(l));
			genParts_eta_new.push_back(genParts_eta->at(l));
			genParts_phi_new.push_back(genParts_phi->at(l));
			genParts_pt_new.push_back(genParts_pt->at(l));
			genParts_px_new.push_back(genParts_px->at(l));
			genParts_py_new.push_back(genParts_py->at(l));
			genParts_pz_new.push_back(genParts_pz->at(l));
			genParts_vx_new.push_back(genParts_vx->at(l));
			genParts_vy_new.push_back(genParts_vy->at(l));
			genParts_vz_new.push_back(genParts_vz->at(l));
			genParts_cotTheta_new.push_back(genParts_cotTheta->at(l));
			genParts_d0_new.push_back(genParts_d0->at(l));
			genParts_dz_new.push_back(genParts_dz->at(l));
			genParts_invPt_new.push_back(genParts_invPt->at(l));
		}

		if ((jentry+1)%8==0) 
		//if ((jentry+1)%7==0) 
		{
			//std::cout<<"Fill n_new="<<TTStubs_size_new<<std::endl;
			newtree.Fill();
			TTStubs_coordx_new.clear();
			TTStubs_coordy_new.clear();
			TTStubs_modId_new.clear();
			TTStubs_trigBend_new.clear();
			TTStubs_bxId_new.clear();
			TTStubs_size_new=0;

			TTStubs_size_new = 0;
			TTStubs_bxId_new.clear();
			TTStubs_coordx_new.clear();
			TTStubs_coordy_new.clear();
			TTStubs_trigBend_new.clear();
			TTStubs_modId_new.clear();
			TTStubs_simEta_new.clear();
			TTStubs_simPhi_new.clear();
			TTStubs_simPt_new.clear();
			TTStubs_eta_new.clear();
			TTStubs_phi_new.clear();
			TTStubs_r_new.clear();
			TTStubs_x_new.clear();
			TTStubs_y_new.clear();
			TTStubs_z_new.clear();

			genParts_size_new = 0;
			genParts_bxId_new.clear();
			genParts_charge_new.clear();
			genParts_pdgId_new.clear();
			genParts_eta_new.clear();
			genParts_phi_new.clear();
			genParts_pt_new.clear();
			genParts_px_new.clear();
			genParts_py_new.clear();
			genParts_pz_new.clear();
			genParts_vx_new.clear();
			genParts_vy_new.clear();
			genParts_vz_new.clear();
			genParts_cotTheta_new.clear();
			genParts_d0_new.clear();
			genParts_dz_new.clear();
			genParts_invPt_new.clear();
		}
	}
	newfile->Write();
}
