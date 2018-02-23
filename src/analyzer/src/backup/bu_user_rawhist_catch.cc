// Author: Hitoshi Sugimura

/* 
   2009.10 
   2010.8.24 Updated

   --> add multipricity of chamber with trigger
*/

//#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <string.h>

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include <TText.h>
#include <TTree.h>

#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "Unpacker.hh"
#include "filesystem_util.hh"
#include "HistHelper.hh"
#include "lexical_cast.hh"
#include "DetectorID.hh"
#include "init_hist.hh"
#include "start_logo.hh"
#include "DetTitle.hh"
#include "DetName.hh"
#include "makeps.hh"
#include "HodoAnalyzer.hh"
#include "HodoMan.hh"
#include "TreeBuilder.hh"
#include "mppc_pos.hh" // temp

//#include "DCAnalyzer.hh"
//#include "MWPCAnalyzer.hh"
//#include "DCgeomMan.hh"
//#include "MakeHistMan.hh"
//#include "GeomMan.hh"
//#include "DCTdcCalibMan.hh"
//#include "TOFMan.hh"
//#include "CDecoder.hh"
//#include "CRecorder.hh"
//#include "CDiscriminator.hh"
//#include "CTemplate.hh"
//#include "CConfig.hh"
//#include "CEventInfo.hh"

#define DEBUG 0

namespace analyzer
{
    namespace
    {
        using namespace hddaq::unpacker;
        using namespace hddaq;

        enum e_argv
        {
            k_process_name,
            k_conf_path,
            k_istream,
            k_n_arg
        };

        enum e_data_type
        {
            k_trailing     = 0,
            k_trailing_err = 1,
            k_leading      = 2,
            k_leading_err  = 3,

            k_invalid_packet_type   = 0,
            k_invalid_packet_tdc_id = 1,
            k_invalid_packet_tdc_ch = 2,
            k_invalid_packet_data   = 3,
            k_trigger_error         = 4
        };
        enum e_mwcp_data
        {
            k_mwpc_leading,
            k_mwpc_trailing
        };

        const std::string k_hist_init_func = "init_hist";
        // tree ____________________________________________________________________
        static Event event;
        TTree* s_tree;
        //  Histgram Object  _______________________________________________________
        std::vector<TH2*> s_cft_adc_hi(NumOfLayersCFT);
        std::vector<TH2*> s_cft_adc_lo(NumOfLayersCFT);
        std::vector<TH2*> s_cft_adc_pe(NumOfLayersCFT);
        std::vector<TH2*> s_cft_tdc(NumOfLayersCFT);
        std::vector<TH1*> s_HitPatCFT;
        std::vector<TH1*> s_MultiplicityCFT;

        std::vector<TH1*> s_BGO_A(NumOfSegBGO);
        std::vector<TH1*> s_BGO_T(NumOfSegBGO);
        std::vector<TH2*> s_BGO_FA(NumOfSegBGO);
        TH1* s_HitPatBGO;

        std::vector<TH1*> s_T54Counter_A(NumOfLayersT54Counter);
        std::vector<TH1*> s_T54Counter_T(NumOfLayersT54Counter);

        std::vector<TH2*> s_veasiroc_adc_hi(NumOfLayersVEASIROC);
        std::vector<TH2*> s_veasiroc_adc_lo(NumOfLayersVEASIROC);
        std::vector<TH2*> s_veasiroc_tdc(NumOfLayersVEASIROC);
        TH2* s_RICH1; 
        TH2* s_RICH2;
        TH2* s_BGO_Cor;
        TH2* s_BGO_vs_T54;

      // temporary

      static const int s_BGO_ped[8] = 
	{
	  14000, 14000, 30047, 52591,
	  52873, 52549, 52689, 52659
	};

    }

    // Process Begin _______________________________________________________________
    int process_begin(const std::vector<std::string>& argv)
    {
        std::vector<std::string>::const_iterator optNoLogo
            = std::find(argv.begin(), argv.end(), "-l");

        if (optNoLogo==argv.end())
            start_logo();
        else {
            std::vector<std::string>& argtmp
                = const_cast<std::vector<std::string>&>(argv);
            argtmp.erase(std::remove(argtmp.begin(), argtmp.end(), "-l"),
                    argtmp.end());
        }
        initPsMaker();

        std::cout << "#D argument list" << std::endl;
        std::copy(argv.begin(), argv.end(),
                std::ostream_iterator<std::string>(std::cout, "\n"));
        std::cout << std::endl;

        int nArg = argv.size();
        if (k_n_arg>nArg)
        {
            std::cerr << "#E usage:\n"
                << argv[k_process_name] 
                << " [config file] [input stream]"
                << std::endl;
            std::exit(1);
        }


        const std::string& confFile(argv[k_conf_path]);
        const std::string& dataSrc(argv[k_istream]);

        std::cout << "config file = " << confFile << std::endl;
        std::string dir = dirname(confFile);
        dir += "/";
        std::cout << "dir = " << dir << std::endl;
        std::ifstream conf(confFile.c_str());
        std::map<std::string,std::string> fileList;
        while (conf.good()) {
            std::string l;
            std::getline(conf, l);
            std::istringstream iss(l);
            std::istream_iterator<std::string> issBegin(iss);
            std::istream_iterator<std::string> issEnd;
            std::vector<std::string> param(issBegin, issEnd);
            if (param.empty())
                continue;
            if (param[0].empty())
                continue;
            if (param.size()==2) {
                const std::string& name = param[0];
                std::string value = param[1];
                std::cout << " key = " << name 
                    << " value = " << value << std::endl;
                if (value.find("/")!=0)
                    value = realpath(dir + value);
                fileList[name] = value;
            }
        }

        UnpackerManager& g_unpacker = GUnpacker::get_instance();
        g_unpacker.set_config_file(fileList["UNPACKER:"],
                                   fileList["DIGIT:"],
                                   fileList["CMAP:"]);
        g_unpacker.set_istream(dataSrc);

        init_hist();
        // Tree ______________________________________________________________________
        s_tree = dynamic_cast<TTree*>(gDirectory->Get("tree"));

        // CFT 
        std::vector<std::string> DetNameCFTA_HI, DetNameCFTA_LO, DetNameCFTA_PE, DetNameCFTT;
        DetName m_CFTA_HI, m_CFTA_LO, m_CFTA_PE, m_CFTT;
        m_CFTA_HI.detnameCFTA_HI(DetNameCFTA_HI);
        m_CFTA_LO.detnameCFTA_LO(DetNameCFTA_LO);
        m_CFTA_PE.detnameCFTA_PE(DetNameCFTA_PE);
        m_CFTT.detnameCFTT(DetNameCFTT);

        for(int layer =0;layer<NumOfLayersCFT;++layer)
        {
            s_cft_adc_hi[layer]=H2::get(DetNameCFTA_HI[layer]);
            s_cft_adc_lo[layer]=H2::get(DetNameCFTA_LO[layer]);
            s_cft_adc_pe[layer]=H2::get(DetNameCFTA_PE[layer]);
            s_cft_tdc[layer]=H2::get(DetNameCFTT[layer]);
        }
        // Hit Pattern & Multiplicity
        for(int layer = 1; layer <= 2; ++layer) {
            for(int seg = 1; seg <= 4; ++seg) {
                s_HitPatCFT.push_back(
                        H1::get(Form("Hit_Pattern_CFT_PHI_%d_%d", layer, seg)));
                s_MultiplicityCFT.push_back(
                        H1::get(Form("Multiplicity_CFT_PHI_%d_%d", layer, seg)));
            }
        }
        s_HitPatCFT.push_back(H1::get("Hit_Pattern_CFT_U"));
        s_MultiplicityCFT.push_back(H1::get("Multiplicity_CFT_U"));

        // BGO
        std::vector<std::string> DetNameBGO_A, DetNameBGO_T, DetNameBGO_FA;
        DetName m_BGO;
        m_BGO.detnameBGOA(DetNameBGO_A);
        m_BGO.detnameBGOT(DetNameBGO_T);
        m_BGO.detnameBGOFA(DetNameBGO_FA);

        for(int seg =0;seg<NumOfSegBGO;++seg)
        {
            s_BGO_A[seg]=H1::get(DetNameBGO_A[seg]);
            s_BGO_T[seg]=H1::get(DetNameBGO_T[seg]);
            s_BGO_FA[seg]=H2::get(DetNameBGO_FA[seg]);
        }
        s_HitPatBGO = H1::get("Hit_Pattern_BGO");
        s_BGO_Cor = H2::get("BGO_A_COR");
        s_BGO_vs_T54 = H2::get("BGO_VS_T54");


	// VME-EASIROIC
        std::vector<std::string> DetNameVEASIROCA_HI, DetNameVEASIROCA_LO, DetNameVEASIROCT;
        DetName m_VEASIROCA_HI, m_VEASIROCA_LO, m_VEASIROCT;
        m_VEASIROCA_HI.detnameVEASIROCA_HI(DetNameVEASIROCA_HI);
        m_VEASIROCA_LO.detnameVEASIROCA_LO(DetNameVEASIROCA_LO);
        m_VEASIROCT.detnameVEASIROCT(DetNameVEASIROCT);

        for(int layer =0;layer<NumOfLayersVEASIROC;++layer)
        {
            s_veasiroc_adc_hi[layer]=H2::get(DetNameVEASIROCA_HI[layer]);
            s_veasiroc_adc_lo[layer]=H2::get(DetNameVEASIROCA_LO[layer]);
            s_veasiroc_tdc[layer]=H2::get(DetNameVEASIROCT[layer]);
        }
#if 0
        s_entry = H1::get("Trigger_Entry");
#endif

        return 0;
    }

    //______________________________________________________________________________
    int process_end()
    {
      //Event0-tag check
      //g_unpacker.print_tag(); 
      
      return 0;
    }

    //______________________________________________________________________________
    int process_event()
    {
        static UnpackerManager& g_unpacker  = GUnpacker::get_instance();
#if DEBUG
        std::cout << "#D " << __FILE__ << " " << __LINE__ <<std::endl;
#endif
	

#if DEBUG
        std::cout << "#D " << __FILE__ << " " << __LINE__ <<std::endl;
#endif

        // CFT 
        for (int layer=0; layer<NumOfLayersCFT; ++layer)
	  {
	    int NumOfSeg = 0;
	    if (layer==0){NumOfSeg = NumOfSegCFT_UV1;}
	    else if (layer==1){NumOfSeg = NumOfSegCFT_PHI1;}
	    else if (layer==2){NumOfSeg = NumOfSegCFT_UV2;}
	    else if (layer==3){NumOfSeg = NumOfSegCFT_PHI2;}
	    else if (layer==4){NumOfSeg = NumOfSegCFT_UV3;}
	    else if (layer==5){NumOfSeg = NumOfSegCFT_PHI3;}
	    else if (layer==6){NumOfSeg = NumOfSegCFT_UV4;}
	    else if (layer==7){NumOfSeg = NumOfSegCFT_PHI4;}

            for (int ch=0; ch<NumOfSeg; ++ch)
            { 
                int nhits_tdc = g_unpacker.get_entries(DetIdCFT,layer, 0, ch, 0);

                if(nhits_tdc==0)continue;

                for (int ihit=0; ihit<nhits_tdc; ++ihit)
                {
                    int tdc=g_unpacker.get(DetIdCFT, layer, 0, 
                            ch, 0, ihit);
		    //		    std::cout << "#D " << tdc <<std::endl;
                    if(s_cft_tdc[layer])
                        s_cft_tdc[layer]->Fill(ch, tdc);
                }

            }


            for (int ch=0; ch<NumOfSeg; ++ch)
            {
                int nhits_adc_high = g_unpacker.get_entries(DetIdCFT,layer, 0, ch, 2);
                if(nhits_adc_high==0)continue;

                for (int ihit=0; ihit<nhits_adc_high; ++ihit)
		  {
		    int adc=g_unpacker.get(DetIdCFT, layer, 0, 
					   ch, 2, ihit);
                    if(s_cft_adc_hi[layer]){
		      s_cft_adc_hi[layer]->Fill(ch, adc);
		      double pedeCFT = 0.;
		      double gainCFT = 100.;
		    
			pedeCFT = hman->GetPedestal(DetIdCFT,layer,ch,0); 
			gainCFT = hman->GetGain(DetIdCFT,layer,ch,0);	
                        double adc_pe = ((double)adc - pedeCFT)/gainCFT;
                        s_cft_adc_pe[layer]->Fill(ch, adc_pe);
                    }
                }

            }

            for (int ch=0; ch<NumOfSeg; ++ch)
            {
                int nhits_adc_low = g_unpacker.get_entries(DetIdCFT,layer, 0, ch, 3);
                if(nhits_adc_low==0)continue;

                for (int ihit=0; ihit<nhits_adc_low; ++ihit)
                {
                    int adc=g_unpacker.get(DetIdCFT, layer, 0, 
                            ch, 3, ihit);
                    if(s_cft_adc_lo[layer])
                        s_cft_adc_lo[layer]->Fill(ch, adc);
                }

            }
        }

        // HitPattern & Multiplicity of CFT Phi1, Phi2
        std::pair<int, int> cft_time_gate = std::make_pair(540, 600);
	/*
        for(int layer = 0; layer < 2; ++layer) {
            for(int seg = 0; seg < 4; ++seg) {
                int multiplicity = 0;
                int index = layer * 4 + seg;
                for(int ch = 0; ch < NumOfSegCFT_PHI / 4; ++ch) {
                    int cft_layer = (layer == 0) ? 0 : 2;
                    int tdc_nhit = g_unpacker.get_entries(DetIdCFT, cft_layer, 0,
                            ch + seg * (NumOfSegCFT_PHI / 4), 0);
                    for(int hit = 0; hit < tdc_nhit; ++hit) {
                        int tdc = g_unpacker.get(DetIdCFT, cft_layer, 0,
                                ch + seg * (NumOfSegCFT_PHI / 4), 0, hit);
                        if(cft_time_gate.first < tdc &&
                                tdc < cft_time_gate.second) {
                            s_HitPatCFT.at(index)->Fill(ch);
                            ++multiplicity;
                            break;
                        }
                    }
                }
                s_MultiplicityCFT.at(index)->Fill(multiplicity);
            }
        }
	*/

        // HitPattern & Multiplicity of CFT U
        {
	  /*
            int multiplicity = 0;
            for(int ch = 0; ch < NumOfSegCFT_U; ++ch) {
                int tdc_nhit = g_unpacker.get_entries(DetIdCFT, 1, 0, ch, 0);
                for(int hit = 0; hit < tdc_nhit; ++hit) {
                    int tdc = g_unpacker.get(DetIdCFT, 1, 0, ch, 0, hit);
                        if(cft_time_gate.first < tdc &&
                                tdc < cft_time_gate.second) {
                            s_HitPatCFT.at(8)->Fill(ch);
                            ++multiplicity;
                            break;
                        }
                }
            }
            s_MultiplicityCFT.at(8)->Fill(multiplicity);
	  */
        }


	// Event-tag check	
	int counter = g_unpacker.get_counter();
	if(counter%499==0){
	  std::cout << "#D " << counter <<std::endl;
	  g_unpacker.print_tag();
	}

	// VME-EASIROC
        for (int layer=0; layer<NumOfLayersVEASIROC; ++layer)
	  {	    
	    const int NumOfSeg = 64;

	    //TdcTag for OpenCampus	    
	    int TdcTag[NumOfSeg];
	    for (int ch=0; ch<NumOfSeg; ++ch) {TdcTag[ch] = -1;}

            for (int ch=0; ch<NumOfSeg; ++ch)
	      { 
		int nhits_tdc = g_unpacker.get_entries(DetIdVEASIROC,layer, 0, ch, 0);
		if(nhits_tdc==0)continue;
		
                for (int ihit=0; ihit<nhits_tdc; ++ihit)
		  {
                    int tdc=g_unpacker.get(DetIdVEASIROC, layer, 0, 
					   ch, 0, ihit);
		    //		    std::cout << "#D " << tdc <<std::endl;
		    
                    if(s_veasiroc_tdc[layer])
		      s_veasiroc_tdc[layer]->Fill(ch, tdc);
		    
		    // TdcTag is for RICH(Open Campus)
		    if (layer == 0){
		      if (ch>=0 && ch <=31) {
			if (tdc>=345&&tdc<=370) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=350&&tdc<=373) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 1) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=350&&tdc<=373) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=350&&tdc<=373) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 2) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=350&&tdc<=373) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=345&&tdc<=366) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 3) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=355&&tdc<=370) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=355&&tdc<=375) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 4) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=355&&tdc<=373) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=350&&tdc<=370) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 5) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=355&&tdc<=370) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=350&&tdc<=370) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 6) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=350&&tdc<=370) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=350&&tdc<=370) {TdcTag[ch]=1;}
		      }
		    } else if (layer == 7) {
		      if (ch>=0 && ch <=31) {
			if (tdc>=355&&tdc<=370) {TdcTag[ch]=1;}
		      } else {
			if (tdc>=345&&tdc<=367) {TdcTag[ch]=1;}
		      }
		    }
		    // <- TdcTag end

		  }
	      }
	    
            for (int ch=0; ch<NumOfSeg; ++ch)
            {
                int nhits_adc_high = g_unpacker.get_entries(DetIdVEASIROC,layer, 0, ch, 2);
                if(nhits_adc_high==0)continue;

                for (int ihit=0; ihit<nhits_adc_high; ++ihit)
		  {
		    int adc=g_unpacker.get(DetIdVEASIROC, layer, 0, 
					   ch, 2, ihit);
		    //if(layer==18){std::cout << "#D " << adc <<std::endl;}
                    if(s_veasiroc_adc_hi[layer]){
		      s_veasiroc_adc_hi[layer]->Fill(ch, adc);
                    }
		    if(s_veasiroc_adc_lo[layer]){
		      s_veasiroc_adc_lo[layer]->Fill(ch, adc);
		    }

		    // temporary for opencampus
		    if (TdcTag[ch] == 1) {		      
		      double x,y;
		      int board, channel;
		      board = 2*layer+1;
		      if (ch>=32) {
			board += 1;
			channel = ch - 32;
		      } else {channel = ch;}		      
		      mppc_pos(board, channel, &x, &y);
		      s_RICH1->Fill(x, y);
		      s_RICH2->Fill(x, y);
		    }
		    
		  }
            }
	    /*
	      for (int ch=0; ch<NumOfSeg; ++ch)
            {
                int nhits_adc_low = g_unpacker.get_entries(DetIdVEASIROC,layer, 0, ch, 3);
                if(nhits_adc_low==0)continue;

                for (int ihit=0; ihit<nhits_adc_low; ++ihit)
                {
                    int adc=g_unpacker.get(DetIdVEASIROC, layer, 0, 
                            ch, 3, ihit);
                    if(s_veasiroc_adc_lo[layer])
                        s_veasiroc_adc_lo[layer]->Fill(ch, adc);
                }
            }
	    */
        }


        // BGO
        // up
        for (int seg=0; seg<NumOfSegBGO; ++seg) {
            int nhits_tdc = g_unpacker.get_entries(DetIdBGO,0, seg, 0, 1);
            for (int ihit=0; ihit<nhits_tdc; ++ihit) {
                int tdc=g_unpacker.get(DetIdBGO, 0, seg, 0, 1, ihit);
                if(s_BGO_T[seg])
                    s_BGO_T[seg]->Fill(tdc);
            }

            int nhits_adc = g_unpacker.get_entries(DetIdBGO,0, seg, 0, 0);
            for (int ihit=0; ihit<nhits_adc; ++ihit) {
                int adc=g_unpacker.get(DetIdBGO, 0, seg, 0, 0, ihit);
                if(s_BGO_A[seg])
                    s_BGO_A[seg]->Fill(adc);
            }


	    // BGO Flash ADC
            int nhits_fadc = g_unpacker.get_entries(DetIdBGO,0, seg, 0, 2);
	    if(nhits_fadc>0){
	    }
	    // (i_sample,data)
            for (int ihit=0; ihit<nhits_fadc; ++ihit) {
	      int fadc=g_unpacker.get(DetIdBGO, 0, seg, 0, 2, ihit);
	      if(s_BGO_FA[seg]){		  
		//s_BGO_FA[seg]->Fill( ihit+1, fadc - s_BGO_ped[seg] );
		s_BGO_FA[seg]->Fill( ihit+1, fadc);
	      }
            }
	    //####fitting -> (height,time)#####
	    //CConfig *CC;
	    Char_t **file;
	    char name[100];
	    sprintf(name,"");file = (Char_t **)name;

        }


        // Correlation of BGO
        for(int seg = 0; seg < NumOfSegBGO; ++seg) {
            std::vector<int> adcs;
            //for(int ch = 0; ch < 2; ++ch) {
                int nhits_adc = g_unpacker.get_entries(DetIdBGO, 0, seg, 0, 0);
                if(nhits_adc) {
                    int adc = g_unpacker.get(DetIdBGO, 0, seg, 0, 0, 0);
                    adcs.push_back(adc);
                }
		// }
            if(adcs.size() == 2) {
                s_BGO_Cor->Fill(adcs.at(0), adcs.at(1));
            }
        }

        // HitPattern of BGO
	TH1* h_hist_bgo = s_HitPatBGO;
        for(int seg = 0; seg < NumOfSegBGO; ++seg) {
	  //for(int ch = 0; ch < 2; ++ch) {
	    int nhits_tdc = g_unpacker.get_entries(DetIdBGO, 0, seg, 0, 1);
	    if(nhits_tdc==0) continue;
	    unsigned int val = g_unpacker.get(DetIdBGO, 0, seg, 0, 1);
	    if(val!=0)
	      {
		h_hist_bgo->Fill(seg);
	      }
	    //	}
        }
        // tree
        TTree *tree;
        tree = s_tree;
        tree->Fill();
#if DEBUG
        std::cout << "#D " << __FILE__ << " " << __LINE__ <<std::endl;
#endif
	
	//getchar();
        return 0;
    }
}


