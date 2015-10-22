#define MakeBank_cxx
#include "MakeBank.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream> 
#include <fstream>
#include "utils.C"
#include <tr1/array>

void MakeBank::Loop()
{

	//Get the stub overlap blinding map
	TFile *blindMap = TFile::Open("stubOverlapHistosFramesOnlyTree_0p2_0p2_0p2_0p2_0p2_0p2.root","READ");
	TTree *blindTree;
	blindMap->GetObject("tmodule",blindTree);
	int id, phi1, z1, phi2, z2; 
	blindTree->SetBranchAddress("id", &id);
	blindTree->SetBranchAddress("x1", &phi1);
	blindTree->SetBranchAddress("y1", &z1);
	blindTree->SetBranchAddress("x2", &phi2);
	blindTree->SetBranchAddress("y2", &z2);
	Long64_t blindEntries = blindTree->GetEntries();

	std::vector<std::tr1::array<int, 12> > patternBank;
	float phiMin[6] = {0.564430,0.653554,0.641981,0.717273,0.658179,0.618448};
	float phiMax[6] = {1.791765,1.710419,1.756567,1.638922,1.673851,1.778293};
	float zMin[6] = {-6.7127,-6.7797,-5.2542,-9.5318,-9.5318,-9.5318};
	float zMax[6] = {26.9799,36.7048,47.7511,59.4103,78.7372,88.9935};


	if (fChain == 0) return;
	//Long64_t nentries = fChain->GetEntries();
	Long64_t nentries = 5217;
	Long64_t nbytes = 0, nb = 0;

	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb; 
		const unsigned nstubs = TTStubs_modId->size();
		if (jentry%100==0) 
			std::cout<<"Processing event #"<<jentry<<", nstubs="<<nstubs<<std::endl;
		if (!nstubs) { // skip if no stub
			continue;
		}    
		if (nstubs > 100000) {
			std::cout << "Way too many stubs: " << nstubs << std::endl;
			return;
		}   

		std::tr1::array<int, 12> aPattern;
		for (unsigned l=0; l<nstubs; ++l) {
			unsigned moduleId   = TTStubs_modId->at(l);
			float Glbphi        = TTStubs_phi->at(l);
			float Glbz          = TTStubs_z->at(l);
			float stub_trigBend = TTStubs_trigBend->at(l);
			float localZ        = TTStubs_coordy->at(l); 
			float localPhi      = TTStubs_coordx->at(l); 
			//std::cout<<moduleId<<" "<<Glbphi<<" "<<Glbz<<std::endl;

			if (requirePtCut3 && !pass3GeVCut(moduleId,stub_trigBend)) continue;
			if (requireBlinding) {
				bool blindFlag = false;
				for (Long64_t i=0;i<blindEntries;i++) {
					blindTree->GetEntry(i);
					if (moduleId==unsigned(id) && (localPhi<phi1 || localPhi>phi2 || localZ<z1 || localZ>z2)) blindFlag = true;
				}   
				if (blindFlag) continue;
			}   
			if (nstubs<6) continue;

			unsigned lay = decodeLayer(moduleId);

			switch (lay) {
				case 5: 
					aPattern[0] = ceil((Glbphi-phiMin[0])/((phiMax[0]-phiMin[0])/256));
					aPattern[1] = ceil((Glbz-zMin[0])/((zMax[0]-zMin[0])/4));
					break;
				case 6:  
					aPattern[2] = ceil((Glbphi-phiMin[1])/((phiMax[1]-phiMin[1])/256));
					aPattern[3] = ceil((Glbz-zMin[1])/((zMax[1]-zMin[1])/4));
					break;
				case 7:  
					aPattern[4] = ceil((Glbphi-phiMin[2])/((phiMax[2]-phiMin[2])/256));
					aPattern[5] = ceil((Glbz-zMin[2])/((zMax[2]-zMin[2])/4));
					break;
				case 8:  
					aPattern[6] = ceil((Glbphi-phiMin[3])/((phiMax[3]-phiMin[3])/256));
					aPattern[7] = ceil((Glbz-zMin[3])/((zMax[3]-zMin[3])/4));
					break;
				case 9:  
					aPattern[8] = ceil((Glbphi-phiMin[4])/((phiMax[4]-phiMin[4])/256));
					aPattern[9] = ceil((Glbz-zMin[4])/((zMax[4]-zMin[4])/4));
					break;
				case 10:  
					aPattern[10] = ceil((Glbphi-phiMin[5])/((phiMax[5]-phiMin[5])/256));
					aPattern[11] = ceil((Glbz-zMin[5])/((zMax[5]-zMin[5])/4));
					break;
				default: break;
			}
		}
		
		//std::cout<<"This pattern: "<<aPattern[0]<<" "<<aPattern[1]<<" "<<aPattern[2]<<" "<<aPattern[3]<<" "<<aPattern[4]<<" "<<aPattern[5]<<" "<<aPattern[6]<<" "<<aPattern[7]<<" "<<aPattern[8]<<" "<<aPattern[9]<<" "<<aPattern[10]<<" "<<aPattern[11]<<std::endl;
		bool badPattern = false;
		for (int i=0; i<5; i++) {
			if (aPattern[i*2]<1 || aPattern[i*2]>256 || aPattern[i*2+1]<1 || aPattern[i*2+1]>4) {
				badPattern=true;
				std::cout<<"This is a bad pattern: "<<"This pattern: "<<aPattern[0]<<" "<<aPattern[1]<<" "<<aPattern[2]<<" "<<aPattern[3]<<" "<<aPattern[4]<<" "<<aPattern[5]<<" "<<aPattern[6]<<" "<<aPattern[7]<<" "<<aPattern[8]<<" "<<aPattern[9]<<" "<<aPattern[10]<<" "<<aPattern[11]<<std::endl;
			}
		}
		if (badPattern) continue;

		bool newPattern = true;
		for(std::vector<std::tr1::array<int, 12> >::iterator it = patternBank.begin(); it != patternBank.end(); ++it) {
			std::tr1::array<int, 12> bankPattern = *it;
			//std::cout<<bankPattern[0]<<" "<<bankPattern[1]<<" "<<bankPattern[2]<<" "<<bankPattern[3]<<" "<<bankPattern[4]<<" "<<bankPattern[5]<<" "<<bankPattern[6]<<" "<<bankPattern[7]<<" "<<bankPattern[8]<<" "<<bankPattern[9]<<" "<<bankPattern[10]<<" "<<bankPattern[11]<<std::endl;   
			if (*it==aPattern) {
				newPattern = false;
				//std::cout<<"repeated pattern in the bank: "<<bankPattern[0]<<" "<<bankPattern[1]<<" "<<bankPattern[2]<<" "<<bankPattern[3]<<" "<<bankPattern[4]<<" "<<bankPattern[5]<<" "<<bankPattern[6]<<" "<<bankPattern[7]<<" "<<bankPattern[8]<<" "<<bankPattern[9]<<" "<<bankPattern[10]<<" "<<bankPattern[11]<<std::endl;
			}   
		}   
		if (newPattern) patternBank.push_back(aPattern);
		//std::cout<<"Bank size: "<<patternBank.size()<<std::endl;
	
	}

	std::cout<<"**************** BANK *****************"<<std::endl;
	char str[50]; int board=0;
	sprintf(str,"Bank_Board%02d.txt",board);
	std::ofstream outfile;
	outfile.open(str);
	for(std::vector<std::tr1::array<int, 12> >::iterator it = patternBank.begin(); it != patternBank.end(); ++it) {
		std::tr1::array<int, 12> bankPattern = *it;
		std::cout<<bankPattern[0]<<" "<<bankPattern[1]<<" "<<bankPattern[2]<<" "<<bankPattern[3]<<" "<<bankPattern[4]<<" "<<bankPattern[5]<<" "<<bankPattern[6]<<" "<<bankPattern[7]<<" "<<bankPattern[8]<<" "<<bankPattern[9]<<" "<<bankPattern[10]<<" "<<bankPattern[11]<<std::endl; 
		outfile<<bankPattern[0]<<" "<<bankPattern[1]<<" "<<bankPattern[2]<<" "<<bankPattern[3]<<" "<<bankPattern[4]<<" "<<bankPattern[5]<<" "<<bankPattern[6]<<" "<<bankPattern[7]<<" "<<bankPattern[8]<<" "<<bankPattern[9]<<" "<<bankPattern[10]<<" "<<bankPattern[11]<<std::endl;
	}
	std::cout<<"Final bank size: "<<patternBank.size()<<std::endl;

}

