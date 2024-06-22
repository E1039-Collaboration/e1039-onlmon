/// OnlMonV1495TaiwanComp.C
#include <sstream>
#include <iomanip>
#include <set>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
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
    TH1* h2 = FindMonHist(oss.str().c_str());
    if (! h2) return 1;
    h2_comp.push_back(h2);
  }

  return 0;
}

int OnlMonV1495TaiwanComp::DrawMonitor()
{
  OnlMonCanvas* can0 = GetCanvas(0);
  TPad* pad0 = can0->GetMainPad();

  unsigned int n_det = m_list_det_id.size();
  pad0->Divide(2, n_det/2);

  for (unsigned int i_det = 0; i_det < n_det; i_det++) {
    //int det_id = m_list_det_id[i_det];

    TVirtualPad* pad01 = pad0->cd(2*i_det+1);
    pad01->SetGrid();
    h2_comp[i_det]->Draw("colz");
  }
  //can0->SetStatus(OnlMonCanvas::OK);
  //int n_evt_all = h1_cnt->GetBinContent(1);
  //int n_evt_ana = h1_cnt->GetBinContent(2);
  //can0->AddMessage(TString::Format("N of analyzed events = %d.", n_evt_ana));

  return 0;
}
