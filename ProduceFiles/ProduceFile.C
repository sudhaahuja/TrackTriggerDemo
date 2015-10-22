#define ProduceFile_cxx
#include "ProduceFile.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include "utils.C"

int fillPREFWithString(int board, std::vector< std::vector< std::bitset<24> > >BitsPREFaBoard);
int fillTxtfileWithString(int board, std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40]);
int fillTreeWithULong64(TTree *tree, ULong64_t OutputData[40][8], std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40]);


void ProduceFile::Loop()
{
	//Get a list of modules for a board
	unsigned OutputModule[10][40]={{0}};
	std::ifstream ifs ("ModuleListBoard00.txt", std::ifstream::in);
	for (int m=0; m<40; m++) {
		ifs >> OutputModule[0][m];
	}   
	ifs.close();


	//prepare module map for PREF
	std::map<int, pair<int,int>> GlbModule_LocalModule_Map_; //key: global module ID for the whole tracker; value: local module ID for this trigger tower
	int glbModuleID = 0;
	int localLad = 0;
	std::ifstream ifs2 ("ModuleList.txt", std::ifstream::in);
	std::string aLadder;
	while(std::getline(ifs2,aLadder)){
		std::stringstream modulestream(aLadder);
		int localMod = 0;
		while(modulestream >> glbModuleID){
			std::pair <int,int> localModuleID(localLad,localMod); 
			GlbModule_LocalModule_Map_[glbModuleID]=localModuleID;
			localMod++;
		}
		if (aLadder.length()) localLad++;
		else localLad = 0;
	}   
	ifs2.clear();
	ifs2.seekg(0, ios::beg);
	while(!ifs2.eof()){
		ifs2 >> glbModuleID;
		std::cout<<"Module:"<<glbModuleID<<" -> localLad="<<GlbModule_LocalModule_Map_[glbModuleID].first<<", localMod="<<GlbModule_LocalModule_Map_[glbModuleID].second<<std::endl;
	}   


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


	//prepare outputfiles
	TFile *outputfile = new TFile("DataSourcingTree.root", "RECREATE");
	/*TDirectory *Tower[48];  
	  sprintf(str,"Tower%02d",1);
	  Tower[0] = outputfile->mkdir(str);
	  Tower[0]->cd();*/
	TTree *tree[10]; //create 10 trees, each for a board 
	ULong64_t OutputData[40][8]={{0}}; //an array of 8 64bits unsigned integers for each link (40 links)
	for (int board=0; board<1; board++) {
		char str[50],str1[50];
		sprintf(str,"Board%02d",board);
		tree[board] = new TTree(str,str);
		for (int m=0; m<40; m++) { //create 40 branches in a borad, each for a module)
			sprintf(str,"Module_%06d",OutputModule[board][m]);
			sprintf(str1,"Module_%06d[8]/l",OutputModule[board][m]);
			tree[board]->Branch(str, OutputData[m], str1);
		}

		//prepare empty txt file for DataSourcing(DS)
		sprintf(str,"DataSourcingBoard%02d.txt",board);
		std::ofstream outfileDS;
		outfileDS.open(str);

		//prepare empty txt file for PREF
		sprintf(str,"PREF_Board%02d.txt",board);
		std::ofstream outfilePREF;
		outfilePREF.open(str);
	}


	if (fChain == 0) return;
	//Long64_t nentries = fChain->GetEntries();
	Long64_t nentries = 1;
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		const unsigned nstubs = TTStubs_modId->size();
		//if (jentry%100==0) 
		//std::cout<<std::endl<<"Processing event #"<<jentry<<", nstubs="<<nstubs<<std::endl;
		if (!nstubs) { // skip if no stub
			continue;
		}    
		if (nstubs > 1000000) {
			std::cout << "Way too many stubs: " << nstubs << std::endl;
			return;
		}
		//if (fabs(genParts_vz->at(0))>4) continue;

		int nStubL[10][40]={{0}};
		int nStubR[10][40]={{0}};
		bool CICStatusL[10][40]={{0}};
		bool CICStatusR[10][40]={{0}};
		//define header block for DS
		std::bitset<26> headerCICL, headerCICR;
		//define final output block for DS
		std::bitset<256> BitsCICL[10][40], BitsCICR[10][40];
		//define PREF block of a stub 
		std::vector< std::vector< std::vector< std::bitset<24> > > >BitsPREF;
		for (int b=0; b<10; b++){
			std::vector< std::vector< std::bitset<24> > >BitsPREFaBoard;
			for (int l=0; l<23; l++){
				std::vector< std::bitset<24> >BitsPREFaLayer;
				BitsPREFaBoard.push_back(BitsPREFaLayer);
			}
			BitsPREF.push_back(BitsPREFaBoard);
		}
		bool fillOutputFlag[10]={0};

		/**********************************************************************/
		/****write stub payload into the DataSourcing output block and PREF****/
		/**********************************************************************/
		for (unsigned l=0; l<nstubs; ++l) {
			//std::cout<<std::endl<<"Processing stubs #"<<l<<std::endl;
			//global coordinates
			unsigned moduleId   = TTStubs_modId->at(l); // std::cout<<std::endl<<"moduleID="<<moduleId<<std::endl;
			unsigned lay        = decodeLayer(moduleId);  //layer
			//unsigned lad        = decodeLadder(moduleId); //phi ladder for barrel; r ring for endcaps 
			//unsigned mod        = decodeModule(moduleId); //z module for barrel; phi module for endcaps 
			//local coordinates
			float localZ        = TTStubs_coordy->at(l);  //0-31(5bits) for PS (4bits for each CIC); 0-1 for 2S
			float localPhi      = TTStubs_coordx->at(l);  //960 strips for PS, 1016 strips for 2S. 2024 values in halfStrip = 11bits = 3bits(chipID)+8bits(stubAddress)
			float stub_trigBend = TTStubs_trigBend->at(l);//-8 to 8, 16 strips, 32 values(5bits) in halfStrip
			//unsigned stub_bx    = TTStubs_bxId->at(l);    //0-7(3bits) for bx
			unsigned stub_bx    = 0; 


			if (requirePtCut3 && !pass3GeVCut(moduleId,stub_trigBend)) continue;
			if (requireBlinding) {
				bool blindFlag = false;
				for (Long64_t i=0;i<blindEntries;i++) {
					blindTree->GetEntry(i);
					if (moduleId==unsigned(id) && (localPhi<phi1 || localPhi>phi2 || localZ<z1 || localZ>z2)) blindFlag = true;
				}
				if (blindFlag) continue;
			}


			std::bitset<3>  bxID;        bxID |= stub_bx;
			std::bitset<11> stubAddress; stubAddress |= int(localPhi*2); //chipID(3bits)+stubAddress(8bits)
			std::bitset<5>  stubBend2S;  stubBend2S |= int(stub_trigBend*2);  // int-->bitset-->ullong: [0,16]-->[00000,10000]-->[0,16]; [-16,-1]-->[10000,11111]-->[16,31]
			std::bitset<5>  stubBendPS;  stubBendPS |= int(stub_trigBend*2);  // if taking 3 bits: [0,3][-4,-1]-->[000,011][100,111]-->[0,3][4,7];  if taking 4 bits: [0,7][-8,-1]-->[0000,0111][1000,1111]-->[0,7][8,15]
			std::bitset<5>  stubZpos;    stubZpos |= int(localZ);//int-->bitset<5> -->bitset<4> -->ullong: [0,15]-->[00000,01111]-->[0000,1111]-->[0,15]; [16,31]-->[10000,11111]-->[0000,1111]-->[0,15]
			int nBitsFor2S = bxID.size()+stubAddress.size()+stubBend2S.size();
			int nBitsForPS = bxID.size()+stubAddress.size()+stubBendPS.size()+(stubZpos.size()-1);


			for (int board=0; board<1; board++) {        //loop 10 boards in this trigger tower (for now, only 1 board)
				for (int m=0; m<40; m++){                 //loop 40 modules in this board
					if (moduleId == OutputModule[board][m]) { //find out which module this stub belongs to
						std::cout<<std::endl<<"****************************************"<<std::endl;
						std::cout<<           "*********A NEW STUB IN BOARD: "<<board<<"*********"<<std::endl;
						std::cout<<           "****************************************"<<std::endl;
						fillOutputFlag[board]=1;

						/**********************************/
						/********fill bits for PREF********/
						/**********************************/
						std::bitset<4>  localLadder;      localLadder |= GlbModule_LocalModule_Map_[moduleId].first;
						std::bitset<4>  localModule;      localModule |= GlbModule_LocalModule_Map_[moduleId].second;
						std::bitset<24> PREFtmp;
						for (unsigned b=0;b<stubZpos.size();b++) {
							PREFtmp.set(b, stubZpos.test(b));
						}
						for (unsigned b=0;b<stubAddress.size();b++) {
							PREFtmp.set(b + stubZpos.size(), stubAddress.test(b));
						}
						for (unsigned b=0;b<localModule.size();b++) {
							PREFtmp.set(b + stubAddress.size() + stubZpos.size(), localModule.test(b));
						}

						for (unsigned b=0;b<localLadder.size();b++) {
							PREFtmp.set(b + localModule.size() + stubAddress.size() + stubZpos.size(), localLadder.test(b));
						}
						BitsPREF[board][lay].push_back(PREFtmp);

						for (int layer=5; layer<11; layer++) {
							std::cout<<"***** layer "<<layer<<" *****"<<std::endl;
							for (unsigned stub=0; stub<BitsPREF[board][layer].size(); stub++) {
								std::cout<<BitsPREF[board][layer][stub]<<std::endl;
							}
						}

						/**********************************/
						/****fill bits for DataSourcing****/
						/**********************************/
						if (lay>7) { //2S modules (layer 8,9,10)
							if (localZ<1) { //CIC L
								std::cout<<"This is the "<<nStubL[board][m]+1<<"th fired stub in 2S-L module "<<moduleId<<", which has bits:"<<std::endl;
								if (nStubL[board][m]<12) {
									for (unsigned b=0;b<bxID.size();b++) { //write bxID (3bits) for this stub into the output block
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsFor2S - bxID.size() + b, bxID.test(b));
									}
									std::cout<<"Set bxID: "<<stub_bx<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubAddress.size();b++) { //write chipID+stubAddress (11bits) for this stub into the output block
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsFor2S - bxID.size() - stubAddress.size() + b, stubAddress.test(b));
									}
									std::cout<<"Set chipID+stubAddress: "<<localPhi<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubBend2S.size();b++) { //write the stubBend bits for this stub into the output block
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsFor2S - bxID.size() - stubAddress.size() - stubBend2S.size() + b, stubBend2S.test(b));
									}
									std::cout<<"Set stubBend: "<<stub_trigBend<<std::endl<<BitsCICL[board][m]<<std::endl;
									nStubL[board][m]++;
								}
								else {
									CICStatusL[board][m]=1;
									std::cout<<"Bandwidth not enough for more fired stub in this 2S-L module!!!"<<std::endl;
								}
							}
							else { //CIC R
								std::cout<<"This is the "<<nStubR[board][m]+1<<"th fired stub in 2S-R module "<<moduleId<<", which has bits:"<<std::endl;
								if (nStubR[board][m]<12) {
									for (unsigned b=0;b<bxID.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsFor2S - bxID.size() + b, bxID.test(b));
									}
									std::cout<<"Set bxID: "<<stub_bx<<std::endl<<BitsCICR[board][m]<<std::endl;
									for (unsigned b=0;b<stubAddress.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsFor2S - bxID.size() - stubAddress.size() + b, stubAddress.test(b));
									}
									std::cout<<"Set chipID+stubAddress: "<<localPhi<<std::endl<<BitsCICR[board][m]<<std::endl;
									for (unsigned b=0;b<stubBend2S.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsFor2S - bxID.size() - stubAddress.size() - stubBend2S.size() + b, stubBend2S.test(b));
									}
									std::cout<<"Set stubBend: "<<stub_trigBend<<std::endl<<BitsCICR[board][m]<<std::endl;
									nStubR[board][m]++;
								}
								else {
									CICStatusR[board][m]=1;
									std::cout<<"Bandwidth not enough for more fired stubs in this 2S-R module!!!"<<std::endl;
								}
							}
						}
						else { //PS modules (layer 5,6,7)
							if (localZ<16) { //CIC L
								std::cout<<"This is the "<<nStubL[board][m]+1<<"th fired stub in PS-L module "<<moduleId<<", which has bits:"<<std::endl;
								if (nStubL[board][m]<10) {
									for (unsigned b=0;b<bxID.size();b++) { 
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsForPS - bxID.size() + b, bxID.test(b));
									}
									std::cout<<"Set bxID: "<<stub_bx<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubAddress.size();b++) {
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsForPS - bxID.size() - stubAddress.size() + b, stubAddress.test(b));
									}
									std::cout<<"Set chipID+stubAddress: "<<localPhi<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubBendPS.size();b++) {
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() + b, stubBendPS.test(b));
									}
									std::cout<<"Set stubBend: "<<stub_trigBend<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubZpos.size()-1;b++) { //write the stubZposition (4bits) for this stub into the output block
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() - (stubZpos.size()-1) + b, stubZpos.test(b));
									}
									std::cout<<"Set z position: "<<localZ<<std::endl<<BitsCICL[board][m]<<std::endl;
									nStubL[board][m]++;
								}
								else {
									CICStatusL[board][m]=1;
									std::cout<<"Bandwidth not enough for more fired stubs in this PS-L module!!!"<<std::endl;
								}
							}
							else { //CIC R
								std::cout<<"This is the "<<nStubR[board][m]+1<<"th fired stub in PS-R module "<<moduleId<<", which has bits:"<<std::endl;
								if (nStubR[board][m]<10) {
									for (unsigned b=0;b<bxID.size();b++) { 
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsForPS - bxID.size() + b, bxID.test(b));
									}
									std::cout<<"Set bxID: "<<stub_bx<<std::endl<<BitsCICR[board][m]<<std::endl;
									for (unsigned b=0;b<stubAddress.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsForPS - bxID.size() - stubAddress.size() + b, stubAddress.test(b));
									}
									std::cout<<"Set chipID+stubAddress: "<<localPhi<<std::endl<<BitsCICR[board][m]<<std::endl;
									for (unsigned b=0;b<stubBendPS.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() + b, stubBendPS.test(b));
									}
									std::cout<<"Set stubBend: "<<stub_trigBend<<std::endl<<BitsCICR[board][m]<<std::endl;
									for (unsigned b=0;b<stubZpos.size()-1;b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() - (stubZpos.size()-1) + b, stubZpos.test(b));
									}
									std::cout<<"Set z position: "<<localZ<<std::endl<<BitsCICR[board][m]<<std::endl;
									nStubR[board][m]++;
								}
								else {
									CICStatusR[board][m]=1;
									std::cout<<"Bandwidth not enough for more fired stubs in this PS module!!!"<<std::endl;
								}
							}
						}
					}
				}
			}
		}


		/**************************************/
		/**********write output files**********/
		/**************************************/
		//1. create DS header, then copy header into the DS output block, then fill the DS output file
		//2. fill the PREF output file
		for (int board=0; board<1; board++) {
			if (fillOutputFlag[board]==0) continue;
			for (int m=0; m<40; m++){
				/*********************************/
				/**********create header**********/
				/*********************************/
				//set number of stubs
				//std::cout<<"nStubL,R = "<<nStubL[board][m]<<", "<<nStubR[board][m]<<std::endl;
				std::bitset<4> NStubL,NStubR;
				NStubL |= nStubL[board][m];
				NStubR |= nStubR[board][m];
				for (unsigned b=0;b<NStubL.size();b++) {
					headerCICL.set(b, NStubL.test(b));
					headerCICR.set(b, NStubR.test(b));
				}

				//set bx ID
				std::bitset<12> GlbBxID;

				//set error/status bit
				headerCICL.set(0 + GlbBxID.size() + NStubL.size(), CICStatusL[board][m]);
				headerCICR.set(0 + GlbBxID.size() + NStubR.size(), CICStatusR[board][m]);

				//set module type
				if (decodeLayer(OutputModule[board][m])>7) { //2S module  
					headerCICL.set(headerCICL.size()-1,0); 
					headerCICR.set(headerCICL.size()-1,0); 
				}
				else { //PS module
					headerCICL.set(headerCICR.size()-1,1);
					headerCICR.set(headerCICR.size()-1,1);
				}

				//print header
				std::cout<<"Module "<<OutputModule[board][m]<<": HeaderCICL="<<headerCICL<<", HeaderCICR="<<headerCICR<<std::endl;

				/***************************************/
				/***copy header into the output block***/
				/***************************************/
				for (unsigned b=0;b<headerCICL.size();b++) {
					BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() + b, headerCICL.test(b));
					BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() + b, headerCICR.test(b));
				}
				headerCICL.reset();
				headerCICR.reset();
			}

			/******************************************/
			/**********fill the DS output file*********/
			/******************************************/
			fillTreeWithULong64(tree[board],OutputData,BitsCICL[board],BitsCICR[board]);
			fillTxtfileWithString(board,BitsCICL[board],BitsCICR[board]);

			/******************************************/
			/*********fill the PREF output file********/
			/******************************************/
			fillPREFWithString(board,BitsPREF[board]);
		}
	}

	outputfile->Write();
}


int fillPREFWithString(int board, std::vector< std::vector< std::bitset<24> > >BitsPREFaBoard) {
	std::ofstream outfile;
	char str[50];
	sprintf(str,"PREF_Board%02d.txt",board);
	outfile.open(str,std::ofstream::app);
	//outfile << "New event: \n";

	int nStubMax=0;
	int nStubLayer[23]={0};
	for (int l=5; l<11; l++) {
		nStubLayer[l] = BitsPREFaBoard[l].size();
		if (nStubMax<nStubLayer[l]) nStubMax=nStubLayer[l];
	}

	for (int s=0; s<nStubMax; s++) {
		for (int l=5; l<11; l++) {
			if (s<nStubLayer[l]) outfile << BitsPREFaBoard[l][s].to_string() << "";
			else outfile << "111111111111111111111111";
		}
		outfile << "\n";
	}

	outfile.close();
	return 0;
}


//Convert bitset to a string. Then save the string into the output txtfile
int fillTxtfileWithString(int board, std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40]) {
	std::ofstream outfile;
	char str[50];
	sprintf(str,"DataSourcingBoard%02d.txt",board);
	outfile.open(str,std::ofstream::app);
	outfile << "New event: \n";

	std::ifstream ifs ("ModuleListBoard00.txt", std::ifstream::in);
	for (int m=0; m<40; m++) {
		TString ModuleID;
		ifs >> ModuleID;
		outfile << "Module " << ModuleID << " " << BitsCICLaBoard[m].to_string() << " "<< BitsCICRaBoard[m].to_string() << "\n";
	}
	outfile << "\n";

	ifs.close();
	outfile.close();
	return 0;
}


//Convert bitset to an array of ULong64. Then save the array into the output tree
int fillTreeWithULong64(TTree *tree, ULong64_t OutputData[40][8], std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40]) {
	std::bitset<64> tmpL,tmpR;
	for (int m=0; m<40; m++) {
		for (int i=0; i<4; i++) { 
			tmpL.reset();
			tmpR.reset();
			for (int b=0; b<64; b++) { 
				tmpL.set(b, BitsCICLaBoard[m].test(b+i*64));
				tmpR.set(b, BitsCICRaBoard[m].test(b+i*64));
			}   
			OutputData[m][7-i] = tmpL.to_ullong();
			OutputData[m][3-i] = tmpR.to_ullong();
		}   
	}
	tree->Fill();
	return 0;
} 


