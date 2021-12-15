//
//	DeltaRayOff.cc
//	Created by  Shih Lin
//	
//	Analysis code for Photon trigger(HLT_Photon200 mainly)
//	root -l -q 'DeltaRayOff.cc("2018","1000")'
//
#include "iostream"
#include "TAttMarker.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TH1F.h"
#include "math.h"
#include <algorithm>
#include <string>
#include "../interface/Candidate.h"
#include "../interface/PlotSet.h"
#include "../interface/MonoCuts.h"
using namespace std;

  void MonoCuts::doAnalysis(vector<MonoCandidate> &cand, vector<Photon> & pho, unsigned nCandidates,unsigned nPhoton, bool TRG, unsigned ev)
  {
	Clear();

     for(unsigned c=0;c<nCandidates;c++){

	MonoCandidate &cands = cand[c];
	bool QualCut = evalQuality(cands);
	bool ECut = evalE(cands);
	bool F51Cut = evalF51(cands);
	bool dEdXCut = evaldEdX(cands);

	//count for total events without TRG	
	if(TRG) CutFlowCand_TRG.push_back(cands);
	//N-1 cut and relative efficiency
        if( ECut && F51Cut && dEdXCut &&TRG) N1CutCand_Qual.push_back(cands); 
        if( QualCut && F51Cut && dEdXCut &TRG) N1CutCand_Energy.push_back(cands);
        if( QualCut && ECut && dEdXCut &&TRG ) N1CutCand_F51.push_back(cands);
	if( QualCut && ECut && F51Cut &&TRG)  N1CutCand_Dedx.push_back(cands);
	if( QualCut && ECut & F51Cut && dEdXCut) N1CutCand_TRG.push_back(cands);
		
	//-----------------------------------------------------------------	
	//---Cutflow histograms--------------------------------------------
	//-----------------------------------------------------------------

	//signal efficiency
	
	if(TRG && QualCut ) CutFlowCand_Qual.push_back(cands); 
	if(TRG && QualCut && ECut ) CutFlowCand_Energy.push_back(cands);
	if(TRG && QualCut && ECut && F51Cut) CutFlowCand_F51.push_back(cands);
	if(TRG && QualCut && ECut && F51Cut && dEdXCut) CutFlowCand_Dedx.push_back(cands);
	
      }//for cand loop

    	
//    if(nPhoton!=0){
//    for(unsigned p=0;p<nPhoton;p++){
//	Photon &photon = pho[p];
//	bool PhotonPtCut = evalPhoton(photon);
//	if(PhotonPtCut) HighPtPhoton.push_back(photon);
//    }
//    }
	//cut flow events calculating
        sort(CutFlowCand_TRG.begin(),CutFlowCand_TRG.begin()+CutFlowCand_TRG.size());
	
	if(CutFlowCand_TRG.size()>0) 
	{
		count++;
		FillNoCutHistogram(0,CutFlowCand_TRG,0);
	}
        sort(CutFlowCand_Qual.begin(),CutFlowCand_Qual.begin()+CutFlowCand_Qual.size());
	if(CutFlowCand_Qual.size()>0) 
	{
		Qual_count++;	
		FillFlowHistogram(0,CutFlowCand_Qual,0);
	}
	
        sort(CutFlowCand_Energy.begin(),CutFlowCand_Energy.begin()+CutFlowCand_Energy.size());
	if(CutFlowCand_Energy.size()>0)
	{
		E_count++;	
		FillFlowHistogram(1,CutFlowCand_Energy,0);
	}
        sort(CutFlowCand_F51.begin(),CutFlowCand_F51.begin()+CutFlowCand_F51.size());
	if(CutFlowCand_F51.size()>0)
	{
		f51_count++;	
		FillFlowHistogram(2,CutFlowCand_F51,0);
	}
	sort(CutFlowCand_Dedx.begin(),CutFlowCand_Dedx.begin()+CutFlowCand_Dedx.size());
	if(CutFlowCand_Dedx.size()>0)
	{
		dEdX_count++;	
		FillFlowHistogram(3,CutFlowCand_Dedx,0);
	}
	///////////////////////////////////////////////////
	/////////  N1 Cut Plots and  Count   //////////////
	///////////////////////////////////////////////////


	//count n_1Plot TRG+Qual and get plots
        sort(N1CutCand_Qual.begin(),N1CutCand_Qual.begin()+N1CutCand_Qual.size());
	if(N1CutCand_Qual.size()>0) 
	{
		FillN1Histogram(0,N1CutCand_Qual);
		NoQual++;
	}
	sort(N1CutCand_Energy.begin(),N1CutCand_Energy.begin()+N1CutCand_Energy.size());
	if(N1CutCand_Energy.size()>0)
	{
		FillN1Histogram(1,N1CutCand_Energy);
		NoE++;	
	}
        sort(N1CutCand_F51.begin(),N1CutCand_F51.begin()+N1CutCand_F51.size());
	if(N1CutCand_F51.size()>0)
	{
		FillN1Histogram(2,N1CutCand_F51);
		NoF51++;	
	}
	sort(N1CutCand_Dedx.begin(),N1CutCand_Dedx.begin()+N1CutCand_Dedx.size());
	if(N1CutCand_Dedx.size()>0)
	{
	
		FillN1Histogram(3,N1CutCand_Dedx);
		NodEdXCut++;	
	}
        sort(N1CutCand_TRG.begin(),N1CutCand_TRG.begin()+N1CutCand_TRG.size());
	if(N1CutCand_TRG.size()>0) 
	{
		FillN1Histogram(4,N1CutCand_TRG);
		NoTRG++;
	}
  }
  void MonoCuts::FillNoCutHistogram(int n,vector<MonoCandidate> Cand, bool matching){
	PlotSet &z = NoCutPlot[n];
	vector<MonoCandidate> Matched;
	if (matching == 1){
		Matched = Matching(Cand);	
		for(int i=0; i < Matched.size() ;i++){
	  	  z.GetPlot(FracSatVNstrips)->Fill(Matched[i].subHits_,Matched[i].subSatHits_/Matched[i].subHits_);
	   	  z.GetPlot(DedXSig)->Fill(Matched[i].dEdXSig_);
	   	  z.GetPlot(RZcurv)->Fill(Matched[i].rzp2_);
           	  z.GetPlot(E55)->Fill(Matched[i].e55_);
	   	  z.GetPlot(F51)->Fill(Matched[i].f51_);
           	  z.GetPlot(HcalIso)->Fill(Matched[i].hIso_);
           	  z.GetPlot(ABCD)->Fill(Matched[i].f51_,Matched[i].dEdXSig_);
		}
	}
	else{
		for(int i=0; i < Cand.size() ;i++){
		  z.GetPlot(FracSatVNstrips)->Fill(Cand[i].subHits_,Cand[i].subSatHits_/Cand[i].subHits_);
		  z.GetPlot(DedXSig)->Fill(Cand[i].dEdXSig_);
		  z.GetPlot(RZcurv)->Fill(Cand[i].rzp2_);
        	  z.GetPlot(E55)->Fill(Cand[i].e55_);
		  z.GetPlot(F51)->Fill(Cand[i].f51_);
        	  z.GetPlot(HcalIso)->Fill(Cand[i].hIso_);
        	  z.GetPlot(ABCD)->Fill(Cand[i].f51_,Cand[i].dEdXSig_);
		}
	}
	Matched.clear();
  }
  void MonoCuts::FillFlowHistogram(int n, vector<MonoCandidate> CutFlowCand, bool matching){
	PlotSet &z = CutFlow[n];
	vector<MonoCandidate> Matched;
	if (matching == 1){
		Matched = Matching(CutFlowCand);	
		for(int i=0; i < Matched.size() ;i++){
	  	  z.GetPlot(FracSatVNstrips)->Fill(Matched[i].subHits_,Matched[i].subSatHits_/Matched[i].subHits_);
	   	  z.GetPlot(DedXSig)->Fill(Matched[i].dEdXSig_);
	   	  z.GetPlot(RZcurv)->Fill(Matched[i].rzp2_);
           	  z.GetPlot(E55)->Fill(Matched[i].e55_);
	   	  z.GetPlot(F51)->Fill(Matched[i].f51_);
           	  z.GetPlot(HcalIso)->Fill(Matched[i].hIso_);
           	  z.GetPlot(ABCD)->Fill(Matched[i].f51_,Matched[i].dEdXSig_);
		}
	}
	else{
		for(int i=0; i < CutFlowCand.size() ;i++){
		  z.GetPlot(FracSatVNstrips)->Fill(CutFlowCand[i].subHits_,CutFlowCand[i].subSatHits_/CutFlowCand[i].subHits_);
		  z.GetPlot(DedXSig)->Fill(CutFlowCand[i].dEdXSig_);
		  z.GetPlot(RZcurv)->Fill(CutFlowCand[i].rzp2_);
        	  z.GetPlot(E55)->Fill(CutFlowCand[i].e55_);
		  z.GetPlot(F51)->Fill(CutFlowCand[i].f51_);
        	  z.GetPlot(HcalIso)->Fill(CutFlowCand[i].hIso_);
        	  z.GetPlot(ABCD)->Fill(CutFlowCand[i].f51_,CutFlowCand[i].dEdXSig_);
		}
	}
	Matched.clear();
  }
  void MonoCuts::FillN1Histogram(int n, vector<MonoCandidate> N1CutCand){
	PlotSet &z = n_1Plot[n];
	for(int i=0; i < N1CutCand.size() ;i++){
	  z.GetPlot(FracSatVNstrips)->Fill(N1CutCand[i].subHits_,N1CutCand[i].subSatHits_/N1CutCand[i].subHits_);
	  z.GetPlot(DedXSig)->Fill(N1CutCand[i].dEdXSig_);
	  z.GetPlot(RZcurv)->Fill(N1CutCand[i].rzp2_);
          z.GetPlot(E55)->Fill(N1CutCand[i].e55_);
	  z.GetPlot(F51)->Fill(N1CutCand[i].f51_);
          z.GetPlot(HcalIso)->Fill(N1CutCand[i].hIso_);
          z.GetPlot(ABCD)->Fill(N1CutCand[i].f51_,N1CutCand[i].dEdXSig_);
	}
  }
  vector<MonoCandidate> MonoCuts::Matching(vector<MonoCandidate> Cand){

	   vector<MonoCandidate> Matched;

	   for(int i=0; i<Cand.size();i++){
		double m_deltaR=0;
		double am_deltaR=0;
		m_deltaR = sqrt(pow(Cand[i].eta_-Cand[0].mono_eta_,2)+
				pow(Cand[i].phi_-Cand[0].mono_phi_,2));
		am_deltaR= sqrt(pow(Cand[i].eta_-Cand[0].amon_eta_,2)+
                                pow(Cand[i].phi_-Cand[0].amon_phi_,2));

		if(m_deltaR<0.15||am_deltaR<0.15){
			Matched.push_back(Cand[i]);		
			Reco++;
			Flag=1;
		}
	   }
	   if(Flag==1)	      MatchedEvent++;
	
	Flag=0;
	return Matched;
  }
  void MonoCuts::Clear(){

	CutFlowCand_TRG.clear();	
	CutFlowCand_Qual.clear();
	CutFlowCand_Energy.clear();
	CutFlowCand_F51.clear();
	CutFlowCand_Dedx.clear();
	N1CutCand_TRG.clear();	
	N1CutCand_Qual.clear();
	N1CutCand_Energy.clear();
	N1CutCand_F51.clear();
	N1CutCand_Dedx.clear();
	HighPtPhoton.clear();
  }

  void MonoCuts::WritePlots(TFile *oFile){
	oFile->cd(trigName_.c_str());
	NoCutPlot[0].WritePlot();
	for(unsigned c=0; c<nCut; c++) n_1Plot[c].WritePlot();

	for(unsigned c=0; c<nCut; c++) CutFlow[c].WritePlot();
	
  }
  void MonoCuts::SignalEff(const string trName, double TotalEvents)
  {
        //signal efficiency = no. of events after all selection cuts/all events
	cout<<trName<<" ================================="<<endl;
        cout<<"        TRG "<<count<<endl;
        cout<<"QualityCuts "<<Qual_count<<endl;
        cout<<"       ECut "<<E_count<<endl;
        cout<<"     F51Cut "<<f51_count<<endl;
        cout<<" dEdXSigCut "<<dEdX_count<<endl;
        cout<<"------------------------------------------"<<endl;
        cout<<"Signal efficiency = "<<(double)dEdX_count/(double)TotalEvents<<endl;
        cout<<endl;
	cout<<"Relative effciency count"<<endl;
        cout<<"------------------------------------------"<<endl;
        cout<<"     No TRG "<<NoTRG<<endl;
        cout<<" No Quality "<<NoQual<<endl;
        cout<<"    No ECut "<< NoE <<endl;
        cout<<"  No F51Cut "<<NoF51<<endl;
        cout<<" No dEdXSig "<<NodEdXCut<<endl;
        cout<<"------------------------------------------"<<endl;
        cout<<"Relative efficiency"<<endl;
        cout<<"------------------------------------------"<<endl;
        cout<<"        TRG "<<(double)dEdX_count/(double)NoTRG<<endl;
        cout<<"QualityCuts "<<(double)dEdX_count/(double)NoQual<<endl;
        cout<<"       ECut "<<(double)dEdX_count/(double)NoE<<endl;
        cout<<"     F51Cut "<<(double)dEdX_count/(double)NoF51<<endl;
        cout<<" dEdXSigCut "<<(double)dEdX_count/(double)NodEdXCut<<endl;
        cout<<endl;
	cout<<"Total reconstructed event "<<MatchedEvent<<endl;
	  
      cout<<endl;
  }

  //Cuts parameters
  const double MonoCuts::xyp0Cut_=0.6;
  const double MonoCuts::xyp2Cut_=1000;
  const double MonoCuts::rzp0Cut_=10;
  const double MonoCuts::rzp1Cut_=999;
  const double MonoCuts::rzp2Cut_=0.005;
  const double MonoCuts::distCut_ = 0.5;
  const double MonoCuts::hIsoCut_= 10;
  const double MonoCuts::dEdXSigCut_ = 9;
  const double MonoCuts::e55Cut_ = 200;
  const double MonoCuts::f51Cut_ = 0.85;
  const double MonoCuts::photonCut_ = 200;

void DeltaRayOff(string year, string mass)
{
	TFile *oFile = new TFile(("output/DeltaRayOff_Analysis_"+year+"_"+mass+"_NoMatched.root").c_str(),"recreate");	
	TFile *fin = new TFile(("/wk_cms2/shihlin0314/CMSSW_8_0_29/src/Systematic/DeltaRayOff/"+year+"/"+mass+"/DeltaRayOff_"+year+"_"+mass+".root").c_str());
        TTree *tree = (TTree*)fin->Get("monopoles");
        Bool_t passHLT_Photon200;
	Bool_t passHLT_Photon175;
	Bool_t passHLT_DoublePhoton70;
        vector<bool> * trigResults = 0;
        vector<string> * trigNames = 0;

        unsigned nCandidates;
	

        vector<double> *subHits=0;
        vector<double> *subSatHits=0;
        vector<double> *dEdXSig=0;
        vector<double> * tIso = 0;
        vector<double> * xyp0 = 0;
        vector<double> * xyp1 = 0;
        vector<double> * xyp2 = 0;
        vector<double> * rzp0 = 0;
        vector<double> * rzp1 = 0;
        vector<double> * rzp2 = 0;

        vector<double> * dist = 0;
        vector<double> * f51 = 0;
        vector<double> * f15 = 0;
        vector<double> * e55 = 0;
        vector<double> * hIso = 0;
        vector<double> * eta = 0;
        vector<double> * phi = 0;
	vector<double> * pho_eta = 0;
	vector<double> * pho_phi = 0;
	vector<double> * pho_pt = 0;
        unsigned nPhoton;

	vector<double> * Cross = 0;
	unsigned event;
	unsigned NPV;
	
        double mono_eta;
        double mono_phi;
        double amon_eta;
        double amon_phi;

	tree->SetBranchAddress("event",&event);
	tree->SetBranchAddress("trigResult",&trigResults);
        tree->SetBranchAddress("trigNames",&trigNames);
        tree->SetBranchAddress("passHLT_Photon200" , &passHLT_Photon200);
	tree->SetBranchAddress("passHLT_Photon175" , &passHLT_Photon175);
        tree->SetBranchAddress("passHLT_DoublePhoton70",&passHLT_DoublePhoton70);
	tree->SetBranchAddress("cand_N",&nCandidates);
        tree->SetBranchAddress("cand_SubHits",&subHits);
        tree->SetBranchAddress("cand_SatSubHits",&subSatHits);
        tree->SetBranchAddress("cand_dEdXSig",&dEdXSig);
        tree->SetBranchAddress("cand_TIso",&tIso);
        tree->SetBranchAddress("cand_f51",&f51);
        tree->SetBranchAddress("cand_f15",&f15);
        tree->SetBranchAddress("cand_SwissCross",&Cross);

        tree->SetBranchAddress("cand_e55",&e55);
        tree->SetBranchAddress("cand_HIso",&hIso);
        tree->SetBranchAddress("cand_XYPar0",&xyp0);
        tree->SetBranchAddress("cand_XYPar1",&xyp1);
        tree->SetBranchAddress("cand_XYPar2",&xyp2);
        tree->SetBranchAddress("cand_RZPar0",&rzp0);
        tree->SetBranchAddress("cand_RZPar1",&rzp1);
        tree->SetBranchAddress("cand_RZPar2",&rzp2);
        tree->SetBranchAddress("cand_eta",&eta);
        tree->SetBranchAddress("cand_phi",&phi);
        tree->SetBranchAddress("cand_dist",&dist);
        tree->SetBranchAddress("mono_eta",&mono_eta);
        tree->SetBranchAddress("mono_phi",&mono_phi);
        tree->SetBranchAddress("amon_eta",&amon_eta);
        tree->SetBranchAddress("amon_phi",&amon_phi);
        tree->SetBranchAddress("pho_N",&nPhoton);
	tree->SetBranchAddress("pho_eta",&pho_eta);
	tree->SetBranchAddress("pho_phi",&pho_phi);
	tree->SetBranchAddress("pho_pt",&pho_pt);


	const unsigned NEvents = tree->GetEntries();
	vector<double> Et;
	
	
	MonoCuts noTrgAnalysis("NOTRG",oFile);
	MonoCuts TrgAnalysis("HLT_Photon200",oFile);

	vector<MonoCandidate> cand(10);	
	vector<Photon> photon(0);

        for(unsigned ev=0; ev<NEvents;ev++){

		if(ev%1000==0)	cout<<ev<<"/"<<NEvents<<endl;
		tree->GetEntry(ev);
		
		if(nCandidates>Et.size()) Et.resize(nCandidates);
		if(nCandidates>cand.size()) cand.resize(nCandidates);

		if(nPhoton>photon.size()) photon.resize(nPhoton);

		for(unsigned i=0;i<nCandidates;i++){
        	
		//	Et[i]= (*e55)[i]/(TMath::CosH(TMath::Abs((*eta)[i])));
		cand[i] = MonoCandidate(
	        (*subHits)[i],
	        (*subSatHits)[i],
	        (*dEdXSig)[i],
	        (*tIso)[i],
	        (*xyp0)[i],
	        (*xyp1)[i],
	        (*xyp2)[i],
	        (*rzp0)[i],
	        (*rzp1)[i],
	        (*rzp2)[i],
	        (*dist)[i],
	        (*f51)[i],
	        (*f15)[i],
		(*Cross)[i],
	        (*e55)[i],
	        (*hIso)[i],
	        (*eta)[i],
	        (*phi)[i],
                mono_eta,
                mono_phi,
                amon_eta,
                amon_phi,
		event,
		NPV	
     		 );
		}
		if(nPhoton!=0){
		for(unsigned j=0;j<nPhoton;j++){
			photon[j] = Photon(
			(*pho_eta)[j],
			(*pho_phi)[j],
			(*pho_pt)[j]
			);
			}
		}
			noTrgAnalysis.doAnalysis(cand,photon,nCandidates,nPhoton,true,ev);		
			if( year == "2016" || year == "2016APV")      TrgAnalysis.doAnalysis(cand,photon,nCandidates,nPhoton,passHLT_Photon175,ev);
			else      TrgAnalysis.doAnalysis(cand,photon,nCandidates,nPhoton,passHLT_Photon200,ev);
	}//for every event loop
	TrgAnalysis.WritePlots(oFile);
	if(year == "2016" || year == "2016APV"){
		TrgAnalysis.SignalEff("HLT_Photon175",NEvents);
		TrgAnalysis.SaveAs_csv(("output/csv_file/DeltaRayOff_"+year+"_"+mass+"_HLT.csv").c_str(),NEvents,mass,"HLT_Photon175");
	}
	else{
		TrgAnalysis.SignalEff("HLT_Photon200",NEvents);
		TrgAnalysis.SaveAs_csv(("output/csv_file/DeltaRayOff_"+year+"_"+mass+"_HLT.csv").c_str(),NEvents,mass,"HLT_Photon200");
	}
	
	noTrgAnalysis.WritePlots(oFile);
	noTrgAnalysis.SignalEff("NOTRG",NEvents);
	noTrgAnalysis.SaveAs_csv(("output/csv_file/DeltaRayOff_"+year+"_"+mass+".csv").c_str(),NEvents,mass,"NoTrg");
	oFile->Close();	
}
