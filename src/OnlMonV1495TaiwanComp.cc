/// OnlMonV1495TaiwanComp.C
#include <sstream>
#include <iomanip>
#include <set>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <interface_main/SQRun.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQHitVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>
#include <geom_svc/GeomSvc.h>
#include <UtilAna/UtilSQHit.h>
#include <UtilAna/UtilHist.h>
#include "OnlMonParam.h"
#include "OnlMonV1495TaiwanComp.h"
using namespace std;

OnlMonV1495TaiwanComp::OnlMonV1495TaiwanComp()
  : m_trig_mask(1023) // 1023 = 0b1111111111 = all triggers
  , m_list_det_name{ "H1T", "H1B", "H2T", "H2B", "H3T", "H3B", "H4T", "H4B" }
{
  NumCanvases(1);
  Name("OnlMonV1495TaiwanComp");
  Title("V1495 vs Taiwan TDCs");
}

int OnlMonV1495TaiwanComp::InitOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonV1495TaiwanComp::InitRunOnlMon(PHCompositeNode* topNode)
{
  h1_cnt = new TH1D("h1_cnt", ";Type;Count", 15, 0.5, 15.5);
  RegisterHist(h1_cnt);

  //OnlMonParam param(this);
  //m_trig_mask = param.GetIntParam("TRIGGER_MASK");

  ostringstream oss;
  GeomSvc* geom = GeomSvc::instance();
  for (auto it = m_list_det_name.begin(); it != m_list_det_name.end(); it++) {
    string det_name = *it;
    int    det_id   = geom->getDetectorID(det_name);
    int    n_ele    = geom->getPlaneNElements(det_id);
    m_list_det_id.push_back(det_id);
    //cout << det_name << " " << det_id << " " << n_ele << endl;

    oss.str("");
    oss << "h2_comp_" << det_id;
    TH2* h2 = new TH2D(oss.str().c_str(), "", n_ele, 0.5, n_ele+0.5,  3, 0.5, 3.5);
    oss.str("");
    oss << det_name << ";Element ID;Hit comparison status";
    h2->SetTitle(oss.str().c_str());
    h2_comp.push_back(h2);
    RegisterHist(h2);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonV1495TaiwanComp::ProcessEventOnlMon(PHCompositeNode* topNode)
{
  SQEvent*            evt = findNode::getClass<SQEvent    >(topNode, "SQEvent");
  SQHitVector*    hit_vec = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
  SQHitVector* tr_hit_vec = findNode::getClass<SQHitVector>(topNode, "SQTriggerHitVector");
  if (!evt || !hit_vec || !tr_hit_vec) return Fun4AllReturnCodes::ABORTEVENT;

  h1_cnt->AddBinContent(1);
  
  //unsigned short trig_bits = evt->get_trigger();
  //if (! (trig_bits & m_trig_mask)) return Fun4AllReturnCodes::EVENT_OK;

  h1_cnt->AddBinContent(2);

  unsigned int n_det = m_list_det_id.size();
  for (unsigned int i_det = 0; i_det < n_det; i_det++) {
    int det_id = m_list_det_id[i_det];
    set<int> hit_ele_id_set;
    auto hits = UtilSQHit::FindHitsFast(evt, hit_vec, det_id);
    for (auto it = hits->begin(); it != hits->end(); it++) {
      if ((*it)->is_in_time()) hit_ele_id_set.insert((*it)->get_element_id());
    }

    set<int> tr_hit_ele_id_set;
    auto tr_hits = UtilSQHit::FindTriggerHitsFast(evt, tr_hit_vec, det_id);
    for (auto it = tr_hits->begin(); it != tr_hits->end(); it++) {
      if ((*it)->is_in_time()) tr_hit_ele_id_set.insert((*it)->get_element_id());
    }

    for (auto it = hit_ele_id_set.begin(); it != hit_ele_id_set.end(); it++) {
      int ele_id = *it;
      auto it_tr = tr_hit_ele_id_set.find(ele_id);
      if (it_tr != tr_hit_ele_id_set.end()) {
        h2_comp[i_det]->Fill(ele_id, HIT_BOTH);
        tr_hit_ele_id_set.erase(it_tr);
      } else {
        h2_comp[i_det]->Fill(ele_id, HIT_TAIWAN);
      }
    }

    for (auto it = tr_hit_ele_id_set.begin(); it != tr_hit_ele_id_set.end(); it++) {
      h2_comp[i_det]->Fill(*it, HIT_V1495);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonV1495TaiwanComp::EndOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonV1495TaiwanComp::FindAllMonHist()
{
  h1_cnt = FindMonHist("h1_cnt");
  if (! h1_cnt) return 1;

  ostringstream oss;
  for (auto it = m_list_det_id.begin(); it != m_list_det_id.end(); it++) {
    oss.str("");
    oss << "h2_comp_" << *it;
    TH2* h2 = (TH2*)FindMonHist(oss.str().c_str());
    if (! h2) return 1;
    h2_comp.push_back(h2);
  }

  return 0;
}

int OnlMonV1495TaiwanComp::DrawMonitor()
{
  cout << "DrawMonitor()" << endl;

  OnlMonCanvas* can0 = GetCanvas(0);
  TPad* pad0 = can0->GetMainPad();

  unsigned int n_det = m_list_det_id.size();
  pad0->Divide(2, n_det/2);

  int n_ng_v1495  = 0;
  int n_ng_taiwan = 0;
  for (unsigned int i_det = 0; i_det < n_det; i_det++) {
    string det_name = m_list_det_name[i_det];
    bool is_H1 = det_name.substr(0, 2) == "H1";
    TVirtualPad* pad01 = pad0->cd(i_det+1);
    pad01->SetMargin(0.1, 0.02, 0.1, 0.1); // (l, r, t, b)
    pad01->SetGrid();
    DrawStatusPlot(h2_comp[i_det], is_H1, pad01, n_ng_v1495, n_ng_taiwan);
  }
  
  if (n_ng_v1495 == 0 && n_ng_taiwan == 0) {
    can0->SetStatus(OnlMonCanvas::OK);
  } else {
    can0->SetStatus(OnlMonCanvas::WARN);
    if (n_ng_v1495  > 0) can0->AddMessage(TString::Format("Event frac. > 0.3 on %d elements of V1495 TDCs.", n_ng_v1495));
    if (n_ng_taiwan > 0) can0->AddMessage(TString::Format("Event frac. > 0.3 on %d elements of Taiwan TDCs.", n_ng_taiwan));
  }
  return 0;
}

void OnlMonV1495TaiwanComp::DrawStatusPlot(TH2* h2, const bool is_H1, TVirtualPad* pad, int& n_ng_v1495, int& n_ng_taiwan)
{
  TH1* h1_all    = h2->ProjectionX("h1_all"   , HIT_BOTH  , HIT_TAIWAN);
  TH1* h1_v1495  = h2->ProjectionX("h1_v1495" , HIT_V1495 , HIT_V1495 );
  TH1* h1_taiwan = h2->ProjectionX("h1_taiwan", HIT_TAIWAN, HIT_TAIWAN);
  TGraphAsymmErrors* eff_v1495  = new TGraphAsymmErrors(h1_v1495 , h1_all);
  TGraphAsymmErrors* eff_taiwan = new TGraphAsymmErrors(h1_taiwan, h1_all);
  eff_v1495 ->SetMarkerStyle(7);
  eff_taiwan->SetMarkerStyle(7);

  int i_lo = 0;
  int i_hi = eff_v1495->GetN();
  if (is_H1) {
    i_lo += 4;
    i_hi -= 4;
  }
  for (int ii = i_lo; ii < i_hi; ii++) {
    eff_v1495 ->SetPointEXlow (ii, 0);
    eff_v1495 ->SetPointEXhigh(ii, 0);
    eff_taiwan->SetPointEXlow (ii, 0);
    eff_taiwan->SetPointEXhigh(ii, 0);
    double v_v1495  = eff_v1495 ->GetX()[ii];
    double v_taiwan = eff_taiwan->GetX()[ii];
    if (v_v1495  > 0.3) n_ng_taiwan++; // Not typo.
    if (v_taiwan > 0.3) n_ng_v1495++;  // Not typo.
  }
  eff_taiwan->SetLineColor  (kBlue);
  eff_taiwan->SetMarkerColor(kBlue);

  string title = h2->GetTitle();
  title += ";Element ID;Event fraction";
  //int n_ele = h2->GetNbinsX();
  TH1* fr = pad->DrawFrame(i_lo+0.5, 0.0,  i_hi+0.5, 1.0, title.c_str());
  eff_v1495 ->Draw("Psame");
  eff_taiwan->Draw("Psame");

  TText text;
  text.SetNDC(true);
  text.DrawText(0.7, 0.95, "No hit on Taiwan TDC");
  text.SetTextColor(kBlue);
  text.DrawText(0.7, 0.90, "No hit on V1495 TDC");
}
