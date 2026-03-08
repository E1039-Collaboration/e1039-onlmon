/// OnlMonRecoGpu.C
#include <sstream>
#include <iomanip>
#include <TSystem.h>
#include <TChain.h>
#include <TH1D.h>
#include <TH2D.h>
//#include <TProfile.h>
#include <interface_main/SQRun.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQHitVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>
//#include <geom_svc/GeomSvc.h>
//#include <UtilAna/UtilSQHit.h>
//#include <UtilAna/UtilHist.h>
#include "OnlMonRecoGpu.h"
using namespace std;

OnlMonRecoGpu::OnlMonRecoGpu()
{
  NumCanvases(2);
  Name("OnlMonRecoGpu" );
  Title("GPU Reco");
}

int OnlMonRecoGpu::InitOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonRecoGpu::InitRunOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonRecoGpu::ProcessEventOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonRecoGpu::EndOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonRecoGpu::FindAllMonHist()
{
  return 0;
}

int OnlMonRecoGpu::DrawMonitor()
{
  int run_id, spill_id, event_id, spill_id_min, spill_id_max;
  GetBasicID(&run_id, &spill_id, &event_id, &spill_id_min, &spill_id_max);
  int n_evt, n_sp;
  GetBasicCount(&n_evt, &n_sp);
  cout << "DrawMonitor(): " << run_id << " " << spill_id  << " " << event_id  << " " << spill_id_min  << " " << spill_id_max  << " " << n_evt  << " " << n_sp << endl;

  TH1* h1_trk_n     = new TH1D("h1_trk_n"    , "", 10, -0.5, 9.5); // ntracks
  TH1* h1_trk_nhits = new TH1D("h1_trk_nhits", "", 10, 8.5, 18.5); // track.nhits
  TH1* h1_trk_chi2  = new TH1D("h1_trk_chi2" , "", 100, 0, 10); // track.chi2
  TH1* h1_trk_px    = new TH1D("h1_trk_px"   , "", 100, -5, 5); // track.px
  TH1* h1_trk_py    = new TH1D("h1_trk_py"   , "", 100, -5, 5); // track.py
  TH1* h1_trk_pz    = new TH1D("h1_trk_pz"   , "", 100, 10, 110); // track.pz
  h1_trk_n    ->SetTitle(";N of tracks/event;N of events");
  h1_trk_nhits->SetTitle(";N of hits/track;N of tracks");
  h1_trk_chi2 ->SetTitle(";Track #chi^{2};N of tracks");
  h1_trk_px   ->SetTitle(";Track p_{x} (GeV);N of tracks");
  h1_trk_py   ->SetTitle(";Track p_{y} (GeV);N of tracks");
  h1_trk_pz   ->SetTitle(";Track p_{z} (GeV);N of tracks");
  
  TH1* h1_dim_n    = new TH1D("h1_dim_n"   , "", 10, -0.5, 9.5); // ndimuons
  TH1* h1_dim_vz   = new TH1D("h1_dim_vz"  , "", 100, -500, 500); // dimuon.vz
  TH1* h1_dim_mass = new TH1D("h1_dim_mass", "", 100, 0, 10); // dimuon.mass
  TH1* h1_dim_px   = new TH1D("h1_dim_px"  , "", 100, -5, 5); // dimuon.px
  TH1* h1_dim_py   = new TH1D("h1_dim_py"  , "", 100, -5, 5); // dimuon.py
  TH1* h1_dim_pz   = new TH1D("h1_dim_pz"  , "", 100, 10, 110); // dimuon.pz
  h1_dim_n   ->SetTitle(";N of dimuons/event;N of events");
  h1_dim_vz  ->SetTitle(";Dimuon z_{vertex};N of dimuons");
  h1_dim_mass->SetTitle(";Dimuon mass (GeV);N of dimuons");
  h1_dim_px  ->SetTitle(";Dimuon p_{x} (GeV);N of dimuons");
  h1_dim_py  ->SetTitle(";Dimuon p_{y} (GeV);N of dimuons");
  h1_dim_pz  ->SetTitle(";Dimuon p_{z} (GeV);N of dimuons");

  // Data-file location:  /data2/users/utsav/work/gpu_recon/run_006155/run_006155_spill_001941907/out_*.root
  const string DIR_DAT = "/data2/users/utsav/work/gpu_recon";
  TChain* chain = new TChain("tree");
  // spill_id_min is often too small when the spill info is not available at the beginning of run.
  // Thus sp starts at "spill_id_max-2*n_sp" as a temporary solution...
  for (int sp = spill_id_max-2*n_sp; sp <= spill_id_max; sp++) {
    ostringstream oss;
    oss << DIR_DAT << setfill('0') << "/run_" << setw(6) << run_id << "/run_" << setw(6) << run_id << "_spill_" << setw(9) << sp << "/out_" << run_id << "_" << sp << "_recal.root";
    string fname = oss.str();
    cout << "  " << fname;
    if (gSystem->AccessPathName(fname.c_str())) {
      cout << ":  Not found." << endl;
    } else {
      cout << ":  Found." << endl;
      chain->Add(fname.c_str());
    }
  }
  
  chain->Project("h1_trk_n"    , "ndimuons");
  chain->Project("h1_trk_nhits", "track.nhits");
  chain->Project("h1_trk_chi2" , "track.chi2");
  chain->Project("h1_trk_px"   , "track.px");
  chain->Project("h1_trk_py"   , "track.py");
  chain->Project("h1_trk_pz"   , "track.pz");

  chain->Project("h1_dim_n"   , "ndimuons");
  chain->Project("h1_dim_vz"  , "dimuon.vz");
  chain->Project("h1_dim_mass", "dimuon.mass");
  chain->Project("h1_dim_px"  , "dimuon.px");
  chain->Project("h1_dim_py"  , "dimuon.py");
  chain->Project("h1_dim_pz"  , "dimuon.pz");

  delete chain;
  
  OnlMonCanvas* can0 = GetCanvas(0);
  //can0->SetStatus(OnlMonCanvas::OK);
  TPad* pad0 = can0->GetMainPad();
  pad0->Divide(2, 3);
  pad0->cd(1);  h1_trk_n    ->Draw();
  pad0->cd(2);  h1_trk_nhits->Draw();
  pad0->cd(3);  h1_trk_chi2 ->Draw();
  pad0->cd(4);  h1_trk_px   ->Draw();
  pad0->cd(5);  h1_trk_py   ->Draw();
  pad0->cd(6);  h1_trk_pz   ->Draw();

  OnlMonCanvas* can1 = GetCanvas(1);
  //can1->SetStatus(OnlMonCanvas::OK);
  TPad* pad1 = can1->GetMainPad();
  pad1->Divide(2, 3);
  pad1->cd(1);  h1_dim_n   ->Draw();
  pad1->cd(2);  h1_dim_vz  ->Draw();
  pad1->cd(3);  h1_dim_mass->Draw();
  pad1->cd(4);  h1_dim_px  ->Draw();
  pad1->cd(5);  h1_dim_py  ->Draw();
  pad1->cd(6);  h1_dim_pz  ->Draw();

  return 0;
}
