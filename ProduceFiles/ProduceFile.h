//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jan 11 17:29:12 2015 by ROOT version 5.34/07
// from TTree tree/
// found on file: /eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root
//////////////////////////////////////////////////////////

#ifndef ProduceFile_h
#define ProduceFile_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>

#include "utils.C"

// Fixed size dimensions of array or collections stored in the TTree if any.

class ProduceFile {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<float>   *TTStubs_coordx;
   vector<float>   *TTStubs_coordy;
   vector<float>   *TTStubs_trigBend;
   vector<unsigned int> *TTStubs_bxId;
   vector<unsigned int> *TTStubs_modId;
   vector<float>   *TTStubs_simEta;
   vector<float>   *TTStubs_simPhi;
   vector<float>   *TTStubs_simPt;
   vector<float>   *TTStubs_eta;
   vector<float>   *TTStubs_phi;
   vector<float>   *TTStubs_r;
   vector<float>   *TTStubs_x;
   vector<float>   *TTStubs_y;
   vector<float>   *TTStubs_z;
   vector<int>     *genParts_charge;
   vector<int>     *genParts_pdgId;
   vector<float>   *genParts_eta;
   vector<float>   *genParts_phi;
   vector<float>   *genParts_pt;
   vector<float>   *genParts_px;
   vector<float>   *genParts_py;
   vector<float>   *genParts_pz;
   vector<float>   *genParts_vx;
   vector<float>   *genParts_vy;
   vector<float>   *genParts_vz;
   vector<float>   *genParts_cotTheta;
   vector<float>   *genParts_d0;
   vector<float>   *genParts_dz;
   vector<float>   *genParts_invPt;


   // List of branches
   TBranch        *b_TTStubs_coordx;   //!
   TBranch        *b_TTStubs_coordy;   //!
   TBranch        *b_TTStubs_trigBend;   //!
   TBranch        *b_TTStubs_bxId;   //!
   TBranch        *b_TTStubs_modId;   //!
   TBranch        *b_TTStubs_simEta;   //!
   TBranch        *b_TTStubs_simPhi;   //!
   TBranch        *b_TTStubs_simPt;   //!
   TBranch        *b_TTStubs_eta;   //!
   TBranch        *b_TTStubs_phi;   //!
   TBranch        *b_TTStubs_r;   //!
   TBranch        *b_TTStubs_x;   //!
   TBranch        *b_TTStubs_y;   //!
   TBranch        *b_TTStubs_z;   //!
   TBranch        *b_genParts_charge;   //!
   TBranch        *b_genParts_pdgId;   //!
   TBranch        *b_genParts_eta;   //!
   TBranch        *b_genParts_phi;   //!
   TBranch        *b_genParts_pt;   //!
   TBranch        *b_genParts_px;   //!
   TBranch        *b_genParts_py;   //!
   TBranch        *b_genParts_pz;   //!
   TBranch        *b_genParts_vx;   //!
   TBranch        *b_genParts_vy;   //!
   TBranch        *b_genParts_vz;   //!
   TBranch        *b_genParts_cotTheta;   //!
   TBranch        *b_genParts_d0;   //!
   TBranch        *b_genParts_dz;   //!
   TBranch        *b_genParts_invPt;   //!

   ProduceFile(TTree *tree=0);
   virtual ~ProduceFile();
   //virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ProduceFile_cxx
ProduceFile::ProduceFile(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root:/ntupler");
      dir->GetObject("tree",tree);

   }
   Init(tree);
}

ProduceFile::~ProduceFile()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ProduceFile::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ProduceFile::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ProduceFile::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TTStubs_coordx = 0;
   TTStubs_coordy = 0;
   TTStubs_trigBend = 0;
   TTStubs_bxId = 0;
   TTStubs_modId = 0;
   TTStubs_simEta = 0;
   TTStubs_simPhi = 0;
   TTStubs_simPt = 0;
   TTStubs_eta = 0;
   TTStubs_phi = 0;
   TTStubs_r = 0;
   TTStubs_x = 0;
   TTStubs_y = 0;
   TTStubs_z = 0;
   genParts_charge = 0;
   genParts_pdgId = 0;
   genParts_eta = 0;
   genParts_phi = 0;
   genParts_pt = 0;
   genParts_px = 0;
   genParts_py = 0;
   genParts_pz = 0;
   genParts_vx = 0;
   genParts_vy = 0;
   genParts_vz = 0;
   genParts_cotTheta = 0;
   genParts_d0 = 0;
   genParts_dz = 0;
   genParts_invPt = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("TTStubs_coordx", &TTStubs_coordx, &b_TTStubs_coordx);
   fChain->SetBranchAddress("TTStubs_coordy", &TTStubs_coordy, &b_TTStubs_coordy);
   fChain->SetBranchAddress("TTStubs_trigBend", &TTStubs_trigBend, &b_TTStubs_trigBend);
   if (multiBx) fChain->SetBranchAddress("TTStubs_bxId", &TTStubs_bxId, &b_TTStubs_bxId);
   fChain->SetBranchAddress("TTStubs_modId", &TTStubs_modId, &b_TTStubs_modId);
   fChain->SetBranchAddress("TTStubs_simEta", &TTStubs_simEta, &b_TTStubs_simEta);
   fChain->SetBranchAddress("TTStubs_simPhi", &TTStubs_simPhi, &b_TTStubs_simPhi);
   fChain->SetBranchAddress("TTStubs_simPt", &TTStubs_simPt, &b_TTStubs_simPt);
   fChain->SetBranchAddress("TTStubs_eta", &TTStubs_eta, &b_TTStubs_eta);
   fChain->SetBranchAddress("TTStubs_phi", &TTStubs_phi, &b_TTStubs_phi);
   fChain->SetBranchAddress("TTStubs_r", &TTStubs_r, &b_TTStubs_r);
   fChain->SetBranchAddress("TTStubs_x", &TTStubs_x, &b_TTStubs_x);
   fChain->SetBranchAddress("TTStubs_y", &TTStubs_y, &b_TTStubs_y);
   fChain->SetBranchAddress("TTStubs_z", &TTStubs_z, &b_TTStubs_z);
   fChain->SetBranchAddress("genParts_charge", &genParts_charge, &b_genParts_charge);
   fChain->SetBranchAddress("genParts_pdgId", &genParts_pdgId, &b_genParts_pdgId);
   fChain->SetBranchAddress("genParts_eta", &genParts_eta, &b_genParts_eta);
   fChain->SetBranchAddress("genParts_phi", &genParts_phi, &b_genParts_phi);
   fChain->SetBranchAddress("genParts_pt", &genParts_pt, &b_genParts_pt);
   fChain->SetBranchAddress("genParts_px", &genParts_px, &b_genParts_px);
   fChain->SetBranchAddress("genParts_py", &genParts_py, &b_genParts_py);
   fChain->SetBranchAddress("genParts_pz", &genParts_pz, &b_genParts_pz);
   fChain->SetBranchAddress("genParts_vx", &genParts_vx, &b_genParts_vx);
   fChain->SetBranchAddress("genParts_vy", &genParts_vy, &b_genParts_vy);
   fChain->SetBranchAddress("genParts_vz", &genParts_vz, &b_genParts_vz);
   fChain->SetBranchAddress("genParts_cotTheta", &genParts_cotTheta, &b_genParts_cotTheta);
   fChain->SetBranchAddress("genParts_d0", &genParts_d0, &b_genParts_d0);
   fChain->SetBranchAddress("genParts_dz", &genParts_dz, &b_genParts_dz);
   fChain->SetBranchAddress("genParts_invPt", &genParts_invPt, &b_genParts_invPt);

   Notify();
}

Bool_t ProduceFile::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ProduceFile::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
/*Int_t ProduceFile::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}*/

#endif // #ifdef ProduceFile_cxx
