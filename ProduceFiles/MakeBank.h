//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar 30 10:15:33 2015 by ROOT version 5.34/07
// from TTree tree/
// found on file: /eos/uscms/store/user/l1upgrades/SLHC/GEN/620_SLHC12p1_ntuple/PU140/TTbarTTbar_E2023TTI_PU140_TuneCUEP6S1_ntuple.2.root
//////////////////////////////////////////////////////////

#ifndef MakeBank_h
#define MakeBank_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class MakeBank {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<float>   *TTStubs_phi;
   vector<float>   *TTStubs_r;
   vector<float>   *TTStubs_z;
   vector<unsigned int> *TTStubs_modId;
   vector<float>   *TTStubs_trigBend;
   vector<float>   *TTStubs_coordx;
   vector<float>   *TTStubs_coordy;
   vector<unsigned int> *AMTTRoads_patternRef;
   vector<unsigned int> *AMTTRoads_tower;
   vector<unsigned int> *AMTTRoads_nstubs;
   vector<float>   *AMTTRoads_patternInvPt;
   vector<vector<unsigned int> > *AMTTRoads_superstripIds;
   vector<vector<vector<unsigned int> > > *AMTTRoads_stubRefs;

   // List of branches
   TBranch        *b_TTStubs_phi;   //!  
   TBranch        *b_TTStubs_r;   //!  
   TBranch        *b_TTStubs_z;   //!  
   TBranch        *b_TTStubs_modId;   //!
   TBranch        *b_TTStubs_trigBend;   //!
   TBranch        *b_TTStubs_coordx;   //!
   TBranch        *b_TTStubs_coordy;   //!
   TBranch        *b_AMTTRoads_patternRef;   //!
   TBranch        *b_AMTTRoads_tower;   //!
   TBranch        *b_AMTTRoads_nstubs;   //!
   TBranch        *b_AMTTRoads_patternInvPt;   //!
   TBranch        *b_AMTTRoads_superstripIds;   //!
   TBranch        *b_AMTTRoads_stubRefs;   //!

   MakeBank(TTree *tree=0);
   virtual ~MakeBank();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MakeBank_cxx
MakeBank::MakeBank(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("results.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("results.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("results.root:/ntupler");
      dir->GetObject("tree",tree);

   }
   Init(tree);
}

MakeBank::~MakeBank()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MakeBank::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MakeBank::LoadTree(Long64_t entry)
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

void MakeBank::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TTStubs_phi = 0;
   TTStubs_r = 0;
   TTStubs_z = 0;
   TTStubs_modId = 0;
   TTStubs_trigBend = 0;
   TTStubs_coordx = 0;
   TTStubs_coordy = 0;
   AMTTRoads_patternRef = 0;
   AMTTRoads_tower = 0;
   AMTTRoads_nstubs = 0;
   AMTTRoads_patternInvPt = 0;
   AMTTRoads_superstripIds = 0;
   AMTTRoads_stubRefs = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("TTStubs_phi", &TTStubs_phi, &b_TTStubs_phi);
   fChain->SetBranchAddress("TTStubs_r", &TTStubs_r, &b_TTStubs_r);
   fChain->SetBranchAddress("TTStubs_z", &TTStubs_z, &b_TTStubs_z);
   fChain->SetBranchAddress("TTStubs_modId", &TTStubs_modId, &b_TTStubs_modId);
   fChain->SetBranchAddress("TTStubs_trigBend", &TTStubs_trigBend, &b_TTStubs_trigBend);
   fChain->SetBranchAddress("TTStubs_coordx", &TTStubs_coordx, &b_TTStubs_coordx);
   fChain->SetBranchAddress("TTStubs_coordy", &TTStubs_coordy, &b_TTStubs_coordy);
   fChain->SetBranchAddress("AMTTRoads_patternRef", &AMTTRoads_patternRef, &b_AMTTRoads_patternRef);
   fChain->SetBranchAddress("AMTTRoads_tower", &AMTTRoads_tower, &b_AMTTRoads_tower);
   fChain->SetBranchAddress("AMTTRoads_nstubs", &AMTTRoads_nstubs, &b_AMTTRoads_nstubs);
   fChain->SetBranchAddress("AMTTRoads_patternInvPt", &AMTTRoads_patternInvPt, &b_AMTTRoads_patternInvPt);
   fChain->SetBranchAddress("AMTTRoads_superstripIds", &AMTTRoads_superstripIds, &b_AMTTRoads_superstripIds);
   fChain->SetBranchAddress("AMTTRoads_stubRefs", &AMTTRoads_stubRefs, &b_AMTTRoads_stubRefs);
   Notify();
}

Bool_t MakeBank::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MakeBank::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MakeBank::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MakeBank_cxx
