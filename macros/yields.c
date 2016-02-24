//
//  plot2.c
//  
//
//  Created by Elodie Resseguie on 11/22/15.
//
//
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
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
#include <iomanip>
#include <sstream>
using namespace std;
    
int mc_event(const char *path)
{
  //  gROOT->SetBatch();
    TCut weight = "TotalWeightNoSF";
    TCut weight_mc;
    TCut new_weight;

    new_weight = "(PassEvent_SMWZsig)";
       
    TCut weight_eee = new_weight && " (ChanFlavor==7)";
    TCut weight_eem = new_weight && "(ChanFlavor==8)";
    TCut weight_emm = new_weight && "(ChanFlavor==9)";
    TCut weight_mmm = new_weight && "(ChanFlavor==10)";
    TCut weight_wm = new_weight && "(lepW_index >= 0)" && "(lepCharge[lepW_index] == -1)";
    TCut weight_wp = new_weight && "(lepW_index >= 0)" && "(lepCharge[lepW_index] == 1)";
    weight_mc = "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig";

    const char *files[14] ={"singletop.root", "tother.root", "ttbar.root", "ttv.root", "tw.root", "tz.root", "vvv.root", "wgam.root", "wz.root", "zgam.root", "zjet.root", "zz.root", "ww.root", "data.root"};
    //const char *path = "/home/mjoana/data2/FinalWZ_rel44/fetch/data-SKIM/hadd/";

    TCanvas *myCanvas = new TCanvas();    

    float data_total, data_eee, data_eem, data_emm, data_mmm, data_wp, data_wm;
    data_total = 0.0;
    data_eee = 0.0;
    data_eem = 0.0;
    data_emm = 0.0;
    data_mmm = 0.0;
    data_wp = 0.0;
    data_wm = 0.0;

    float total[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float total_eee[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float total_eem[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float total_emm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float total_mmm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    float uncert[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_eee[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_eem[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_emm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_mmm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    float wp[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_wp[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float wm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float uncert_wm[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    float total_other,total_other_eee, total_other_eem, total_other_emm, total_other_mmm, total_other_wm, total_other_wp;
    float uncert_other,uncert_other_eee, uncert_other_eem, uncert_other_emm, uncert_other_mmm, uncert_other_wm, uncert_other_wp;

    float final, final_eee, final_emm, final_eem, final_mmm, final_wp, final_wm;
    float uncert_final, uncert_final_eee, uncert_final_eem, uncert_final_emm, uncert_final_mmm, uncert_final_wp, uncert_final_wm;

    float zjet, zjet_eee, zjet_emm, zjet_eem, zjet_mmm, zjet_wp, zjet_wm;
    float uncert_zjet, uncert_zjet_eee, uncert_zjet_eem, uncert_zjet_emm, uncert_zjet_mmm, uncert_zjet_wp, uncert_zjet_wm;

    float ttbtw, ttbtw_eee, ttbtw_emm, ttbtw_eem, ttbtw_mmm, ttbtw_wp, ttbtw_wm;
    float uncert_ttbtw, uncert_ttbtw_eee, uncert_ttbtw_eem, uncert_ttbtw_emm, uncert_ttbtw_mmm, uncert_ttbtw_wp, uncert_ttbtw_wm;

    zjet=0.0;
    zjet_eee=0.0;
    zjet_eem=0.0;
    zjet_emm=0.0;
    zjet_mmm=0.0;
    zjet_wm=0.0;
    zjet_wp=0.0;
    uncert_zjet=0.0;
    uncert_zjet_eee=0.0;
    uncert_zjet_eem=0.0;
    uncert_zjet_emm=0.0;
    uncert_zjet_mmm=0.0;
    uncert_zjet_wm=0.0;
    uncert_zjet_wp=0.0;

    ttbtw=0.0;
    ttbtw_eee=0.0;
    ttbtw_eem=0.0;
    ttbtw_emm=0.0;
    ttbtw_mmm=0.0;
    ttbtw_wm=0.0;
    ttbtw_wp=0.0;
    uncert_ttbtw=0.0;
    uncert_ttbtw_eee=0.0;
    uncert_ttbtw_eem=0.0;
    uncert_ttbtw_emm=0.0;
    uncert_ttbtw_mmm=0.0;
    uncert_ttbtw_wm=0.0;
    uncert_ttbtw_wp=0.0;

    total_other=0.0;
    total_other_eee=0.0;
    total_other_eem=0.0;
    total_other_emm=0.0;
    total_other_mmm=0.0;
    total_other_wm=0.0;
    total_other_wp=0.0;
    uncert_other=0.0;
    uncert_other_eee=0.0;
    uncert_other_eem=0.0;
    uncert_other_emm=0.0;
    uncert_other_mmm=0.0;
    uncert_other_wm=0.0;
    uncert_other_wp=0.0;

    final=0.0;
    final_eee=0.0;
    final_emm=0.0;
    final_eem=0.0;
    final_mmm=0.0;
    final_wp=0.0;
    final_wm=0.0;
    uncert_final=0.0;
    uncert_final_eee=0.0;
    uncert_final_eem=0.0;
    uncert_final_emm=0.0;
    uncert_final_mmm=0.0;
    uncert_final_wp=0.0; 
    uncert_final_wm=0.0;

    TFile *f_data;
    TTree *t_data;
    for (int i=0; i < 14; i++){
      f_data = new TFile(((string)path+(string)files[i]).c_str());
      t_data = (TTree*)f_data->Get("physics");
      std::cout << std::fixed;
      std::cout << std::setprecision(2);
      if (i==13){
	data_total = t_data->GetEntries(new_weight);
	data_eee = t_data->GetEntries(weight_eee);
	data_eem = t_data->GetEntries(weight_eem);
	data_emm = t_data->GetEntries(weight_emm);
	data_mmm = t_data->GetEntries(weight_mmm);
	data_wm = t_data->GetEntries(weight_wm);
	data_wp = t_data->GetEntries(weight_wp);

      }

      else{
	TH1F *h1= new TH1F("h1", "hist from tree", 2,0,2);
	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(ChanFlavor==7)");
	total_eee[i] =  h1 -> GetBinContent(2);
	uncert_eee[i] = h1->GetBinError(2);
	final_eee+=total_eee[i];
	uncert_final_eee+= pow(uncert_eee[i],2);
	if ((i==0) || (i==1) || (i==7)){
	  total_other_eee+=total_eee[i];
	  uncert_other_eee+= pow(uncert_eee[i],2);
	}
	if ((i==9) || (i==10)){
          zjet_eee+=total_eee[i];
          uncert_zjet_eee+= pow(uncert_eee[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	    ttbtw_eee+=total_eee[i];
	    uncert_ttbtw_eee+= pow(uncert_eee[i],2);
	}

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(ChanFlavor==8)");
	total_eem[i] =  h1 -> GetBinContent(2);
        uncert_eem[i] = h1->GetBinError(2);
        final_eem+=total_eem[i];
	uncert_final_eem+= pow(uncert_eem[i],2);
	if ((i==0) || (i==1)|| (i==7)){
          total_other_eem+=total_eem[i];
          uncert_other_eem+= pow(uncert_eem[i],2);
        }
	if ((i==9) || (i==10)){
          zjet_eem+=total_eem[i];
          uncert_zjet_eem+= pow(uncert_eem[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	  ttbtw_eem+=total_eem[i];
	  uncert_ttbtw_eem+= pow(uncert_eem[i],2);
        }

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(ChanFlavor==9)");
	total_emm[i] =  h1 -> GetBinContent(2);
        uncert_emm[i] = h1->GetBinError(2);
       	final_emm+=total_emm[i];
	uncert_final_emm+= pow(uncert_emm[i],2);
	if ((i==0) ||(i==1)|| (i==7)){
          total_other_emm+=total_emm[i];
          uncert_other_emm+= pow(uncert_emm[i],2);
        }
	if ((i==9) || (i==10)){
          zjet_emm+=total_emm[i];
          uncert_zjet_emm+= pow(uncert_emm[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	  ttbtw_emm+=total_emm[i];
	  uncert_ttbtw_emm+= pow(uncert_emm[i],2);
        }

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(ChanFlavor==10)");
	total_mmm[i] =  h1 -> GetBinContent(2);
        uncert_mmm[i] = h1->GetBinError(2);
       	final_mmm+=total_mmm[i];
	uncert_final_mmm+= pow(uncert_mmm[i],2);
	if ((i==0) ||(i==1)|| (i==7) ){
          total_other_mmm+=total_mmm[i];
          uncert_other_mmm+= pow(uncert_mmm[i],2);
        }
	if ((i==9) || (i==10)){
          zjet_mmm+=total_mmm[i];
          uncert_zjet_mmm+= pow(uncert_mmm[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	  ttbtw_mmm+=total_mmm[i];
	  uncert_ttbtw_mmm+= pow(uncert_mmm[i],2);
        }

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(lepW_index>=0)*(lepCharge[lepW_index]==1)");
        wp[i] =  h1 -> GetBinContent(2);
        uncert_wp[i] = h1->GetBinError(2);
       	final_wp+=wp[i];
	uncert_final_wp+= pow(uncert_wp[i],2);
	if ((i==0) ||(i==1)|| (i==7)){
          total_other_wp+=wp[i];
          uncert_other_wp+= pow(uncert_wp[i],2);
        }
	if ((i==9) || (i==10)){
          zjet_wp+=wp[i];
          uncert_zjet_wp+= pow(uncert_wp[i],2);
        }
	if ((i ==4) || (i==2)|| (i==12)){
	  ttbtw_wp+=wp[i];
	  uncert_ttbtw_wp+= pow(uncert_wp[i],2);
        }

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig*(lepW_index>=0)*(lepCharge[lepW_index]==-1)");
        wm[i] =  h1 -> GetBinContent(2);
        uncert_wm[i] = h1->GetBinError(2);
       	final_wm+=wm[i];
	uncert_final_wm+= pow(uncert_wm[i],2);
	if ((i==0) || (i==1)|| (i==7) ){
          total_other_wm+=wm[i];
          uncert_other_wm+= pow(uncert_wm[i],2);
        }
	if ((i==9) || (i==10)){
          zjet_wm+=wm[i];
          uncert_zjet_wm+= pow(uncert_wm[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	  ttbtw_wm+=wm[i];
	  uncert_ttbtw_wm+= pow(uncert_wm[i],2);
        }

	t_data->Draw("GRL>>h1", "TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig");
	total[i] =  h1 -> GetBinContent(2);
        uncert[i] = h1->GetBinError(2);
	final+=total[i];
	uncert_final+= pow(uncert[i],2);
	if ((i==0) ||(i==1)|| (i==7)){
          total_other+=total[i];
          uncert_other+= pow(uncert[i],2);
        }
	if ((i==9) || (i==10)){
          zjet+=total[i];
          uncert_zjet+= pow(uncert[i],2);
        }
	if ((i ==4) || (i==2) || (i==12)){
	  ttbtw+=total[i];
	  uncert_ttbtw+= pow(uncert[i],2);
        }
      }
    }

    for (int i=0; i <13; i++){
      cout << files[i] << " ";
      cout << "& " << total_eee[i] << "$\\pm$ " << uncert_eee[i];
      cout << "& " << total_eem[i] << "$\\pm$ " << uncert_eem[i];
      cout << "& " << total_emm[i] << "$\\pm$ " << uncert_emm[i];
      cout << "& " << total_mmm[i] << "$\\pm$ " << uncert_mmm[i];
      cout << "& " << wp[i] << "$\\pm$ " << uncert_wp[i];
      cout << "& " << wm[i] << "$\\pm$ " << uncert_wm[i];
      cout << "& " << total[i] << "$\\pm$ " << uncert[i] << endl;
    }

    cout << "ttbar+tw+ww & " << ttbtw_eee << "$\\pm$ " << (sqrt(uncert_ttbtw_eee)) << " & " << ttbtw_eem << "$\\pm$ " << (sqrt(uncert_ttbtw_eem));
    cout <<  " & " << ttbtw_emm << "$\\pm$ " << (sqrt(uncert_ttbtw_emm)) <<  " & " << ttbtw_mmm << "$\\pm$ " << (sqrt(uncert_ttbtw_mmm));
    cout  <<  " & " << ttbtw_wp << "$\\pm$ " << (sqrt(uncert_ttbtw_wp)) << " & " << ttbtw_wm << "$\\pm$ " << (sqrt(uncert_ttbtw_wm));
    cout << " & " << ttbtw << "$\\pm$ " << (sqrt(uncert_ttbtw)) << endl;

    cout << "zjet+zgam & " << zjet_eee << "$\\pm$ " << (sqrt(uncert_zjet_eee)) << " & " << zjet_eem << "$\\pm$ " << (sqrt(uncert_zjet_eem));
    cout <<  " & " << zjet_emm << "$\\pm$ " << (sqrt(uncert_zjet_emm)) <<  " & " << zjet_mmm << "$\\pm$ " << (sqrt(uncert_zjet_mmm));
    cout  <<  " & " << zjet_wp << "$\\pm$ " << (sqrt(uncert_zjet_wp)) << " & " << zjet_wm << "$\\pm$ " << (sqrt(uncert_zjet_wm));
    cout << " & " << zjet << "$\\pm$ " << (sqrt(uncert_zjet)) << endl;

    cout << "other & " << total_other_eee << "$\\pm$ " << (sqrt(uncert_other_eee)) << " & " << total_other_eem << "$\\pm$ " << (sqrt(uncert_other_eem));
    cout <<  " & " << total_other_emm << "$\\pm$ " << (sqrt(uncert_other_emm)) <<  " & " << total_other_mmm << "$\\pm$ " << (sqrt(uncert_other_mmm)); 
    cout  <<  " & " << total_other_wp << "$\\pm$ " << (sqrt(uncert_other_wp)) << " & " << total_other_wm << "$\\pm$ " << (sqrt(uncert_other_wm));
    cout << " & " << total_other << "$\\pm$ " << (sqrt(uncert_other)) << endl;
						  
    cout << "Total Exp. & " << final_eee << "$\\pm$ " << (sqrt(uncert_final_eee)) << " & " << final_eem << "$\\pm$ " << (sqrt(uncert_final_eem));
    cout <<  " & " << final_emm << "$\\pm$ " << (sqrt(uncert_final_emm)) <<  " & " << final_mmm << "$\\pm$ " << (sqrt(uncert_final_mmm));
    cout  <<  " & " << final_wp << "$\\pm$ " << (sqrt(uncert_final_wp)) << " & " << final_wm << "$\\pm$ " << (sqrt(uncert_final_wm));
    cout << " & " << final << "$\\pm$ " << (sqrt(uncert_final)) <<endl;
    
    cout << "Data & " << data_eee << " & " << data_eem << " & " << data_emm << " & " << data_mmm << " & " << data_wp << " & " << data_wm << " & " << data_total << endl;

    cout << "Data/Exp. & " << data_eee/final_eee << "$\\pm$ " <<sqrt(pow(sqrt(data_eee)/data_eee,2)+pow(uncert_final_eee/final_eee,2)) << " & ";
    cout << data_eem/final_eem << "$\\pm$ " << sqrt(pow(sqrt(data_eem)/data_eem,2)+pow(uncert_final_eem/final_eem,2)) << " & " ;
    cout << data_emm/final_emm << "$\\pm$ " << sqrt(pow(sqrt(data_emm)/data_emm,2)+pow(uncert_final_emm/final_emm,2)) << " & " ;
    cout << data_mmm/final_mmm << "$\\pm$ " << sqrt(pow(sqrt(data_mmm)/data_mmm,2)+pow(uncert_final_mmm/final_mmm,2)) << " & " ;
    cout << data_wp/final_wp << "$\\pm$ " << sqrt(pow(sqrt(data_wp)/data_wp,2)+pow(uncert_final_wp/final_wp,2)) << " & ";
    cout << data_wm/final_wm << "$\\pm$ " << sqrt(pow(sqrt(data_wm)/data_wm,2)+pow(uncert_final_wm/final_wm,2)) << " & ";
    cout << data_total/final << "$\\pm$ " << sqrt(pow(sqrt(data_total)/data_total,2)+pow(uncert_final/final,2)) << endl;
    
    return 0;
}
