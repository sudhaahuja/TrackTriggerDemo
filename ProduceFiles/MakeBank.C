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
	TFile *blindMap = TFile::Open("../inputfiles/stubOverlapHistosFramesOnlyTree_0p2_0p2_0p2_0p2_0p2_0p2.root","READ");
	TTree *blindTree;
	blindMap->GetObject("tmodule",blindTree);
	int id, phi1, z1, phi2, z2; 
	blindTree->SetBranchAddress("id", &id);
	blindTree->SetBranchAddress("x1", &phi1);
	blindTree->SetBranchAddress("y1", &z1);
	blindTree->SetBranchAddress("x2", &phi2);
	blindTree->SetBranchAddress("y2", &z2);
	Long64_t blindEntries = blindTree->GetEntries();

	std::vector<std::pair<Long64_t, std::tr1::array<int, 12> > > patternBank;
	float phiMin[6] = {0.564430,0.653554,0.641981,0.717273,0.658179,0.618448};
	float phiMax[6] = {1.791765,1.710419,1.756567,1.638922,1.673851,1.778293};
	float zMin[6] = {-6.7127,-6.7797,-5.2542,-9.5318,-9.5318,-9.5318};
	float zMax[6] = {26.9799,36.7048,47.7511,59.4103,78.7372,88.9935};


	if (fChain == 0) return;
	//Long64_t nentries = fChain->GetEntries();
	Long64_t nentries = 5478;
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
		if (nstubs<6) continue;

		std::pair<Long64_t, std::tr1::array<int, 12> > aPattern;
		aPattern.first=jentry;
		aPattern.second.assign(-1);
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

			unsigned lay = decodeLayer(moduleId);

			switch (lay) {
				case 5: 
					aPattern.second[0] = floor((Glbphi-phiMin[0])/((phiMax[0]-phiMin[0])/256));
					aPattern.second[1] = floor((Glbz-zMin[0])/((zMax[0]-zMin[0])/4));
					break;
				case 6:  
					aPattern.second[2] = floor((Glbphi-phiMin[1])/((phiMax[1]-phiMin[1])/256));
					aPattern.second[3] = floor((Glbz-zMin[1])/((zMax[1]-zMin[1])/4));
					break;
				case 7:  
					aPattern.second[4] = floor((Glbphi-phiMin[2])/((phiMax[2]-phiMin[2])/256));
					aPattern.second[5] = floor((Glbz-zMin[2])/((zMax[2]-zMin[2])/4));
					break;
				case 8:  
					aPattern.second[6] = floor((Glbphi-phiMin[3])/((phiMax[3]-phiMin[3])/256));
					aPattern.second[7] = floor((Glbz-zMin[3])/((zMax[3]-zMin[3])/4));
					break;
				case 9:  
					aPattern.second[8] = floor((Glbphi-phiMin[4])/((phiMax[4]-phiMin[4])/256));
					aPattern.second[9] = floor((Glbz-zMin[4])/((zMax[4]-zMin[4])/4));
					break;
				case 10: 
					aPattern.second[10] = floor((Glbphi-phiMin[5])/((phiMax[5]-phiMin[5])/256));
					aPattern.second[11] = floor((Glbz-zMin[5])/((zMax[5]-zMin[5])/4));
					break;
				default: break;
			}
		}
		//std::cout<<"This pattern("<<aPattern.first<<"): "<<aPattern.second[0]<<" "<<aPattern.second[1]<<" "<<aPattern.second[2]<<" "<<aPattern.second[3]<<" "<<aPattern.second[4]<<" "<<aPattern.second[5]<<" "<<aPattern.second[6]<<" "<<aPattern.second[7]<<" "<<aPattern.second[8]<<" "<<aPattern.second[9]<<" "<<aPattern.second[10]<<" "<<aPattern.second[11]<<std::endl;
		
		bool badPattern = false;
		for (int i=0; i<6; i++) {
			if (aPattern.second[i*2]<0 || aPattern.second[i*2]>255 || aPattern.second[i*2+1]<0 || aPattern.second[i*2+1]>3) {
				badPattern=true;
				std::cout<<"This is a bad pattern ("<<aPattern.first<<"): "<<aPattern.second[0]<<" "<<aPattern.second[1]<<" "<<aPattern.second[2]<<" "<<aPattern.second[3]<<" "<<aPattern.second[4]<<" "<<aPattern.second[5]<<" "<<aPattern.second[6]<<" "<<aPattern.second[7]<<" "<<aPattern.second[8]<<" "<<aPattern.second[9]<<" "<<aPattern.second[10]<<" "<<aPattern.second[11]<<std::endl;
			}
		}
		if (badPattern) continue;

		bool newPattern = true;
		for(std::vector<std::pair<Long64_t, std::tr1::array<int, 12> > >::iterator it = patternBank.begin(); it != patternBank.end(); ++it) {
			std::pair<Long64_t, std::tr1::array<int, 12> >aBankPattern = *it;
			//std::cout<<aBankPattern.second[0]<<" "<<aBankPattern.second[1]<<" "<<aBankPattern.second[2]<<" "<<aBankPattern.second[3]<<" "<<aBankPattern.second[4]<<" "<<aBankPattern.second[5]<<" "<<aBankPattern.second[6]<<" "<<aBankPattern.second[7]<<" "<<aBankPattern.second[8]<<" "<<aBankPattern.second[9]<<" "<<aBankPattern.second[10]<<" "<<aBankPattern.second[11]<<std::endl;   
			if (aBankPattern.second==aPattern.second) {
				newPattern = false;
				std::cout<<"repeated pattern in the bank ("<<aBankPattern.first<<"): "<<aBankPattern.second[0]<<" "<<aBankPattern.second[1]<<" "<<aBankPattern.second[2]<<" "<<aBankPattern.second[3]<<" "<<aBankPattern.second[4]<<" "<<aBankPattern.second[5]<<" "<<aBankPattern.second[6]<<" "<<aBankPattern.second[7]<<" "<<aBankPattern.second[8]<<" "<<aBankPattern.second[9]<<" "<<aBankPattern.second[10]<<" "<<aBankPattern.second[11]<<std::endl;
			}   
		}   
		if (newPattern) patternBank.push_back(aPattern);
		//std::cout<<"Bank size: "<<patternBank.size()<<std::endl;
	
	}

	std::cout<<"**************** BANK *****************"<<std::endl;
	char str[50]; int board=0;
	sprintf(str,"outputfiles/Bank_Board%02d.txt",board);
	std::ofstream outfile;
	outfile.open(str);
	for(std::vector<std::pair<Long64_t, std::tr1::array<int, 12> > >::iterator it = patternBank.begin(); it != patternBank.end(); ++it) {
		std::pair<Long64_t, std::tr1::array<int, 12> >aBankPattern = *it;
		std::cout<<aBankPattern.first<<" "<<aBankPattern.second[0]<<" "<<aBankPattern.second[1]<<" "<<aBankPattern.second[2]<<" "<<aBankPattern.second[3]<<" "<<aBankPattern.second[4]<<" "<<aBankPattern.second[5]<<" "<<aBankPattern.second[6]<<" "<<aBankPattern.second[7]<<" "<<aBankPattern.second[8]<<" "<<aBankPattern.second[9]<<" "<<aBankPattern.second[10]<<" "<<aBankPattern.second[11]<<std::endl; 
		outfile  <<aBankPattern.first<<" "<<aBankPattern.second[0]<<" "<<aBankPattern.second[1]<<" "<<aBankPattern.second[2]<<" "<<aBankPattern.second[3]<<" "<<aBankPattern.second[4]<<" "<<aBankPattern.second[5]<<" "<<aBankPattern.second[6]<<" "<<aBankPattern.second[7]<<" "<<aBankPattern.second[8]<<" "<<aBankPattern.second[9]<<" "<<aBankPattern.second[10]<<" "<<aBankPattern.second[11]<<std::endl;
	}
	std::cout<<"Final bank size: "<<patternBank.size()<<std::endl;
	outfile.close();
}

