/// OnlMonTrigNim.C
#include <sstream>
#include <iomanip>
#include <TStyle.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQHitVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>
#include <geom_svc/GeomSvc.h>
#include <UtilAna/UtilSQHit.h>
#include <UtilAna/UtilHist.h>
#include "OnlMonTrigEP.h"
#include "OnlMonTrigNim.h"
using namespace std;

/** 
 * NIM1: H1234YT || H1234YB
 * NIM2: H1234XT || H1234XB
 * NIM3: H1234XT && H1234XB
 */
OnlMonTrigNim::OnlMonTrigNim()
{
  NumCanvases(1);
  Name("OnlMonTrigNim");
  Title("NIM Purity/Efficiency" );//  Title("NIM Trigger");
}

int OnlMonTrigNim::InitOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonTrigNim::InitRunOnlMon(PHCompositeNode* topNode)
{
  h1_cnt = new TH1D("h1_cnt", ";;Statistics", 10, 0.5, 10.5);
  h1_cnt->GetXaxis()->SetBinLabel( 1, "Analyzed");
  h1_cnt->GetXaxis()->SetBinLabel( 2, "Emu. X T");
  h1_cnt->GetXaxis()->SetBinLabel( 3, "Emu. X B");
  h1_cnt->GetXaxis()->SetBinLabel( 4, "Emu. Y T");
  h1_cnt->GetXaxis()->SetBinLabel( 5, "Emu. Y B");
  h1_cnt->GetXaxis()->SetBinLabel( 6, "Emu. NIM1");
  h1_cnt->GetXaxis()->SetBinLabel( 7, "Emu. NIM2");
  h1_cnt->GetXaxis()->SetBinLabel( 8, "Emu. NIM3");
  h1_cnt->GetXaxis()->SetBinLabel( 9, "FPGA1,NIM2,NIM3");
  h1_cnt->GetXaxis()->SetBinLabel(10, "NIM1");
  RegisterHist(h1_cnt);

  h2_purity = new TH2D("h2_purity", "", 3, 0.5, 3.5,  2, -0.5, 1.5); // X = NIM1...5, Y = NG/OK
  h2_eff    = new TH2D("h2_eff"   , "", 3, 0.5, 3.5,  2, -0.5, 1.5); // X = NIM1...5, Y = NG/OK
  RegisterHist(h2_purity);
  RegisterHist(h2_eff);

  h2_count = new TH2D("h2_count", "N of events in which each plane has hit;;", 8, 0.5, 8.5,  10, 0.5, 10.5);
  for (int ii = 1; ii <= 5; ii++) {
    ostringstream oss;
    oss << "NIM " << ii;
    h2_count->GetYaxis()->SetBinLabel(ii, oss.str().c_str());
    oss.str("");
    oss << "FPGA " << ii;
    h2_count->GetYaxis()->SetBinLabel(ii + 5, oss.str().c_str());
  }
  h2_count->GetXaxis()->SetBinLabel(1, "H1X");
  h2_count->GetXaxis()->SetBinLabel(2, "H1Y");
  h2_count->GetXaxis()->SetBinLabel(3, "H2X");
  h2_count->GetXaxis()->SetBinLabel(4, "H2Y");
  h2_count->GetXaxis()->SetBinLabel(5, "H3X");
  h2_count->GetXaxis()->SetBinLabel(6, "H4X");
  h2_count->GetXaxis()->SetBinLabel(7, "H4Y1");
  h2_count->GetXaxis()->SetBinLabel(8, "H4Y2");

  RegisterHist(h2_count);

  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonTrigNim::ProcessEventOnlMon(PHCompositeNode* topNode)
{
  SQEvent*     event   = findNode::getClass<SQEvent    >(topNode, "SQEvent");
  SQHitVector* hit_vec = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
  if (!event || !hit_vec) return Fun4AllReturnCodes::ABORTEVENT;

  bool trig_fpga1 = event->get_trigger(SQEvent::MATRIX1);
  bool trig_nim1  = event->get_trigger(SQEvent::NIM1);
  bool trig_nim2  = event->get_trigger(SQEvent::NIM2);
  bool trig_nim3  = event->get_trigger(SQEvent::NIM3);
  if (! (trig_fpga1 || trig_nim1 || trig_nim2 || trig_nim3)) return Fun4AllReturnCodes::EVENT_OK;
  h1_cnt->AddBinContent(1);

  auto hv_h1b   = UtilSQHit::FindHitsFast(event, hit_vec, "H1B");
  auto hv_h1t   = UtilSQHit::FindHitsFast(event, hit_vec, "H1T");
  auto hv_h1l   = UtilSQHit::FindHitsFast(event, hit_vec, "H1L");
  auto hv_h1r   = UtilSQHit::FindHitsFast(event, hit_vec, "H1R");
  auto hv_h2b   = UtilSQHit::FindHitsFast(event, hit_vec, "H2B");
  auto hv_h2t   = UtilSQHit::FindHitsFast(event, hit_vec, "H2T");
  auto hv_h2l   = UtilSQHit::FindHitsFast(event, hit_vec, "H2L");
  auto hv_h2r   = UtilSQHit::FindHitsFast(event, hit_vec, "H2R");
  auto hv_h3b   = UtilSQHit::FindHitsFast(event, hit_vec, "H3B");
  auto hv_h3t   = UtilSQHit::FindHitsFast(event, hit_vec, "H3T");
  auto hv_h4b   = UtilSQHit::FindHitsFast(event, hit_vec, "H4B");
  auto hv_h4t   = UtilSQHit::FindHitsFast(event, hit_vec, "H4T");
  auto hv_h4y1l = UtilSQHit::FindHitsFast(event, hit_vec, "H4Y1L");
  auto hv_h4y1r = UtilSQHit::FindHitsFast(event, hit_vec, "H4Y1R");
  auto hv_h4y2l = UtilSQHit::FindHitsFast(event, hit_vec, "H4Y2L");
  auto hv_h4y2r = UtilSQHit::FindHitsFast(event, hit_vec, "H4Y2R");
  
  int n_x1b = CountInTimeHits(hv_h1b);
  int n_x1t = CountInTimeHits(hv_h1t);
  int n_x2b = CountInTimeHits(hv_h2b);
  int n_x2t = CountInTimeHits(hv_h2t);
  int n_x3b = CountInTimeHits(hv_h3b);
  int n_x3t = CountInTimeHits(hv_h3t);
  int n_x4b = CountInTimeHits(hv_h4b);
  int n_x4t = CountInTimeHits(hv_h4t);
  bool emu_xt = n_x1t > 0 && n_x2t > 0 && n_x3t > 0 && n_x4t > 0;
  bool emu_xb = n_x1b > 0 && n_x2b > 0 && n_x3b > 0 && n_x4b > 0;
  bool emu_2 = emu_xt || emu_xb;
  bool emu_3 = emu_xt && emu_xb;
  
  int n_y1b  = CountInTimeHits(hv_h1l  ,  1, 10) + CountInTimeHits(hv_h1r  ,  1, 10);
  int n_y1t  = CountInTimeHits(hv_h1l  , 11, 20) + CountInTimeHits(hv_h1r  , 11, 20);
  int n_y2b  = CountInTimeHits(hv_h2l  ,  1,  9) + CountInTimeHits(hv_h2r  ,  1,  9);
  int n_y2t  = CountInTimeHits(hv_h2l  , 11, 19) + CountInTimeHits(hv_h2r  , 11, 19);
  int n_y41b = CountInTimeHits(hv_h4y1l,  1,  8) + CountInTimeHits(hv_h4y1r,  1,  8);
  int n_y41t = CountInTimeHits(hv_h4y1l,  9, 16) + CountInTimeHits(hv_h4y1r,  9, 16);
  int n_y42b = CountInTimeHits(hv_h4y2l,  1,  8) + CountInTimeHits(hv_h4y2r,  1,  8);
  int n_y42t = CountInTimeHits(hv_h4y2l,  9, 16) + CountInTimeHits(hv_h4y2r,  9, 16);
  bool emu_yt = n_y1t > 0 && n_y2t > 0 && n_y41t > 0 && n_y42t > 0;
  bool emu_yb = n_y1b > 0 && n_y2b > 0 && n_y41b > 0 && n_y42b > 0;
  bool emu_1 = emu_yt || emu_yb;

  if (emu_xt) h1_cnt->AddBinContent(2);
  if (emu_xb) h1_cnt->AddBinContent(3);
  if (emu_yt) h1_cnt->AddBinContent(4);
  if (emu_yb) h1_cnt->AddBinContent(5);
  if (emu_1 ) h1_cnt->AddBinContent(6);
  if (emu_2 ) h1_cnt->AddBinContent(7);
  if (emu_3 ) h1_cnt->AddBinContent(8);
  
  /// Check the purity
  if (trig_nim1) h2_purity->Fill(1, (emu_1 ? 1 : 0));
  if (trig_nim2) h2_purity->Fill(2, (emu_2 ? 1 : 0));
  if (trig_nim3) h2_purity->Fill(3, (emu_3 ? 1 : 0));

  /// Check the efficiency
  int trig_bits_raw = OnlMonTrigEP::EmulateRawTriggerBits(hit_vec);
  if (trig_fpga1 || trig_nim2 || trig_nim3) { // X triggers
    h1_cnt->AddBinContent(9);
    if (emu_1) h2_eff->Fill(1, (trig_bits_raw & (0x1 << SQEvent::NIM1) ? 1 : 0));
  }
  if (trig_nim1) { // Y triggers
    h1_cnt->AddBinContent(10);
    if (emu_2) h2_eff->Fill(2, (trig_bits_raw & (0x1 << SQEvent::NIM2) ? 1 : 0));
    if (emu_3) h2_eff->Fill(3, (trig_bits_raw & (0x1 << SQEvent::NIM3) ? 1 : 0));
  }
    
//    bool hit_h1x  = hv_h1b->size() > 0  ||  hv_h1t->size() > 0;
//    bool hit_h1y  = hv_h1l->size() > 0  ||  hv_h1r->size() > 0;
//    bool hit_h2x  = hv_h2b->size() > 0  ||  hv_h2t->size() > 0;
//    bool hit_h2y  = hv_h2l->size() > 0  ||  hv_h2r->size() > 0;
//    bool hit_h3x  = hv_h3b->size() > 0  ||  hv_h3t->size() > 0;
//    bool hit_h4x  = hv_h4b->size() > 0  ||  hv_h4t->size() > 0;
//    bool hit_h4y1 = hv_h4y1l->size() > 0  ||  hv_h4y1r->size() > 0;
//    bool hit_h4y2 = hv_h4y2l->size() > 0  ||  hv_h4y2r->size() > 0;
    
    /// Fill the histogram per trigger
//  bool trig_flag[10] = {
//    event->get_trigger(SQEvent::NIM1),
//    event->get_trigger(SQEvent::NIM2),
//    event->get_trigger(SQEvent::NIM3),
//    event->get_trigger(SQEvent::NIM4),
//    event->get_trigger(SQEvent::NIM5),
//    event->get_trigger(SQEvent::MATRIX1),
//    event->get_trigger(SQEvent::MATRIX2),
//    event->get_trigger(SQEvent::MATRIX3),
//    event->get_trigger(SQEvent::MATRIX4),
//    event->get_trigger(SQEvent::MATRIX5)
//  };
//  for (int i_trig = 0; i_trig < 10; i_trig++) {
//    if (! trig_flag[i_trig]) continue;
//    h2_count->Fill(0.0, i_trig+1); // Count N of all events in the underflow bin
//    if (hit_h1x ) h2_count->Fill(1, i_trig+1);
//    if (hit_h1y ) h2_count->Fill(2, i_trig+1);
//    if (hit_h2x ) h2_count->Fill(3, i_trig+1);
//    if (hit_h2y ) h2_count->Fill(4, i_trig+1);
//    if (hit_h3x ) h2_count->Fill(5, i_trig+1);
//    if (hit_h4x ) h2_count->Fill(6, i_trig+1);
//    if (hit_h4y1) h2_count->Fill(7, i_trig+1);
//    if (hit_h4y2) h2_count->Fill(8, i_trig+1);
//  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonTrigNim::EndOnlMon(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int OnlMonTrigNim::FindAllMonHist()
{
  h1_cnt = FindMonHist("h1_cnt");
  if (! h1_cnt) return 1; 

  h2_purity = (TH2*)FindMonHist("h2_purity");
  if (! h2_purity) return 1;

  h2_eff = (TH2*)FindMonHist("h2_eff");
  if (! h2_eff) return 1;

  //h2_count = (TH2*)FindMonHist("h2_count");
  //return (h2_count  ?  0  :  1);
  return 0;
}

int OnlMonTrigNim::DrawMonitor()
{
  OnlMonCanvas* can0 = GetCanvas(0);
  TPad* pad0 = can0->GetMainPad();
  pad0->Divide(1, 3);

  TVirtualPad* pad01 = pad0->cd(1);
  pad01->SetGrid();
  h1_cnt->Draw();
  TText text;
  text.SetNDC(true);
  text.SetTextAlign(22);
  text.DrawText(0.20, 0.95, "NIM1 = Y1234T || Y1234B");
  text.DrawText(0.50, 0.95, "NIM2 = X1234T || X1234B");
  text.DrawText(0.80, 0.95, "NIM3 = X1234T && X1234B");

  TVirtualPad* pad02 = pad0->cd(2);
  pad02->SetGrid();
  TH1* h1_pur_all = h2_purity->ProjectionX("h1_pur_all");
  TH1* h1_pur_ok  = h2_purity->ProjectionX("h1_pur_ok", 2, 2);
  TGraphAsymmErrors* gr_pur = new TGraphAsymmErrors(h1_pur_ok, h1_pur_all);
  gr_pur->SetTitle(";NIM;Purity");
  gr_pur->SetMarkerStyle(21);
  gr_pur->SetMarkerColor(kBlue);
  gr_pur->SetLineColor  (kBlue);
  gr_pur->Draw("AP");
  gr_pur->GetYaxis()->SetRangeUser(0, 1);

  TVirtualPad* pad03 = pad0->cd(3);
  pad03->SetGrid();
  TH1* h1_eff_all = h2_eff->ProjectionX("h1_eff_all");
  TH1* h1_eff_ok  = h2_eff->ProjectionX("h1_eff_ok", 2, 2);
  TGraphAsymmErrors* gr_eff = new TGraphAsymmErrors(h1_eff_ok, h1_eff_all);
  gr_eff->SetTitle(";NIM;Efficiency");
  gr_eff->SetMarkerStyle(21);
  gr_eff->SetMarkerColor(kBlue);
  gr_eff->SetLineColor  (kBlue);
  gr_eff->Draw("AP");
  gr_eff->GetYaxis()->SetRangeUser(0, 1);

  if (h1_pur_all->Integral() > 100 && h1_eff_all->Integral() > 100) {
    can0->SetStatus(OnlMonCanvas::OK);
  } else {
    can0->AddMessage("No event.");
    can0->SetStatus(OnlMonCanvas::WARN);
  }

//  static TH2* h2_rate = 0;
//  if (! h2_rate) delete h2_rate;
//  h2_rate = (TH2*)h2_count->Clone("h2_rate");
//  h2_rate->SetTitle("Percent of events in which each plane has hit");
//  for (int iy = 1; iy <= h2_rate->GetNbinsY(); iy++) {
//    double cont0 = h2_rate->GetBinContent(0, iy); // underflow bin
//    if (cont0 <= 0) continue;
//    for (int ix = 1; ix <= h2_rate->GetNbinsX(); ix++) {
//      double cont = h2_rate->GetBinContent(ix, iy);
//      h2_rate->SetBinContent(ix, iy, 100 * cont / cont0);
//    }
//  }
//  h2_rate->GetZaxis()->SetRangeUser(0, 100);
//
//  OnlMonCanvas* can0 = GetCanvas(0);
//  TPad* pad0 = can0->GetMainPad();
//  pad0->Divide(1, 2);
//
//  TVirtualPad* pad01 = pad0->cd(1);
//  pad01->SetGrid();
//  h2_count->Draw("colz");

  //TVirtualPad* pad02 = pad0->cd(2);
  //pad02->SetGrid();
  //h2_rate->Draw("colz");
  //h2_rate->SetMarkerSize(2.0); // = text size (1.0 by default)
  //gStyle->SetPaintTextFormat("3.0f");
  //h2_rate->Draw("TEXTsame");

//  if (h2_count->Integral() == 0) {
//    can0->AddMessage("No event.");
//    can0->SetStatus(OnlMonCanvas::WARN);
//  } else {
//    can0->SetStatus(OnlMonCanvas::OK);
//  }

  return 0;
}

int OnlMonTrigNim::CountInTimeHits(std::vector<SQHit*>* vec, const int ele_lo, const int ele_hi)
{
  int cnt = 0;
  for (auto it = vec->begin(); it != vec->end(); it++) {
    if (! (*it)->is_in_time()) continue;
    int ele = (*it)->get_element_id();
    if (ele_lo <= ele && ele <= ele_hi) cnt++;
  }
  return cnt;
}
