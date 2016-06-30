#define ProduceFile_cxx
#include "ProduceFile.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <tr1/array>

int fillGenSimInfo(int board, std::vector< std::tr1::array<float, 23> > simABoard);
int fillPRBF2WithString(std::vector< std::vector< std::bitset<32> > >BitsPRBF2aBx, int jentry);
int fillTxtfileWithString(int board, std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40], std::vector< std::vector< std::vector<unsigned> > > BoardModuleMap, int jentry);
int fillTreeWithULong64(TTree *tree, ULong64_t OutputData[40][8], std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40]);


void ProduceFile::Loop()
{
	//Get a list of modules for a board
	std::vector< std::vector< std::vector<unsigned> > > BoardModuleMap;
	for (int board = 0; board<10; board++){
		std::vector< std::vector<unsigned> > Modules_aBoard;
		char str[50];
		sprintf(str,"../inputfiles/ModuleMap%02d.txt",board+1);
		std::ifstream ifs (str, std::ifstream::in);
		std::string aLink;
		while(std::getline(ifs,aLink)){
			std::vector<unsigned> Modules_aLink;
			std::stringstream modulestream(aLink);
			unsigned moduleID;
			while(modulestream >> moduleID){
				Modules_aLink.push_back(moduleID);
			}
			Modules_aBoard.push_back(Modules_aLink);
		}
		ifs.close();
		BoardModuleMap.push_back(Modules_aBoard);
	}   
	for(std::vector< std::vector< std::vector<unsigned> > >::iterator board = BoardModuleMap.begin(); board != BoardModuleMap.end(); ++board) {
		for(std::vector< std::vector <unsigned> >::iterator link = board->begin(); link != board->end(); ++link) {
			for(std::vector <unsigned>::iterator mod = link->begin(); mod != link->end(); ++mod) {
			std::cout<<*mod<<" ";
			}
		}
		std::cout<<std::endl;
	}

	//prepare module map for PRBF.2
	std::map<unsigned, pair<int,int>> GlbModule_LocalModule_Map_; //key: global module ID for the whole tracker; value: local module ID for this trigger tower
	std::vector<unsigned> glbModuleIDs;
	unsigned glbModuleID;
	int localLad = 0;
	std::ifstream ifs2 ("../inputfiles/ModuleList.txt", std::ifstream::in);
	std::string aLadder;
	while(std::getline(ifs2,aLadder)){
		std::stringstream modulestream(aLadder);
		int localMod = 0;
		while(modulestream >> glbModuleID){
			glbModuleIDs.push_back(glbModuleID);
			std::pair <int,int> localModuleID(localLad,localMod); 
			GlbModule_LocalModule_Map_[glbModuleID]=localModuleID;
			localMod++;
		}
		if (aLadder.length()) localLad++;
		else localLad = 0;
	}   
	//ifs2.clear();
	//ifs2.seekg(0, ios::beg);
	//while(!ifs2.eof()){
	//	ifs2 >> glbModuleID;
	//	std::cout<<"Module:"<<glbModuleID<<" -> localLad="<<GlbModule_LocalModule_Map_[glbModuleID].first<<", localMod="<<GlbModule_LocalModule_Map_[glbModuleID].second<<std::endl;
	//}
	for(std::vector<unsigned>::iterator it = glbModuleIDs.begin(); it != glbModuleIDs.end(); ++it) {
		std::cout<<"Module:"<<*it<<" -> localLad="<<GlbModule_LocalModule_Map_[*it].first<<", localMod="<<GlbModule_LocalModule_Map_[*it].second<<std::endl;
	}


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


	TH1F *InputLatency = new TH1F("InputLatency","InputLatency",200,0,200);
	InputLatency->GetXaxis()->SetTitle("the maximum stubRate/layer among all layers");
	InputLatency->GetYaxis()->SetTitle("Entries");

	//prepare outputfiles
	TFile *outputfile = new TFile("outputfiles/DataSourcingTree.root", "RECREATE");
	/*TDirectory *Tower[48];  
	  sprintf(str,"Tower%02d",1);
	  Tower[0] = outputfile->mkdir(str);
	  Tower[0]->cd();*/
	TTree *tree[BoardModuleMap.size()]; //create 10 trees, each for a board 
	ULong64_t OutputData[40][8]={{0}}; //an array of 8 64bits unsigned integers for each link (40 links)
	for (unsigned board=0; board<BoardModuleMap.size(); board++) {
		char str[50],str1[50];
		sprintf(str,"Board%02d",board);
		tree[board] = new TTree(str,str);
		for (int m=0; m<40; m++) { //create 40 branches in a borad, each for a link)
			sprintf(str,"Module_%06d",BoardModuleMap[board][m][0]);
			sprintf(str1,"Module_%06d[8]/l",BoardModuleMap[board][m][0]);
			tree[board]->Branch(str, OutputData[m], str1);
		}

		//prepare empty txt file for DataSourcing(DS)
		sprintf(str,"outputfiles/DataSourcingBoard%02d.txt",board);
		std::ofstream outfileDS;
		outfileDS.open(str);
	}
	//prepare empty txt file for PRBF.2 and sim
	std::ofstream outfilePRBF2;
	outfilePRBF2.open("outputfiles/PRBF2.txt");
	std::ofstream outfileSIM;
	outfileSIM.open("outputfiles/sim.txt");

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntries();
	//Long64_t nentries = 1;
	//Long64_t nentries = 351;
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		const unsigned nstubs = TTStubs_modId->size();
		//if (jentry%100==0) 
		std::cout<<std::endl<<"Processing event #"<<jentry<<", nstubs="<<nstubs<<std::endl;
		if (!nstubs) { // skip if no stub
			continue;
		}    
		if (nstubs > 1000000) {
			std::cout << "Way too many stubs: " << nstubs << std::endl;
			return;
		}
		//if (fabs(genParts_vz->at(0))>4) continue;

		int nStubL[10][40]={{0}};  //make sure BoardModuleMap.size()<=10;
		int nStubR[10][40]={{0}};
		std::vector< std::vector<std::tr1::array<float, 23> > >simInfo;
		for (unsigned b=0; b<BoardModuleMap.size(); b++){ //10 boards
			std::vector<std::tr1::array<float, 23> >simInfoABoard;
			simInfo.push_back(simInfoABoard);
		}
		bool CICStatusL[10][40]={{0}};
		bool CICStatusR[10][40]={{0}};
		//define header block for DS
		std::bitset<26> headerCICL, headerCICR;
		//define final output block for DS
		std::bitset<256> BitsCICL[BoardModuleMap.size()][40], BitsCICR[BoardModuleMap.size()][40];
		//define PRBF2 block of a stub
		std::vector< std::vector< std::vector< std::bitset<32> > > >BitsPRBF2;
		for (int b=0; b<40; b++){ //40 bx
			std::vector< std::vector< std::bitset<32> > >BitsPRBF2aBx;
			for (int l=0; l<23; l++){ //23 layers
				std::vector< std::bitset<32> >BitsPRBF2aLayer;
				BitsPRBF2aBx.push_back(BitsPRBF2aLayer);
			}
			BitsPRBF2.push_back(BitsPRBF2aBx);
		}
		bool fillOutputFlag=0;


		/************************************************************************/
		/****write stub payload into the DataSourcing output block and PRBR.2****/
		/************************************************************************/
		for (unsigned l=0; l<nstubs; ++l) {
			//std::cout<<std::endl<<"Processing stubs #"<<l<<std::endl;
			//global coordinates
			unsigned moduleId   = TTStubs_modId->at(l); // std::cout<<std::endl<<"moduleID="<<moduleId<<std::endl;
			unsigned lay        = decodeLayer(moduleId);  //layer
			unsigned lad        = decodeLadder(moduleId); //phi ladder for barrel; r ring for endcaps 
			//unsigned mod        = decodeModule(moduleId); //z module for barrel; phi module for endcaps 
			//local coordinates
			float localZ        = TTStubs_coordy->at(l);  //0-31(5bits) for PS (4bits for each CIC); 0-1 for 2S
			float localPhi      = TTStubs_coordx->at(l);  //960 strips for PS, 1016 strips for 2S. 2024 values in halfStrip = 11bits = 3bits(chipID)+8bits(stubAddress)
			float stub_trigBend = TTStubs_trigBend->at(l);//-8 to 8, 16 strips, 32 values(5bits) in halfStrip
			unsigned stub_bx;
			if (multiBx) stub_bx= TTStubs_bxId->at(l);    //0-7(3bits) for bx
			else stub_bx = 0;
			std::tr1::array<float,23> simAStub;
			simAStub[0]=jentry;
			simAStub[1]=moduleId;
			simAStub[2]=genParts_charge->at(0);
			simAStub[3]=genParts_pdgId->at(0);
			simAStub[4]=genParts_vx->at(0);
			simAStub[5]=genParts_vy->at(0);
			simAStub[6]=genParts_vz->at(0);
			simAStub[7]=genParts_cotTheta->at(0);
			simAStub[8]=genParts_d0->at(0);
			simAStub[9]=genParts_dz->at(0);
			simAStub[10]=genParts_invPt->at(0);
			simAStub[11]=TTStubs_simPt->at(l);  // ==genParts_pt->at(0);
			simAStub[12]=TTStubs_simPhi->at(l); // ==genParts_phi->at(0); 
			simAStub[13]=TTStubs_simEta->at(l); // ==genParts_eta->at(0);
			simAStub[14]=TTStubs_phi->at(l);
			simAStub[15]=TTStubs_eta->at(l);
			simAStub[16]=TTStubs_r->at(l);
			simAStub[17]=TTStubs_x->at(l);
			simAStub[18]=TTStubs_y->at(l);
			simAStub[19]=TTStubs_z->at(l);
			simAStub[20]=localPhi;
			simAStub[21]=localZ;
			simAStub[22]=stub_trigBend;

			if (requirePtCut3 && !pass3GeVCut(moduleId,stub_trigBend)) {std::cout<<std::endl<<"THIS STUB FAILS PT 3GEC CUT"<<std::endl; continue;}
			if (requireGenPtCut3 && TTStubs_simPt->at(l)<3) {std::cout<<std::endl<<"THIS STUB FAILS GEN PT 3GEC CUT"<<std::endl; continue;}
			if (requireBlinding) {
				bool blindFlag = false;
				for (Long64_t i=0;i<blindEntries;i++) {
					blindTree->GetEntry(i);
					if (moduleId==unsigned(id) && (localPhi<phi1 || localPhi>phi2 || localZ<z1 || localZ>z2)) blindFlag = true;
				}
				if (blindFlag) {std::cout<<std::endl<<"THIS STUB FAILS OVERLAP MASKING CUT"<<std::endl; continue;}
			}


			std::bitset<3>  bxID;        bxID |= stub_bx;
			std::bitset<4>  layIdx;       layIdx |= int(lay-5); // [5,21] --> [0,16]
			std::bitset<11> stubAddress; stubAddress |= int(localPhi*2); //chipID(3bits)+stubAddress(8bits)
			std::bitset<5>  stubBend2S;  stubBend2S |= int(stub_trigBend*2);  // int-->bitset-->ullong: [0,16]-->[00000,10000]-->[0,16]; [-16,-1]-->[10000,11111]-->[16,31]
			std::bitset<5>  stubBendPS;  stubBendPS |= int(stub_trigBend*2);  // if taking 3 bits: [0,3][-4,-1]-->[000,011][100,111]-->[0,3][4,7];  if taking 4 bits: [0,7][-8,-1]-->[0000,0111][1000,1111]-->[0,7][8,15]
			std::bitset<5>  stubZpos5;    //int-->bitset<5> -->bitset<4> -->ullong: [0,15]-->[00000,01111]-->[0000,1111]-->[0,15]; [16,31]-->[10000,11111]-->[0000,1111]-->[0,15]
			std::bitset<4>  stubZpos;
			if (lay>7) stubZpos5 |= int(localZ*16); //2S
			else { //PS
				stubZpos |= int(localZ);
				stubZpos5 |= int(localZ);
			}
			

			/**********************************/
			/*******fill bits for PRBF.2*******/
			/**********************************/

			for(std::vector<unsigned>::iterator it = glbModuleIDs.begin(); it != glbModuleIDs.end(); ++it) {
				if (moduleId == *it) {          //find out if this stub belongs to the selected modules for this trigger tower
					fillOutputFlag=1;            //if there is no stubs belongs to this trigger tower, this value will be still 0, and we do not fill output file
					std::bitset<4>  localLadder;      localLadder |= GlbModule_LocalModule_Map_[moduleId].first;
					std::bitset<5>  localModule;      localModule |= GlbModule_LocalModule_Map_[moduleId].second;
					std::bitset<32> PRBFtmp;
					
					if (halfLayer && ( (lay == 5 && lad == 4) || (lay == 5 && lad == 3 && localPhi < 480) ) ) continue;
					
					//for (unsigned b=0;b<layIdx.size();b++) {
					//PRBFtmp.set(b,  layIdx.test(b));
					//}
					if (lay>7) { //2S modules (layer 8,9,10) 
						for (unsigned b=0;b<stubBend2S.size()-1;b++) {
							PRBFtmp.set(b + 2,  stubBend2S.test(b));                                                            //00000000000000000000000000BBBBLL
						}
					}
					else {//PS modules (layer 5,6,7)
						for (unsigned b=0;b<stubBendPS.size()-1;b++) {
							PRBFtmp.set(b + 2,  stubBendPS.test(b));                                                            //00000000000000000000000000BBBBLL
						}
					}
					for (unsigned b=0;b<stubZpos5.size();b++) {
						PRBFtmp.set(b + PRBFtmp.size() - 1 - localModule.size() - localLadder.size() - stubAddress.size() - stubZpos5.size(),  stubZpos5.test(b)); //000000000000000000000ZZZZZBBBBLL
					}
					for (unsigned b=0;b<stubAddress.size();b++) {
						PRBFtmp.set(b + PRBFtmp.size() - 1 - localModule.size() - localLadder.size() - stubAddress.size(),  stubAddress.test(b)); //0000000000AAASSSSSSSSZZZZZBBBBLL
					}
					for (unsigned b=0;b<localLadder.size();b++) {
						PRBFtmp.set(b + PRBFtmp.size() - 1 - localModule.size() - localLadder.size(),  localLadder.test(b));                      //000000LLLLAAASSSSSSSSZZZZZBBBBLL
					}
					for (unsigned b=0;b<localModule.size();b++) {
						PRBFtmp.set(b + PRBFtmp.size() - 1 - localModule.size(),  localModule.test(b));                                           //0MMMMMLLLLAAASSSSSSSSZZZZZBBBBLL
					}
					for (unsigned b=0;b<1;b++) {
						PRBFtmp.set(b + PRBFtmp.size() - 1, 1);                                                                                   //1MMMMMLLLLAAASSSSSSSSZZZZZBBBBLL
					}
					BitsPRBF2[0][lay].push_back(PRBFtmp);

					std::cout<<std::endl;
					for (int layer=5; layer<11; layer++) {						
						std::cout<<"***** layer "<<layer<<" *****"<<std::endl;
						for (unsigned stub=0; stub<BitsPRBF2[0][layer].size(); stub++) {
							std::cout<<BitsPRBF2[0][layer][stub]<<std::endl;
						}
					}   
				}
			}   



			/**********************************/
			/****fill bits for DataSourcing****/
			/**********************************/

			int nBitsFor2S = bxID.size()+stubAddress.size()+stubBend2S.size();
			int nBitsForPS = bxID.size()+stubAddress.size()+stubBendPS.size()+stubZpos.size();
			for (unsigned board=0; board<BoardModuleMap.size(); board++) { //loop 10 boards in this trigger tower
				for (int m=0; m<40; m++){                              //loop 40 links in this board
					if (moduleId == BoardModuleMap[board][m][0] || moduleId == BoardModuleMap[board][m][1]) { //find out if this stub belongs to the selected modules
						std::cout<<std::endl<<"****************************************"<<std::endl;
						std::cout<<           "*********A NEW STUB IN BOARD: "<<board<<"*********"<<std::endl;
						std::cout<<           "****************************************"<<std::endl;
						//fillOutputFlag=1;            //if there is no stubs belongs to the 40 modules, this value will be still 0, and we do not fill output file

						simInfo[board].push_back(simAStub);

						if (lay>7) { //2S modules (layer 8,9,10)
							if (localZ<1) { //CIC L
								std::cout<<"This is the "<<nStubL[board][m]+1<<"th fired stub in 2S-L module "<<moduleId<<", which has bits:"<<std::endl;
								if (nStubL[board][m]<12) {
									for (unsigned b=0;b<bxID.size();b++) { //write bxID (3bits) for this stub into the output block  XXX0000000000000000
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsFor2S - bxID.size() + b, bxID.test(b));
									}
									std::cout<<"Set bxID: "<<stub_bx<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubAddress.size();b++) { //write chipID+stubAddress (11bits) for this stub into the output block  XXXAAASSSSSSSS00000
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsFor2S - bxID.size() - stubAddress.size() + b, stubAddress.test(b));
									}
									std::cout<<"Set chipID+stubAddress: "<<localPhi<<std::endl<<BitsCICL[board][m]<<std::endl;
									for (unsigned b=0;b<stubBend2S.size();b++) { //write the stubBend bits for this stub into the output block  XXXAAASSSSSSSSBBBBB
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
									for (unsigned b=0;b<stubZpos.size();b++) { //write the stubZposition (4bits) for this stub into the output block  XXXAAASSSSSSSSBBBBBZZZZ
										BitsCICL[board][m].set(BitsCICL[board][m].size() - headerCICL.size() - nStubL[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() - stubZpos.size() + b, stubZpos.test(b));
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
									for (unsigned b=0;b<stubZpos.size();b++) {
										BitsCICR[board][m].set(BitsCICR[board][m].size() - headerCICR.size() - nStubR[board][m]*nBitsForPS - bxID.size() - stubAddress.size() - stubBendPS.size() - stubZpos.size() + b, stubZpos.test(b));
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
		//1. fill the PRBF2 output file
		//2. create DS header, then copy header into the DS output block, then fill the DS output file

		if (SkipEventWithTwoEmptyLayer) {  //if there is 0 stubs in any layer, skip this event. Set SkipEventWithTwoEmptyLayer=0 if runing for PU only simple
			int nEmptyLayer = 0;
			for (int layer=5; layer<11; layer++) {
				if (BitsPRBF2[0][layer].size()==0) nEmptyLayer++;
			}
			if (nEmptyLayer>1) {
				fillOutputFlag=0;
				std::cout<<std::endl<<"THIS EVENT FAILS EMPTY LAYER CUT"<<std::endl;
			}
		}
		if (fillOutputFlag==0) continue;

		/*******************************************/
		/********fill the PRBF.2 output file********/
		/*******************************************/
		int nStubMax = fillPRBF2WithString(BitsPRBF2[0],jentry);
		InputLatency->Fill(nStubMax);

		/*******************************************/
		/*****fill the DataSourcing output file*****/
		/*******************************************/
		for (unsigned board=0; board<BoardModuleMap.size(); board++) {
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
					headerCICL.set(b, NStubL.test(b)); //0000000000000000000000NNNN
					headerCICR.set(b, NStubR.test(b));
				}

				//set bx ID
				std::bitset<12> GlbBxID; //0000000000BBBBBBBBBBBBNNNN

				//set CIC error/status bit
				headerCICL.set(0 + GlbBxID.size() + NStubL.size(), CICStatusL[board][m]); //000000000SBBBBBBBBBBBBNNNN
				headerCICR.set(0 + GlbBxID.size() + NStubR.size(), CICStatusR[board][m]);

				//set CBS/MPA error/status bit //0SSSSSSSSSBBBBBBBBBBBBNNNN

				//set module type
				if (decodeLayer(BoardModuleMap[board][m][0])>7) { //2S module  
					headerCICL.set(headerCICL.size()-1,0); //HSSSSSSSSSBBBBBBBBBBBBNNNN
					headerCICR.set(headerCICL.size()-1,0); 
				}
				else { //PS module
					headerCICL.set(headerCICR.size()-1,1);
					headerCICR.set(headerCICR.size()-1,1);
				}

				//print header
				//std::cout<<"Module "<<BoardModuleMap[board][m][0]<<": HeaderCICL="<<headerCICL<<", HeaderCICR="<<headerCICR<<std::endl;

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
			fillTxtfileWithString(board,BitsCICL[board],BitsCICR[board],BoardModuleMap,jentry);

			/******************************************/
			/**********fill the gen info file**********/
			/******************************************/
			fillGenSimInfo(board,simInfo[board]);
		}
	}

	TCanvas *c1 = new TCanvas("c1","c1"); 
	InputLatency->Draw();
	c1->SaveAs("outputfiles/InputLatency.pdf");
	c1->SaveAs("outputfiles/InputLatency.png");
	outputfile->Write();
}


int fillGenSimInfo(int board, std::vector< std::tr1::array<float, 23> > simABoard) {
	std::ofstream outfileSIM;
	outfileSIM.open("outputfiles/sim.txt",std::ofstream::app);
	for(std::vector<std::tr1::array<float, 23> >::iterator it = simABoard.begin(); it != simABoard.end(); ++it) {
		std::tr1::array<float, 23> simAStub = *it;
		std::cout<<"Board"<<board<<" "
			<<simAStub[0]<<" "<<simAStub[1]<<" "<<simAStub[2]<<" "<<simAStub[3]<<" "<<simAStub[4]<<" "
			<<simAStub[5]<<" "<<simAStub[6]<<" "<<simAStub[7]<<" "<<simAStub[8]<<" "<<simAStub[9]<<" "
			<<simAStub[10]<<" "<<simAStub[11]<<" "<<simAStub[12]<<" "<<simAStub[13]<<" "<<simAStub[14]<<" "
			<<simAStub[15]<<" "<<simAStub[16]<<" "<<simAStub[17]<<" "<<simAStub[18]<<" "<<simAStub[19]<<" "
			<<simAStub[20]<<" "<<simAStub[21]<<" "<<simAStub[22]<<"\n"; 
		outfileSIM<<simAStub[0]<<" Board "<<board<<" moduleId "<<simAStub[1]<<" genCharge "<<simAStub[2]<<" genPdgId "<<simAStub[3]<<"\n"
			<<" genVx "<<simAStub[4]<<" genVy "<<simAStub[5]<<" genVz "<<simAStub[6]<<" genD0 "<<simAStub[8]<<" genDz "<<simAStub[9]<<"\n"
			<<" genCotTheta "<<simAStub[7]<<" genInvPt "<<simAStub[10]<<" genPt "<<simAStub[11]<<" genPhi "<<simAStub[12]<<" genEta "<<simAStub[13]<<"\n"
			<<" stubPhi "<<simAStub[14]<<" stubEta "<<simAStub[15]<<" stubR "<<simAStub[16]<<" stubX "<<simAStub[17]<<" stubY "<<simAStub[18]<<" stubZ "<<simAStub[19]<<"\n"
			<<" stubLocalPhi "<<simAStub[20]<<" stubLocalZ "<<simAStub[21]<<" stubBend "<<simAStub[22]<<"\n"<<"\n";
	}   
	//outfileSIM << "\n";

	outfileSIM.close();
	return 0;
}


int fillPRBF2WithString(std::vector< std::vector< std::bitset<32> > >BitsPRBF2aBx, int jentry) {
	std::ofstream outfile;
	outfile.open("outputfiles/PRBF2.txt",std::ofstream::app);

	int nStubMax=0;
	int nStubLayer[23]={0};
	for (int l=5; l<11; l++) {
		nStubLayer[l] = BitsPRBF2aBx[l].size();
		if (nStubMax<nStubLayer[l]) nStubMax=nStubLayer[l];
	}

	std::bitset<32> AllZero; 

	for (int s=0; s<nStubMax; s++) {
		outfile << jentry << " 0 "; 
		for (int l=5; l<11; l++) {
			if (s<nStubLayer[l]) outfile << BitsPRBF2aBx[l][s].to_string() << " ";
			else outfile << AllZero << " ";
		}
		outfile << "\n";
	}

	//EOE
	outfile << jentry << " 1 " << AllZero << " " << AllZero << " " << AllZero << " " << AllZero << " " << AllZero << " " << AllZero << " \n";
	outfile.close();
	return nStubMax;
}


//Convert bitset to a string. Then save the string into the output txtfile
int fillTxtfileWithString(int board, std::bitset<256> BitsCICLaBoard[40], std::bitset<256> BitsCICRaBoard[40], std::vector< std::vector< std::vector<unsigned> > > BoardModuleMap, int jentry) {
	std::ofstream outfile;
	char str[50];
	sprintf(str,"outputfiles/DataSourcingBoard%02d.txt",board);
	outfile.open(str,std::ofstream::app);
	outfile << "Entry "<<jentry<<"\n";

	for (int m=0; m<40; m++) {
		//outfile << "Module " << BoardModuleMap[board][m][0] << " " << BitsCICLaBoard[m].to_string() << " "<< BitsCICRaBoard[m].to_string() << "\n";
		outfile << "Module " << BoardModuleMap[board][m][0] << "\n";
		for (int j=0; j<(256/32); j++) {
			for (int k=0; k<32; k++) {
				outfile << BitsCICLaBoard[m].test(255-32*j-k);
			}
			outfile << "\n";
		}
		for (int j=0; j<(256/32); j++) {
			for (int k=0; k<32; k++) {
				outfile << BitsCICRaBoard[m].test(255-32*j-k);
			}
			outfile << "\n";
		}
	}
	outfile << "\n";

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


