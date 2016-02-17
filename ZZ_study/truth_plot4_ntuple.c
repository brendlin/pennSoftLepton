#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <TROOT.h>
#include "TF1.h"
#include <TSystem.h>
#include <TFile.h>
#include <TParameter.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <stdlib.h>
#include "TStyle.h"
#include <TGraph.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TLorentzVector.h>
#include <TLegend.h>

using namespace std;

int truth_plot4_ntuple()
{
    gROOT->Reset();
    TCut new_weight,new_weight_v2;
    TCut new_reco_w;
    new_weight = "(nBaselineLeptons < 4) && (nSignalLeptons == 3) &&(PassEvent_eventcleaning) && (HLT_e24_lhmedium_L1EM18VH || HLT_e24_lhmedium_L1EM20VH || HLT_e60_lhmedium|| HLT_e120_lhloose || HLT_mu20_iloose_L1MU15 ||HLT_mu50) && (PassTriggerEle || PassTriggerMu) && (SFOS == 1) && (81.188 < Mll) && (Mll < 101.188) && (WlepPassWSelection > 0)&&(MTW>30.)";
    
    //TFile *f_reco = new TFile("~/Desktop/plot/mc/zz.root");
    //TFile *f_reco = new TFile("zz_reco.root");
    TFile *f_reco = new TFile("/disk/userdata00/atlas_data2/eress/run/test5/fetch/data-SKIM/zz.root");
    //TFile *f_reco = new TFile("/disk/userdata00/atlas_data2/eress/run/test3/data-SKIM/zz_361603.root");
    TFile *f_truth_new = new TFile("zz_truth.root");
    
    TTree *t_reco = (TTree*)f_reco->Get("physics");
    //TFile *f_truth = new TFile("~/Desktop/plot/mc/zz_361603.root");
    // TTree *t_truth = (TTree*)f_truth->Get("physics");
    //TFile *f_truth_new = new TFile("~/Desktop/plot/mc/zz_361603_mb.root");
   //TFile *f_truth_new = new TFile("zz_361603_truth.root");
    TTree *t_truth = (TTree*)f_truth_new->Get("physics");
    
    
    cout<< t_truth->GetEntries()<<endl;
    cout<<t_reco->GetEntries()<<endl;
    //    cout<<t_truth_new->GetEntries()<<endl;
    
    double total_baseline = t_reco->GetEntries(new_weight);
    cout<<"total baseline :"<< total_baseline<<endl;
    //new_weight = "PassEvent_SMWZ3L && SFOS && WlepPassWSelection && (81.188 < Mll) && (Mll < 101.188)&&(MTW>30.)";
    new_weight = "PassEvent_SMWZsig";
    cout<<"total baseline :"<< t_reco->GetEntries(new_weight) << endl;
    //cout <<t_truth->GetEntries(new_reco_w ) << " " << t_truth_new->GetEntries(new_reco_w ) << endl;
    
    
    //cut variables
    float TotalWeightNoSF,MTW,Mll;
    int nSignalLeptons,nBaselineLeptons, HLT_mu20_iloose_L1MU15, HLT_e60_lhmedium, HLT_e24_lhmedium_L1EM18VH, HLT_mu50,HLT_e120_lhloose, HLT_e24_lhmedium_L1EM20VH;
    int truth_nSignalLeptons,SFOS;
    int PassEvent_eventcleaning, PassEvent_eee, PassEvent_eem, PassEvent_emm,PassEvent_mmm, PassEvent_SMWZ3L;
    int ChanFlavor;
    ULong64_t reco_eventNo, truth_eventNo;
    int lep0_index,lep1_index, lep2_index, truth_lepElectronID;
    int lepleadZ_index, lepsubleadZ_index, lepW_index;
    
    int WlepPassWSelection;
    t_reco->SetBranchAddress("SFOS",&SFOS);
    t_reco->SetBranchAddress("lepleadZ_index",&lepleadZ_index);
    t_reco->SetBranchAddress("lepsubleadZ_index",&lepsubleadZ_index);
    t_reco->SetBranchAddress("lepW_index",&lepW_index);
    t_reco->SetBranchAddress("EventNumber",&reco_eventNo);
    t_truth->SetBranchAddress("EventNumber",&truth_eventNo);
    t_reco->SetBranchAddress("ChanFlavor",&ChanFlavor);
    
    t_reco->SetBranchAddress("PassEvent_eventcleaning",&PassEvent_eventcleaning);
    t_reco->SetBranchAddress("PassEvent_eee",&PassEvent_eee);
    t_reco->SetBranchAddress("PassEvent_eem",&PassEvent_eem);
    t_reco->SetBranchAddress("PassEvent_emm",&PassEvent_emm);
    t_reco->SetBranchAddress("PassEvent_mmm",&PassEvent_mmm);
    t_reco->SetBranchAddress("PassEvent_SMWZ3L",&PassEvent_SMWZ3L);
    t_reco->SetBranchAddress("TotalWeightNoSF",&TotalWeightNoSF);
    t_reco->SetBranchAddress("lep0_index",&lep0_index);
    t_reco->SetBranchAddress("lep1_index",&lep1_index);
    t_reco->SetBranchAddress("lep2_index",&lep2_index);
    t_reco->SetBranchAddress("HLT_mu20_iloose_L1MU15",&HLT_mu20_iloose_L1MU15);
    t_reco->SetBranchAddress("HLT_mu50",&HLT_mu50);
    t_reco->SetBranchAddress("HLT_e24_lhmedium_L1EM18VH",&HLT_e24_lhmedium_L1EM18VH);
    t_reco->SetBranchAddress("HLT_e24_lhmedium_L1EM20VH",&HLT_e24_lhmedium_L1EM20VH);
    t_reco->SetBranchAddress("HLT_e120_lhloose",&HLT_e120_lhloose);
    t_reco->SetBranchAddress("HLT_e60_lhmedium",&HLT_e60_lhmedium);
    t_reco->SetBranchAddress("MTW",&MTW);
    t_reco->SetBranchAddress("Mll",&Mll);
    t_reco->SetBranchAddress("nSignalLeptons", &nSignalLeptons);
    t_reco->SetBranchAddress("nBaselineLeptons", &nBaselineLeptons);
    t_reco->SetBranchAddress("WlepPassWSelection", &WlepPassWSelection);
    //t_reco->SetBranchAddress("HLT_e120_lhloose",&HLT_e120_lhloose);
    
    t_truth->AddFriend(t_reco,"t_reco");
    
    //analysis variables
    std::vector<float>* lepSFRecoIdIso;
    std::vector<float>* reco_lepEta;
    std::vector<float>* reco_lepPhi;
    std::vector<float>* reco_lepPt;
    std::vector<int>* reco_lepCharge;
    std::vector<int>* reco_lepFlavor;
    std::vector<float>* truth_lepEta;
    std::vector<float>* truth_lepPhi;
    std::vector<float>* truth_lepPt;
    std::vector<int>* truth_lepCharge;
    std::vector<float>* truth_lepFlavor;
    
    lepSFRecoIdIso=0;
    reco_lepEta=0;
    reco_lepPhi=0;
    reco_lepPt=0;
    reco_lepCharge=0;
    reco_lepFlavor=0;
    truth_lepEta=0;
    truth_lepPhi=0;
    truth_lepPt=0;
    truth_lepCharge=0;
    truth_lepFlavor=0;
    
    TBranch *lepEta;
    TBranch *lepPhi;
    TBranch *lepPt;
    TBranch *lepCharge;
    TBranch *lepFlavor;
    
    t_reco->SetBranchAddress("lepSFRecoIdIso", &lepSFRecoIdIso);
    t_reco->SetBranchAddress("lepPt",&reco_lepPt);
    t_reco->SetBranchAddress("lepEta",&reco_lepEta);
    t_reco->SetBranchAddress("lepPhi",&reco_lepPhi);
    t_reco->SetBranchAddress("lepFlavor",&reco_lepFlavor);
    t_reco->SetBranchAddress("lepCharge",&reco_lepCharge);
    
    t_truth->SetBranchAddress("lepPt",&truth_lepPt);
    t_truth->SetBranchAddress("lepEta",&truth_lepEta);
    t_truth->SetBranchAddress("lepPhi",&truth_lepPhi);
    t_truth->SetBranchAddress("lepFlavor",&truth_lepFlavor);
    t_truth->SetBranchAddress("lepCharge",&truth_lepCharge);
    t_truth->SetBranchAddress("nSignalLeptons", &truth_nSignalLeptons);
    //t_truth->SetBranchAddress("lepElectronID", &truth_lepElectronID);
    
    std::vector<int>* truthLepStatus;
    std::vector<int>* truthLepBarcode;
    std::vector<int>* truthLepOrigin;
    std::vector<int>* truthLepType;
    std::vector<int>* truthLepOutCome;
    std::vector<int>* truthLepMotherID;
    
    truthLepStatus = 0;
    truthLepBarcode = 0;
    truthLepOrigin = 0;
    truthLepType = 0;
    truthLepOutCome = 0;
    truthLepMotherID = 0;
    
    t_truth->SetBranchAddress("truthLepStatus",&truthLepStatus);
    t_truth->SetBranchAddress("truthLepBarcode",&truthLepBarcode);
    t_truth->SetBranchAddress("truthLepOrigin",&truthLepOrigin);
    t_truth->SetBranchAddress("truthLepType",&truthLepType);
    t_truth->SetBranchAddress("truthLepOutCome",&truthLepOutCome);
    t_truth->SetBranchAddress("truthLepMotherID",&truthLepMotherID);
    
    
    gROOT->cd(0);
    //output histograms
    TH1F *deltaR= new TH1F("DeltaR","DeltaR",50,0,0.05);
    TH1F *Pt= new TH1F("Pt","Pt",50,0,200);
    TH1F *eta= new TH1F("Eta","Eta",100,-3.14,3.14);
    TH1F *phi= new TH1F("Phi","Phi",100,-3.14,3.14);
    
    TH1F *eee_leptonChannel= new TH1F("eee_leptonChannel","eee_leptonChannel",50,0,200);
    TH1F *eem_leptonChannel= new TH1F("eem_leptonChannel","eem_leptonChannel",50,0,200);
    TH1F *emm_leptonChannel= new TH1F("emm_leptonChannel","emm_leptonChannel",50,0,200);
    TH1F *mmm_leptonChannel= new TH1F("mmm_leptonChannel","mmm_leptonChannel",50,0,200);
    
    TH1F *Pt_outside= new TH1F("Pt_outside","P_{t}",50,0,200);
    TH1F *eta_outside= new TH1F("Eta_outside","\eta",100,-5,5);
    TH1F *Pt_mu_outside= new TH1F("Pt_mu_outside","P_{t}",50,0,200);
    TH1F *eta_mu_outside= new TH1F("Eta_mu_outside","\eta",100,-5,5);
    TH1F *Pt_ele_outside= new TH1F("Pt_ele_outside","P_{t}",50,0,200);
    TH1F *eta_ele_outside= new TH1F("Eta_ele_outside","\eta",100,-5,5);
    
    TH1F *Pt_muon= new TH1F("Pt_muon","Pt_muon",50,0,200);
   // TNtuple Pt_muon("Pt_muon", "", "Pt_m:weight");
    TH1F *eta_muon= new TH1F("Eta_muon","Eta_muon",100,-5,5);
    TH1F *Pt_electron= new TH1F("Pt_electron","Pt_electron",50,0,200);
    TH1F *Pt_ele_noptcut_out= new TH1F("Pt_electron","Pt_electron",50,0,200);
    TH1F *Pt_ele_noptcut= new TH1F("Pt_electron","Pt_electron",50,0,200);

    TH1F *Pt_mu_noptcut_out= new TH1F("Pt_muon","Pt_muon",50,0,200);
    TH1F *Pt_mu_noptcut= new TH1F("Pt_muon","Pt_muon",50,0,200);

    TH1F *eta_electron= new TH1F("Eta_electron","Eta_electron",100,-5,5);
    TH1F *Pt_new= new TH1F("Pt_new","Pt_new",50,0,200);
    TH1F *eta_new= new TH1F("Eta_new","Eta_new",100,-5,5);
    TH1F *electron_id= new TH1F("Electron_id","Electron_id",5,0,5);
    TH1F *mother_id= new TH1F("lepOrigin","lepOrigin",50,0,50);
    TH1F *lep_outcome= new TH1F("lepOutCome","lepOutCome",50,0,50);
    TH1F *lep_type= new TH1F("lepType","lepType",20,0,20);
    TH1F *zcount= new TH1F("zcount","zcount",20,0,20);
    TH1F *truthdR= new TH1F("truthDR","truthDR",100,0,20);
    TH1F *mll_12= new TH1F("Mll_12","Mll_12",50,0,200);
    TH1F *mll_34= new TH1F("Mll_34","Mll_34",50,0,200);
    TH1F *chanflav= new TH1F("chanflav","chanflav",20,0,20);

    TH1F *Pt_eee= new TH1F("Pt_eee","P_{t}",50,0,200);
    TH1F *Pt_eem= new TH1F("Pt_eem","P_{t}",50,0,200);
    TH1F *Pt_emm= new TH1F("Pt_emm","P_{t}",50,0,200);
    TH1F *Pt_mmm= new TH1F("Pt_mmm","P_{t}",50,0,200);
    
    
    int f_flavor;
    int f_charge;
    int f_mother;
    int f_origin;
    
    gStyle->SetOptStat(0);
    //gStyle->SetOptStat(111111);
    gStyle->SetLegendFont(20);

    
    int match[8];
    int new_match[4]={0,0,0,0};
    int arr_index[4]={-1,-1,-1};
    
    double delta_R;
    double tmp_delta_R, match_delta_R;
    int match_i;
    double total_events=0;
    double total_zevents=0;
    double num_four=0;
    double fraction=0;
    double fraction_ele=0;
    double fraction_mu=0;
    double fraction_in=0;
    double fraction_ele_in=0;
    double fraction_mu_in=0;
    double weight;
    int count;
    int num_entries=0;
    int max_entries = t_reco->GetEntries();
    int max_entries_aftercut =0;
    int num_entries_pass=0;
    int check;
    int z_count=0;
    int index_four[10]={1000,1000,1000,1000,10000,1000,1000,1000,1000,1000};
    int index;
    int index_match;
    int total_plot=0;
    int num_ele=0;
    int num_muon=0;
    int fail =0;
    int nomatch=0;
    int count_four=0;
    int count_gt4=0;
    double def_pt;
    int def_index;
    int index_fourth;
    
    int num_muons,num_electrons;
    
    TLorentzVector lep1,lep2,lep3,lep4, lepThird;
    double particle1_mass, particle2_mass, particle3_mass, particle4_mass, particlethird_mass, m12, m34;
    double tmp_mll, delta_mll;
    
    ofstream recodata;
    recodata.open ("recodata.txt");
    ofstream tdata;
    tdata.open ("truthdata.txt");
    ofstream idata;
    idata.open ("indexdata.txt");
    ofstream fdata;
    fdata.open ("faildata.txt");
    ofstream odata;
    odata.open ("fourthdata.txt");
    ofstream fidata;
    fidata.open ("fourthindex.txt");
    ofstream pdata;
    pdata.open ("plotdata.txt");
    ofstream gdata;
    gdata.open ("gt4data.txt");

    TFile *outputNtuple = new TFile("unIDtruthZZleptons.root","RECREATE");
    TTree *outputTree = new TTree("truth","unIDtruthZZleptons");

    float truth_lep_pt = -999.;
    float truth_lep_eta = -999.;
    int truth_lep_id = -999;

    outputTree->Branch("truth_lep_pt",&truth_lep_pt,"truth_lep_pt/F");
    outputTree->Branch("truth_lep_eta",&truth_lep_eta,"truth_lep_eta/F");
    outputTree->Branch("truth_lep_id",&truth_lep_id,"truth_lep_id/I");
    
    int lepthird_index =-1;
    
    for (int i=0;i <max_entries; i++)
    {
      arr_index[0]=-1;
      arr_index[1]=-1;
      arr_index[2]=-1;
      // cout << 1 << endl;
       num_muons=0;
        num_electrons=0;
        t_reco->GetEntry(i);
        t_truth->GetEntry(i);
        if (truth_eventNo == reco_eventNo) nomatch++;

        check=0;
        new_match[0]=0;
        new_match[1]=0;
        new_match[2]=0;
        z_count=0;
       
        count_four=0;
	index_fourth = -1;
	lepthird_index = -1;
        //if (((lep0_index>=0)&&(reco_lepPt->at(lep0_index) > 25)&&(HLT_e24_lhmedium_L1EM20VH || HLT_e120_lhloose||HLT_mu20_iloose_L1MU15==1||HLT_mu50==1||HLT_e24_lhmedium_L1EM18VH==1||HLT_e60_lhmedium==1)&&(MTW>30)&&(nSignalLeptons == 3)&&(nBaselineLeptons<4)&&(81.188 < Mll && Mll < 101.188)&&(MTW>30)&&(WlepPassWSelection==1)&&(PassEvent_eventcleaning==1)))
        if (PassEvent_SMWZ3L && SFOS && WlepPassWSelection && (81.188 < Mll) && (Mll < 101.188)&&(MTW>30.))
        {
	   cout << "Event Number" << reco_eventNo << endl;
	  //  cout << 2 << endl;
            weight =TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index);
            total_events++;

            //chanflav->Fill(ChanFlavor,weight);
            
	    if (lepleadZ_index <0 || lepsubleadZ_index <0 || lepW_index <0) continue;
            /*for (int j=0; j<truth_nSignalLeptons; j++)
            {
                if(truthLepOrigin->at(j) == 13)
                {
                    z_count++;
                }
            }
            if (z_count<4) continue;
            
            total_zevents++;*/

            tdata << ".. event: " << i << endl;
            
            idata << ".. event: " << i << endl;
            recodata << ".. event: " << i << endl;
            cout <<"P0 " <<reco_lepFlavor->at(lepleadZ_index) << ',' << reco_lepCharge->at(lepleadZ_index) << ',' <<reco_lepPhi->at(lepleadZ_index) << ',' << reco_lepEta->at(lepleadZ_index) << ','<< reco_lepPt->at(lepleadZ_index)<<"\n";
            cout <<"P1 "<<reco_lepFlavor->at(lepsubleadZ_index) << ',' << reco_lepCharge->at(lepsubleadZ_index) << ',' <<reco_lepPhi->at(lepsubleadZ_index) << ',' << reco_lepEta->at(lepsubleadZ_index) << ','<< reco_lepPt->at(lepsubleadZ_index)<< "\n";
            cout <<"P2 " <<reco_lepFlavor->at(lepW_index) << ',' << reco_lepCharge->at(lepW_index) << ',' <<reco_lepPhi->at(lepW_index) << ',' << reco_lepEta->at(lepW_index) << ','<< reco_lepPt->at(lepW_index)<<"\n";
            
            for (int z=0; z<truth_nSignalLeptons;z++) //loop over truth leptons
            {
	      //  cout << 3 << endl;
	      //cout << "z " << z << " " << lepleadZ_index << " " << lepsubleadZ_index << " " << lepW_index << endl;
                cout <<truth_lepFlavor->at(z) << ',' << truth_lepCharge->at(z) << ',' <<truth_lepPhi->at(z) << ',' << truth_lepEta->at(z) <<','<<truthLepOrigin->at(z) << ','<< truthLepMotherID->at(z) << ','<< truth_lepPt->at(z)<< "\n";
                  //  cout <<"in loop" << endl;
                delta_R=1;
                match_i=10;
                
                if (truth_lepFlavor->at(z)==1) num_electrons++;
                if (truth_lepFlavor->at(z)==2) num_muons++;

                //if((new_match[0] < 1000)&&(truth_lepFlavor->at(z) == reco_lepFlavor->at(lepleadZ_index))&&(truth_lepCharge->at(z) == reco_lepCharge->at(lepleadZ_index)))
		if ((truth_lepFlavor->at(z) == reco_lepFlavor->at(lepleadZ_index)))
                {
                    tmp_delta_R = sqrt  (pow((truth_lepPhi->at(z)-reco_lepPhi->at(lepleadZ_index)),2)+pow((truth_lepEta->at(z)-reco_lepEta->at(lepleadZ_index)),2));
		    //cout << "0 " << z << " " << tmp_delta_R<< endl;
			   if (arr_index[0] > -1){
			     match_delta_R = sqrt  (pow((truth_lepPhi->at(arr_index[0])-reco_lepPhi->at(lepleadZ_index)),2)+pow((truth_lepEta->at(arr_index[0])-reco_lepEta->at(lepleadZ_index)),2));}
			   else match_delta_R = 5;
			   if ((tmp_delta_R<=delta_R) && (tmp_delta_R < match_delta_R))
                    {
                        match_i=0;
                        delta_R=tmp_delta_R;
                        //  cout << "1 " << z << " " << delta_R<< endl;
                    }
                }
                    
                //if((new_match[1] < 1000)&&(truth_lepFlavor->at(z) == reco_lepFlavor->at(lepsubleadZ_index))&&(truth_lepCharge->at(z) == reco_lepCharge->at(lepsubleadZ_index)))
		if((truth_lepFlavor->at(z) == reco_lepFlavor->at(lepsubleadZ_index)))
                {
                   // cout << "2" << endl;
                        
                    tmp_delta_R = sqrt(pow((truth_lepPhi->at(z)-reco_lepPhi->at(lepsubleadZ_index)),2)+pow((truth_lepEta->at(z)-reco_lepEta->at(lepsubleadZ_index)),2));
		    //cout << "1 " << z << " " << tmp_delta_R<< endl;
                    if (arr_index[1] > -1){
		      match_delta_R = sqrt  (pow((truth_lepPhi->at(arr_index[1])-reco_lepPhi->at(lepsubleadZ_index)),2)+pow((truth_lepEta->at(arr_index[1])-reco_lepEta->at(lepsubleadZ_index)),2));}
		    else match_delta_R =5;
                    if ((tmp_delta_R<=delta_R) && (tmp_delta_R < match_delta_R))
                    {
                        match_i=1;
                        delta_R=tmp_delta_R;
                        //  cout << "1 " << z << " " << delta_R<< endl;
                            
                    }
                }
                //cout <<reco_lepFlavor->at(2)<< endl;
                //if((new_match[2] < 1000)&&(truth_lepFlavor->at(z) == reco_lepFlavor->at(lepW_index))&&(truth_lepCharge->at(z) == reco_lepCharge->at(lepW_index)))
                if((truth_lepFlavor->at(z) == reco_lepFlavor->at(lepW_index)))
		{
                   //  cout << "3" << endl;
                        
                    tmp_delta_R = sqrt(pow((truth_lepPhi->at(z)-reco_lepPhi->at(lepW_index)),2)+pow((truth_lepEta->at(z)-reco_lepEta->at(lepW_index)),2));
                    //cout << "2 " << z << " " << tmp_delta_R<< endl;
                    if (arr_index[2] > -1){
		      match_delta_R = sqrt  (pow((truth_lepPhi->at(arr_index[2])-reco_lepPhi->at(lepW_index)),2)+pow((truth_lepEta->at(arr_index[2])-reco_lepEta->at(lepW_index)),2));}
		    else match_delta_R =5;
                    if ((tmp_delta_R<=delta_R) && (tmp_delta_R < match_delta_R))
                    {
                        match_i=2;
                        delta_R=tmp_delta_R;
                        lepthird_index = z;
                        //cout << "2 " << z << " " << delta_R<< endl;
                        
                    }
                }
                
                if (match_i<10){
                   // cout << match_i<< endl;
		  //new_match[match_i]=1000;
                    arr_index[match_i]=z;
                    idata <<z<<endl;
                    // cout << "lep "<< z << " match" << match_i << endl;
                    deltaR->Fill(delta_R,weight);
                    check++;
                }
            }
           
	    cout << "lep third index " << lepthird_index << endl;
                
            //matching truth leptons based on flavor, charge, motherid and origin to determine which is the 4th
            if ((arr_index[0] > -1) && (arr_index[1] > -1) && (arr_index[2] > -1) && (lepthird_index > -1))
            {
	      //cout << check << " " << lepthird_index << endl;
                //turning the three reco leptons into tlv
               /* particle1_mass= (truth_lepFlavor->at(arr_index[0]) ==1)?0.511:105.658;  //MeV
                lep1.SetPtEtaPhiM(1000.*truth_lepPt->at(arr_index[0]), truth_lepEta->at(arr_index[0]),truth_lepPhi->at(arr_index[0]),particle1_mass);//MeV
                
                particle2_mass= (truth_lepFlavor->at(arr_index[1]) ==1)?0.511:105.658;  //MeV
                lep2.SetPtEtaPhiM(1000.*truth_lepPt->at(arr_index[1]), truth_lepEta->at(arr_index[1]),truth_lepPhi->at(arr_index[1]),particle2_mass);//MeV*/
	      //cout << arr_index[lepthird_index] << endl;
                particle3_mass= (truth_lepFlavor->at(lepthird_index) ==1)?0.511:105.658;  //MeV
                lep3.SetPtEtaPhiM(1000.*truth_lepPt->at(lepthird_index), truth_lepEta->at(lepthird_index),truth_lepPhi->at(lepthird_index),particle3_mass);//MeV
                
                num_entries_pass++;
                //cout << fabs(truth_lepPt->at(arr_index[0])-truth_lepPt->at(arr_index[1])) << endl;
                /*if ((truth_lepFlavor->at(arr_index[0]) == truth_lepFlavor->at(arr_index[1]))&&((-1)*truth_lepCharge->at(arr_index[0]) == truth_lepCharge->at(arr_index[1])))
                {
                    
                    f_flavor=truth_lepFlavor->at(arr_index[2]);
                    f_charge=(-1)*truth_lepCharge->at(arr_index[2]);
                    m12 = (lep1+lep2).M()/1000.;
                    lepThird = lep3;
                    mll_12->Fill(m12,weight);
                    
                }
                else if ((truth_lepFlavor->at(arr_index[2]) == truth_lepFlavor->at(arr_index[1]))&&((-1)*truth_lepCharge->at(arr_index[2]) == truth_lepCharge->at(arr_index[1])))
                {
                    f_flavor=truth_lepFlavor->at(arr_index[0]);
                    f_charge=(-1)*truth_lepCharge->at(arr_index[0]);
                    m12 = (lep3+lep2).M()/1000.;
                    lepThird = lep1;
                    mll_12->Fill(m12,weight);
                }
                
                else if ((truth_lepFlavor->at(arr_index[2]) == truth_lepFlavor->at(arr_index[0]))&&((-1)*truth_lepCharge->at(arr_index[2]) == truth_lepCharge->at(arr_index[0])))
                {
                    f_flavor=truth_lepFlavor->at(arr_index[1]);
                    f_charge=(-1)*truth_lepCharge->at(arr_index[1]);
                    m12 = (lep1+lep3).M()/1000.;
                    lepThird = lep2;
                    mll_12->Fill(m12,weight);
                }*/
                odata << ".. event: " << i << endl;
                fidata << ".. event: " << i << endl;

                odata << f_flavor << ','<< f_charge  << endl;
                odata << "electrons " << num_electrons<<','<<"muons " << num_muons << endl;
		
		cout << "index 0: " << arr_index[0] << " index 1: " << arr_index[1] << " index 2: " << arr_index[2] << endl;
                delta_mll = 100;
		lepThird = lep3;
		f_charge = (-1)*truth_lepCharge->at(lepthird_index);
		f_flavor = truth_lepFlavor->at(lepthird_index);
		//cout << "4 charge " << f_charge << " flavor" << f_flavor <<endl;
                for (int z=0; z<truth_nSignalLeptons;z++)
                {
		  //  cout << 5 << endl;
		   cout << z << " " <<arr_index[1] << " " << arr_index[0] << " " << arr_index[2] << endl;
                  //  if ((truth_lepPt->at(z) < 1)) continue;
                    //cout << truth_lepPt->at(z) << endl;
                    if ((z==arr_index[1])||(z==arr_index[0])||(z==arr_index[2]))continue;
                    /*if (truth_nSignalLeptons == 4)
                    {
                        num_four++;
                        count_four ++;

                        index_fourth=z;
			//    cout << "4 leptons index " << index_fourth << endl;
                        particle4_mass= (truth_lepFlavor->at(z) ==1)?0.511:105.658;  //MeV
                        lep4.SetPtEtaPhiM(1000.*truth_lepPt->at(z), truth_lepEta->at(z),truth_lepPhi->at(z),particle4_mass);//MeV
                        m34 = (lepThird+lep4).M()/1000.;
                        //fidata << num_four << ","<< fidata << z << endl;
                        //fidata <<truth_lepFlavor->at(z) << "," << truth_lepCharge->at(z) << "," <<truth_lepPhi->at(z) << "," << truth_lepEta->at(z) <<","<<truthLepOrigin->at(z) << ","<< truthLepMotherID->at(z) << ","<< truth_lepPt->at(z)<< "\n";
                        break;
			}*/
                    //else
                    //{
		    cout << z << " " <<truth_lepFlavor->at(z) << f_flavor << " "  << f_charge << " " <<truth_lepCharge->at(z) << endl;
		      if (f_flavor != truth_lepFlavor->at(z)) continue;
                       if (f_charge != truth_lepCharge->at(z)) continue;
                        particle4_mass= (truth_lepFlavor->at(z) == 1)?0.511:105.658;  //MeV
                        lep4.SetPtEtaPhiM(1000.*truth_lepPt->at(z), truth_lepEta->at(z),truth_lepPhi->at(z),particle4_mass);//MeV
                        tmp_mll = (lepThird+lep4).M()/1000.;
                        count_four++;
			cout << "tmp_mll " << tmp_mll << endl;
                        if (abs(tmp_mll - 91.188)<delta_mll)
                        {
                            delta_mll = abs(tmp_mll - 91.188);
                            m34 = tmp_mll;
			    cout << "mll " << tmp_mll << endl;
                            index_fourth = z;
                        }
                    }
		// }
		//cout << "index_fourth " << index_fourth << endl;
		
               // cout << index_fourth << " " << m34 << endl;

                if (count_four == 0 || index_fourth < 0) continue;
		mll_34 -> Fill (m34, weight);
		int z = index_fourth;
		cout << "* " <<reco_eventNo << " * " << truth_lepFlavor->at(z) << " * "<<truth_lepPt->at(z) << " *" << truth_lepEta->at(z) << " * " <<truth_lepPhi->at(z)<< " * " <<  endl;

		gdata << "* " <<reco_eventNo << " * " << truth_lepFlavor->at(z) << " * "<<truth_lepPt->at(z) << " * \
" << truth_lepEta->at(z) << " * " <<truth_lepPhi->at(z)<< " * " <<  endl;
		//lep_outcome->Fill(truthLepOutCome->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
		    // lep_type->Fill(truthLepType->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                    //mother_id->Fill(truthLepOrigin->at(index_fourth), weight);
                    total_plot++;
                    pdata << ".. event: " << i << endl;
                    pdata << num_four << endl;
		    //  pdata <<truth_lepFlavor->at(index_fourth) << ',' << truth_lepCharge->at(index_fourth) << ',' <<truth_lepPhi->at(index_fourth) << ',' << truth_lepEta->at(index_fourth) <<','<<truthLepOrigin->at(index_fourth) << ','<< truthLepMotherID->at(index_fourth) << ','<< truth_lepPt->at(index_fourth)<< "\n";

                    truth_lep_pt = truth_lepPt->at(index_fourth);
                    truth_lep_eta = truth_lepEta->at(index_fourth);
                    //truth_lep_id = truth_lepFlavor->at(index_fourth);

                    outputTree->Fill();
		    //  cout << "6 index fourth " << index_fourth << endl;
                    if (truth_lepFlavor->at(index_fourth)==2)
                    {
		      //cout << " here " << endl;
                        if ((truth_lepPt->at(index_fourth) <= 7)||(fabs(truth_lepEta->at(index_fourth)) >= 2.5))
                        {
			  eta_mu_outside->Fill(truth_lepEta->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
			  Pt_mu_outside->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                            

                            fraction_mu++;
                            fraction++;

                        }
                        if(fabs(truth_lepEta->at(index_fourth)) >= 2.5)
                        {
			  //Pt_mu_noptcut_out->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                           // fraction_mu++;
                            //fraction++;
                        }

                        if (fabs(truth_lepEta->at(index_fourth)) < 2.5)
                        {
			  //Pt_mu_noptcut->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                           // fraction_mu_in++;
                           // fraction_in++;
                        }
                        
                        if ((truth_lepPt->at(index_fourth) > 7)&&(fabs(truth_lepEta->at(index_fourth)) < 2.5))
                        {
			  Pt_muon->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                            

			  eta_muon->Fill(truth_lepEta->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                            fraction_mu_in++;
                            fraction_in++;

                        }
                    }
                    if (truth_lepFlavor->at(index_fourth)==1)
                    {
		      // cout << " here 2 " << endl;
                        if ((truth_lepPt->at(index_fourth) <= 7)||(fabs(truth_lepEta->at(index_fourth)) >= 2.5))
                        {
			  eta_ele_outside->Fill(truth_lepEta->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
			  Pt_ele_outside->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));

                            fraction_ele++;
                            fraction++;

                        }
                        
                        if(fabs(truth_lepEta->at(index_fourth)) >= 2.5)
                        {
			  //Pt_ele_noptcut_out->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                            //fraction_ele++;
                            //fraction++;
                        }
                        
                        if (fabs(truth_lepEta->at(index_fourth)) < 2.5)
                        {
			  //Pt_ele_noptcut->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));

                            //fraction_ele_in++;
                            //fraction_in++;
                        }
                        
                        if ((truth_lepPt->at(index_fourth) > 7)&&(fabs(truth_lepEta->at(index_fourth)) < 2.5))
                        {
			  Pt_electron->Fill(truth_lepPt->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));

			  eta_electron->Fill(truth_lepEta->at(index_fourth),TotalWeightNoSF*lepSFRecoIdIso->at(lep0_index)*lepSFRecoIdIso->at(lep1_index)*lepSFRecoIdIso->at(lep2_index));
                            

                            fraction_ele_in++;
                            fraction_in++;

                        }
                    }

            }
        }
      }

    outputTree->Write();

    cout << "number of events where truth and reco ids match " << nomatch << endl;
    cout << "num ele " << num_ele << " num muon " << num_muon << endl;
    cout << "total events after cuts:" << total_events << endl;
    cout << "total events after 4 Z leptons:" << total_zevents <<endl;
    cout << "pass matching:" << num_entries_pass << endl;
    cout << "number events with 4th matched:" <<num_four<<endl;
    cout << "number of events being plotted:" << total_plot<<endl;
    cout << "outside fraction ele " << fraction_ele << " fraction mu " << fraction_mu << " out of " << fraction << endl;
    cout << "Percent outside:" << fraction/total_plot << " fraction ele:" << fraction_ele/fraction << " fraction mu:" << fraction_mu/fraction << endl;
    cout << "inside fraction ele " << fraction_ele_in << " fraction mu " << fraction_mu_in << " out of " << fraction_in << endl;
    cout << "Percent inside:" << fraction_in/total_plot << " fraction ele:" << fraction_ele_in/fraction_in << " fraction mu:" << fraction_mu_in/fraction_in << endl;
    cout << "Muons: " << (fraction_mu + fraction_mu_in) << " % in: " << fraction_mu_in/(fraction_mu_in + fraction_mu) << " % out: " <<fraction_mu/(fraction_mu_in + fraction_mu)  << endl;
    cout << "Electrons: " << (fraction_ele + fraction_ele_in) << " % in: " << fraction_ele_in/(fraction_ele_in + fraction_ele) << " % out: " <<fraction_ele/(fraction_ele_in + fraction_ele)  << endl;
    cout << "number of events with > 1 4th lepton:" << count_gt4 << endl;
    
    cout << "muons inside(from eta): " << eta_muon -> Integral(-1,-1) <<endl;
    cout << "muons inside(from pt): " << Pt_muon -> Integral(-1,-1) <<endl;
    cout << "electrons inside(from eta): " << eta_electron->Integral(-1,-1)<<endl;
    cout << "electrons inside(from pt): " << Pt_electron -> Integral(-1,-1)<<endl;
    cout << "muons outside(from pt): " << Pt_mu_outside -> Integral(-1,-1) <<endl;
    cout << "muons outside(from eta): " << eta_mu_outside -> Integral(-1,-1)<<endl;
    cout << "electrons outside(from pt): " << Pt_ele_outside->Integral(-1,-1)<<endl;
    cout << "electrons outside(from eta): " << eta_ele_outside->Integral(-1,-1)<<endl;
    
    cout << "eee: " << chanflav-> GetBinContent(8) <<endl;
    cout << "eem: " << chanflav -> GetBinContent(9) <<endl;
    cout << "emm: " << chanflav -> GetBinContent(10) <<endl;
    cout << "mmm: " << chanflav -> GetBinContent(11) <<endl;

    
    //gStyle->SetOptStat("");

    /*TCanvas* c50 = new TCanvas("c50","c50",10,10, 1000, 700);
    chanflav->Draw();
    c50->Print("~/Desktop/chanflav.pdf");*/
    
    /*    TCanvas* c1 = new TCanvas("c1","c1",10,10, 1000, 700);
    c1->cd();
    c1->SetLogy();
    deltaR->Draw();
    deltaR->SetTitle("");
    deltaR->GetXaxis()->SetTitle("#Delta R between Truth and Reco");
    deltaR->GetYaxis()->SetTitle("entries");
    c1->Print("deltaR.pdf");
    cout <<deltaR->Integral() <<endl;
    
    
    TCanvas* c14 = new TCanvas("c14","c14",10,10, 1000, 700);
    c14->cd();
    c14->SetLogy();
    Pt_muon->Draw();
    Pt_muon->SetTitle("");
    Pt_muon->GetXaxis()->SetTitle("p^{T}_{fourth muon}");
    Pt_muon->GetYaxis()->SetTitle("entries");
    TLegend* leg = new TLegend(0.7,0.8,0.9,0.9);
    leg->AddEntry(Pt_muon,"Muons","l");
    leg->Draw();
    c14->Print("pt_muon.pdf");
    
    TCanvas* c15 = new TCanvas("c15","c15",10,10, 1000, 700);
    c15->cd();
    eta_muon->Draw();
    eta_muon->SetTitle("");
    eta_muon->GetXaxis()->SetTitle("#eta_{fourth muon}");
    eta_muon->GetYaxis()->SetTitle("entries");
    TLegend* leg1 = new TLegend(0.7,0.8,0.9,0.9);
    leg1->AddEntry(eta_muon,"Muons","l");
    leg1->Draw();
    c15->Print("eta_muon.pdf");
    cout << " mu in : " << eta_muon->GetEntries();

    TCanvas* c16 = new TCanvas("c16","c16",10,10, 1000, 700);
    c16->cd();
    c16->SetLogy();
    Pt_electron->Draw();
    Pt_electron->SetTitle("");
    Pt_electron->GetXaxis()->SetTitle("p^{T}_{fourth electron}");
    Pt_electron->GetYaxis()->SetTitle("entries");
    TLegend* leg2 = new TLegend(0.7,0.8,0.9,0.9);
    leg2->AddEntry(Pt_electron,"Electrons","l");
    leg2->Draw();
    c16->Print("pt_electron.pdf");
    
    
    TCanvas* c17 = new TCanvas("c17","c17",10,10, 1000, 700);
    c17->cd();
    eta_electron->Draw();
    eta_electron->SetTitle("");
    eta_electron->GetXaxis()->SetTitle("#eta_{fourth electron}");
    eta_electron->GetYaxis()->SetTitle("entries");
    TLegend* leg3 = new TLegend(0.7,0.8,0.9,0.9);
    leg3->AddEntry(eta_electron,"Electrons","l");
    leg3->Draw();
    c17->Print("eta_electron.pdf");
    cout << " ele in : " << eta_electron ->GetEntries();

    
    TCanvas* c18 = new TCanvas("c18","c18",10,10, 1000, 700);
    c18->cd();
    c18->SetLogy();
    Pt_mu_outside->Draw();
    Pt_mu_outside->SetTitle("");
    Pt_mu_outside->GetXaxis()->SetTitle("p^{T}_{fourth muon}");
    Pt_mu_outside->GetYaxis()->SetTitle("entries");
    TLegend* leg4 = new TLegend(0.7,0.8,0.9,0.9);
    leg4->AddEntry(Pt_mu_outside,"Muons","l");
    leg4->Draw();
    c18->Print("pt_mu_outside.pdf");

    
    
    TCanvas* c19 = new TCanvas("c19","c19",10,10, 1000, 700);
    c19->cd();
    eta_mu_outside->Draw();
    eta_mu_outside->SetTitle("");
    eta_mu_outside->GetXaxis()->SetTitle("#eta_{fourth muon}");
    eta_mu_outside->GetYaxis()->SetTitle("entries");
    TLegend* leg5 = new TLegend(0.7,0.8,0.9,0.9);
    leg5->AddEntry(eta_mu_outside,"Muons","l");
    leg5->Draw();
    c19->Print("eta_mu_outside.pdf");
    cout << " mu out : " << eta_mu_outside ->GetEntries();
    
    TCanvas* c20 = new TCanvas("c20","c20",10,10, 1000, 700);
    c20->cd();
    c20->SetLogy();
    Pt_ele_outside->Draw();
    Pt_ele_outside->SetTitle("");
    Pt_ele_outside->GetXaxis()->SetTitle("p^{T}_{fourth electron}");
    Pt_ele_outside->GetYaxis()->SetTitle("entries");
    TLegend* leg6 = new TLegend(0.7,0.8,0.9,0.9);
    leg6->AddEntry(Pt_ele_outside,"Electrons","l");
    leg6->Draw();
    c20->Print("pt_ele_outside.pdf"); 
    //cout << " ele out " << Pt_ele_outside->GetEntries << endl;
    
    TCanvas* c21 = new TCanvas("c21","c21",10,10, 1000, 700);
    c21->cd();
    eta_ele_outside->Draw();
    eta_ele_outside->SetTitle("");
    eta_ele_outside->GetXaxis()->SetTitle("#eta_{fourth electron}");
    eta_ele_outside->GetYaxis()->SetTitle("entries");
    TLegend* leg7 = new TLegend(0.7,0.8,0.9,0.9);
    leg7->AddEntry(eta_ele_outside,"Electrons","l");
    leg7->Draw();
    c21->Print("eta_ele_outside.pdf");
    cout << " ele out : " << eta_ele_outside ->GetEntries();
    */  
    TCanvas* c30 = new TCanvas("c30","c30",10,10, 1000, 700);
    c30->cd();
    mll_34->Draw();
    mll_34->SetTitle("");
    mll_34->GetXaxis()->SetTitle("M_{ll}");
    mll_34->GetYaxis()->SetTitle("entries");
    c30->Print("mll_34.pdf");
    cout << "mll " << mll_34->Integral()<< endl;
    
    return 0;
}
