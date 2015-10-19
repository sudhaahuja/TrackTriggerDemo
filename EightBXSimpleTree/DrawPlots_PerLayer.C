void Draw(TCanvas *c, TFile *f, TString Option, Color_t Color, int Style){
	TH1F *h;
	Char_t str[100];
	Char_t strTex[100];
	TLatex Tex;
	Tex.SetNDC();
	for (int layer=10; layer>4; layer--) {
			sprintf(str,"Layer%02d/nStubTower16Layer%02d",layer,layer);
			f->GetObject(str,h);
			c->cd(1+(10-layer));
			//gPad->SetLogy(); // SetLogy in current pad 
			h->SetLineColor(Color);
			h->SetLineStyle(Style);
			h->Draw(Option);
			h->GetXaxis()->SetRangeUser(0,1000);
			sprintf(strTex,"Tower16, Layer%02d",layer);
			Tex.DrawLatex(0.1,0.95,strTex);
	}
}

void Draw2(TCanvas *c, TFile *f, TString Option, Color_t Color, int Style){
	TH1F *h;
	Char_t strTex[100];
	f->GetObject("nSTUB_board01",h);
	h->SetLineColor(Color);
	h->SetLineStyle(Style);
	h->Draw(Option);
}

void DrawPlots_PerLayer(){
	TFile *in1 = new TFile("Histograms_8PU140.root","READ");
	TFile *in2 = new TFile("Histograms_8PU200.root","READ");
	TFile *in3 = new TFile("Histograms_TTbarTTbarPlus8PU140.root","READ");
	TFile *in4 = new TFile("Histograms_TTbarTTbarPlus8PU200.root","READ");
   TFile *in5 = new TFile("Histograms_8PU140_3GeV.root","READ");
   TFile *in6 = new TFile("Histograms_8PU200_3GeV.root","READ");
   TFile *in7 = new TFile("Histograms_TTbarTTbarPlus8PU140_3GeV.root","READ");
   TFile *in8 = new TFile("Histograms_TTbarTTbarPlus8PU200_3GeV.root","READ");
/*	TFile *in9 = new TFile("../Histograms_muon_2GeV.root","READ");
	TFile *in10 = new TFile("../Histograms_muon_3GeV.root","READ");
   TFile *in11 = new TFile("../Histograms_140PU_3GeV_1evt.root","READ");
   TFile *in12 = new TFile("../Histograms_140PUTTBar_3GeV_1evt.root","READ");
   TFile *in13 = new TFile("Histograms_8PU140_2GeV_1evt.root","READ");
	TFile *in14 = new TFile("Histograms_8PU140_3GeV_1evt.root","READ");
   TFile *in15 = new TFile("Histograms_TTbarTTbarPlus8PU140_3GeV_1evt.root","READ");
   TFile *in16 = new TFile("../Histograms_140PUTTBar_2GeV.root","READ");
	TFile *in17 = new TFile("../Histograms_140PUTTBar_3GeV.root","READ");
	TFile *in18 = new TFile("../Histograms_200PUTTBar_2GeV.root","READ");
	TFile *in19 = new TFile("../Histograms_200PUTTBar_3GeV.root","READ");

*/	


	TCanvas *c1 = new TCanvas("c1","c1",900,600);
	c1->Divide(3,2);
	Draw(c1,in1,"",kRed,1);
	Draw(c1,in3,"same",kBlue,1);
	c1->SaveAs("PU140_2GeV.pdf");

   TCanvas *c2 = new TCanvas("c2","c2",900,600);
   c2->Divide(3,2);
   Draw(c2,in2,"",kRed,1);
   Draw(c2,in4,"same",kBlue,1);
   c2->SaveAs("PU200_2GeV.pdf");

   TCanvas *c3 = new TCanvas("c3","c3",900,600);
   c3->Divide(3,2);
   Draw(c3,in5,"",kRed,1);
   Draw(c3,in7,"same",kBlue,1);
   c3->SaveAs("PU140_3GeV.pdf");

   TCanvas *c4 = new TCanvas("c4","c4",900,600);
   c4->Divide(3,2);
   Draw(c4,in6,"",kRed,1);
   Draw(c4,in8,"same",kBlue,1);
   c4->SaveAs("PU200_3GeV.pdf");

   TCanvas *c1_ = new TCanvas("c1_","c1_",600,600);
   Draw2(c1_,in1,"",kRed,1);
   Draw2(c1_,in3,"same",kBlue,1);
   c1_->SaveAs("PU140_2GeV_.pdf");

   TCanvas *c2_ = new TCanvas("c2_","c2_",600,600);
   Draw2(c2_,in2,"",kRed,1);
   Draw2(c2_,in4,"same",kBlue,1);
   c2_->SaveAs("PU200_2GeV_.pdf");

   TCanvas *c3_ = new TCanvas("c3_","c3_",600,600);
   Draw2(c3_,in5,"",kRed,1);
   Draw2(c3_,in7,"same",kBlue,1);
   c3_->SaveAs("PU140_3GeV_.pdf");

   TCanvas *c4_ = new TCanvas("c4_","c4_",600,600);
   Draw2(c4_,in6,"",kRed,1);
   Draw2(c4_,in8,"same",kBlue,1);
   c4_->SaveAs("PU200_3GeV_.pdf");


/*
   TCanvas *c5 = new TCanvas("c5","c5",900,600);
   c5->Divide(3,2);
   Draw(c5,in9,"",kRed,1);
   Draw(c5,in10,"same",kBlue,2);
   c5->SaveAs("Muon_2and3GeV.pdf");

   TCanvas *c6 = new TCanvas("c6","c6",900,600);
   c6->Divide(3,2);
   Draw(c6,in11,"",kRed,1);
   Draw(c6,in12,"same",kBlue,2);
   c6->SaveAs("140PU1evt.pdf");

   TCanvas *c7 = new TCanvas("c7","c7",900,600);
   c7->Divide(3,2);
   Draw(c7,in13,"",kRed,1);
   Draw(c7,in14,"same",kBlue,2);
   c7->SaveAs("8PU140_2and3GeV_1evt.pdf");

   TCanvas *c8 = new TCanvas("c8","c8",900,600);
   c8->Divide(3,2);
   Draw(c8,in17,"",kRed,1);
	Draw(c8,in16,"same",kBlue,2);
   c8->SaveAs("PU140_2and3GeV.pdf");

   TCanvas *c10 = new TCanvas("c10","c10",900,600);
   c10->Divide(3,2);
   Draw(c10,in19,"",kRed,1);
	Draw(c10,in18,"same",kBlue,2);
   c10->SaveAs("PU200_2and3GeV.pdf");
*/

}
