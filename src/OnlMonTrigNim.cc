/// OnlMonTrigNim.C
#include <sstream>
#include <iomanip>
#include <TStyle.h>
#include <TH2D.h>
#include <THStack.h>
#include <TLegend.h>
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
  h1_cnt->GetXaxis()->SetBinLabel( 9, "FPGA1,NIM2,3");
  h1_cnt->GetXaxis()->SetBinLabel(10, "NIM1");
  RegisterHist(h1_cnt);

  h2_purity = new TH2D("h2_purity", "", 3, 0.5, 3.5,  2, -0.5, 1.5); // X = NIM1...3, Y = NG/OK
  h2_eff    = new TH2D("h2_eff"   , "", 3, 0.5, 3.5,  2, -0.5, 1.5); // X = NIM1...3, Y = NG/OK
  RegisterHist(h2_purity);
  RegisterHist(h2_eff);

  h2_purity_ng = new TH2D("h2_purity_ng", "", 3, 0.5, 3.5,  8, 0.5, 8.5); // X = NIM1...3, Y = planes
  h2_eff_ng    = new TH2D("h2_eff_ng"   , "", 3, 0.5, 3.5,  3, 0.5, 3.5); // X = NIM1...3, Y = triggers
  RegisterHist(h2_purity_ng);
  RegisterHist(h2_eff_ng);

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
  if (trig_nim1) {
    h2_purity->Fill(1, (emu_1 ? 1 : 0));
    if (! emu_1) {
      h2_purity_ng->Fill(1, 0); // Total count in underflow bin
      if (n_y1t  == 0) h2_purity_ng->Fill(1, 1);
      if (n_y2t  == 0) h2_purity_ng->Fill(1, 2);
      if (n_y41t == 0) h2_purity_ng->Fill(1, 3);
      if (n_y42t == 0) h2_purity_ng->Fill(1, 4);
      if (n_y1b  == 0) h2_purity_ng->Fill(1, 5);
      if (n_y2b  == 0) h2_purity_ng->Fill(1, 6);
      if (n_y41b == 0) h2_purity_ng->Fill(1, 7);
      if (n_y42b == 0) h2_purity_ng->Fill(1, 8);
    }
  }
  if (trig_nim2) {
    h2_purity->Fill(2, (emu_2 ? 1 : 0));
    if (! emu_2) {
      h2_purity_ng->Fill(2, 0); // Total count in underflow bin
      if (n_x1t == 0) h2_purity_ng->Fill(2, 1);
      if (n_x2t == 0) h2_purity_ng->Fill(2, 2);
      if (n_x3t == 0) h2_purity_ng->Fill(2, 3);
      if (n_x4t == 0) h2_purity_ng->Fill(2, 4);
      if (n_x1b == 0) h2_purity_ng->Fill(2, 5);
      if (n_x2b == 0) h2_purity_ng->Fill(2, 6);
      if (n_x3b == 0) h2_purity_ng->Fill(2, 7);
      if (n_x4b == 0) h2_purity_ng->Fill(2, 8);
    }
  }
  if (trig_nim3) {
    h2_purity->Fill(3, (emu_3 ? 1 : 0));
    if (! emu_3) {
      h2_purity_ng->Fill(3, 0); // Total count in underflow bin
      if (n_x1t == 0) h2_purity_ng->Fill(3, 1);
      if (n_x2t == 0) h2_purity_ng->Fill(3, 2);
      if (n_x3t == 0) h2_purity_ng->Fill(3, 3);
      if (n_x4t == 0) h2_purity_ng->Fill(3, 4);
      if (n_x1b == 0) h2_purity_ng->Fill(3, 5);
      if (n_x2b == 0) h2_purity_ng->Fill(3, 6);
      if (n_x3b == 0) h2_purity_ng->Fill(3, 7);
      if (n_x4b == 0) h2_purity_ng->Fill(3, 8);
    }
  }

  /// Check the efficiency
  int trig_bits_raw = OnlMonTrigEP::EmulateRawTriggerBits(hit_vec);
  if (trig_fpga1 || trig_nim2 || trig_nim3) { // X triggers
    h1_cnt->AddBinContent(9);
    if (emu_1) {
      bool nim1_raw = trig_bits_raw & (0x1 << SQEvent::NIM1);
      h2_eff->Fill(1, (nim1_raw ? 1 : 0));
      if (! nim1_raw) {
        h2_eff_ng->Fill(1, 0); // Total count in underflow bin
        if (trig_fpga1) h2_eff_ng->Fill(1, 1);
        if (trig_nim2 ) h2_eff_ng->Fill(1, 2);
        if (trig_nim3 ) h2_eff_ng->Fill(1, 3);
      }
    }
  }
  if (trig_nim1) { // Y triggers
    h1_cnt->AddBinContent(10);
    if (emu_2) {
      bool nim2_raw = trig_bits_raw & (0x1 << SQEvent::NIM2);
      h2_eff->Fill(2, (nim2_raw ? 1 : 0));
      //if (! nim2_raw) {
      //}
    }
    if (emu_3) {
      bool nim3_raw = trig_bits_raw & (0x1 << SQEvent::NIM3);
      h2_eff->Fill(3, (nim3_raw ? 1 : 0));
    }
  }

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

  h2_purity_ng = (TH2*)FindMonHist("h2_purity_ng");
  if (! h2_purity_ng) return 1;

  h2_eff_ng = (TH2*)FindMonHist("h2_eff_ng");
  if (! h2_eff_ng) return 1;

  return 0;
}

int OnlMonTrigNim::DrawMonitor()
{
  OnlMonCanvas* can0 = GetCanvas(0);
  TPad* pad0 = can0->GetMainPad();
  pad0->Divide(1, 3);

  TVirtualPad* pad01 = pad0->cd(1);
  pad01->SetGrid();
  pad01->SetMargin(0.1, 0.1, 0.15, 0.1); // (l, r, b, t)
  h1_cnt->GetXaxis()->SetLabelSize(0.08);
  h1_cnt->Draw();
  TText text;
  text.SetNDC(true);
  text.SetTextSize(1.5 * text.GetTextSize());
  text.DrawText(0.60, 0.93, "NIM1 = Y1234T || Y1234B");
  text.DrawText(0.60, 0.86, "NIM2 = X1234T || X1234B");
  text.DrawText(0.60, 0.79, "NIM3 = X1234T && X1234B");

  TVirtualPad* pad02 = pad0->cd(2);
  pad02->Divide(2, 1);
  TVirtualPad* pad021 = pad02->cd(1);
  pad021->SetGrid();

  //pad02->SetGrid();
  TH1* h1_pur_all = h2_purity->ProjectionX("h1_pur_all");
  TH1* h1_pur_ok  = h2_purity->ProjectionX("h1_pur_ok", 2, 2);
  TGraphAsymmErrors* gr_pur = new TGraphAsymmErrors(h1_pur_ok, h1_pur_all);
  gr_pur->SetTitle(";NIM;Purity");
  gr_pur->SetMarkerStyle(21);
  gr_pur->SetMarkerColor(kBlue);
  gr_pur->SetLineColor  (kBlue);
  gr_pur->GetXaxis()->SetLabelSize(0.08);
  gr_pur->Draw("AP");
  gr_pur->GetYaxis()->SetRangeUser(0, 1);

  TVirtualPad* pad022 = pad02->cd(2);
  pad022->SetGrid();
  TLegend* leg022 = new TLegend(0.9, 0.6, 0.99, 0.9);
  leg022->SetHeader("  NIM");

  for (int nim = 1; nim <= 3; nim++) {
    ostringstream oss;
    oss << "h1_purity_ng_nim" << nim;
    TH1* h1 = h2_purity_ng->ProjectionY(oss.str().c_str(), nim, nim);
    h1->Scale(100 / h1->GetBinContent(0));
    h1->SetLineColor(nim);
    if (nim == 1) {
      const char* NAME[8] = { "1T", "2T", "3T", "4T", "1B", "2B", "3B", "4B" };
      for (int ii = 0; ii < 8; ii++) h1->GetXaxis()->SetBinLabel(ii+1, NAME[ii]);
      h1->Draw("HIST");
      h1->GetXaxis()->SetLabelSize(0.08);
      h1->GetYaxis()->SetRangeUser(0, 100);
      h1->SetTitle("Planes with no hit in impure events;;Rate (%)");
    } else {
      h1->Draw("HISTsame");
    }

    oss.str("");
    oss << " " << nim;
    leg022->AddEntry(h1, oss.str().c_str(), "l");
  }
  leg022->Draw();

  TVirtualPad* pad03 = pad0->cd(3);
  //pad03->SetGrid();
  pad03->Divide(2, 1);
  TVirtualPad* pad031 = pad03->cd(1);
  pad031->SetGrid();

  TH1* h1_eff_all = h2_eff->ProjectionX("h1_eff_all");
  TH1* h1_eff_ok  = h2_eff->ProjectionX("h1_eff_ok", 2, 2);
  TGraphAsymmErrors* gr_eff = new TGraphAsymmErrors(h1_eff_ok, h1_eff_all);
  gr_eff->SetTitle(";NIM;Efficiency");
  gr_eff->SetMarkerStyle(21);
  gr_eff->SetMarkerColor(kBlue);
  gr_eff->SetLineColor  (kBlue);
  gr_eff->GetXaxis()->SetLabelSize(0.08);
  gr_eff->Draw("AP");
  gr_eff->GetYaxis()->SetRangeUser(0, 1);

  TVirtualPad* pad032 = pad03->cd(2);
  pad032->SetGrid();
  TH1* h1_eff_ng_nim1 = h2_eff_ng->ProjectionY("h1_eff_ng_nim1", 1, 1);
  h1_eff_ng_nim1->Scale(100 / h1_eff_ng_nim1->GetBinContent(0));
  h1_eff_ng_nim1->SetTitle("Trigger types that took ineff. NIM1 events;;Rate (%)");
  h1_eff_ng_nim1->GetXaxis()->SetBinLabel(1, "FPGA1");
  h1_eff_ng_nim1->GetXaxis()->SetBinLabel(2, "NIM2");
  h1_eff_ng_nim1->GetXaxis()->SetBinLabel(3, "INM3");
  h1_eff_ng_nim1->GetXaxis()->SetLabelSize(0.08);
  h1_eff_ng_nim1->Draw("HIST");
  h1_eff_ng_nim1->GetYaxis()->SetRangeUser(0, 100);

  OnlMonCanvas::MonStatus_t status = OnlMonCanvas::OK;
  for (int i_pt = 0; i_pt < gr_pur->GetN(); i_pt++) {
    double nim, pur;
    gr_pur->GetPoint(i_pt, nim, pur);
    double err_hi = gr_pur->GetErrorYhigh(i_pt);
    if (pur + 3 * err_hi < 0.7) {
      status = OnlMonCanvas::WARN;
      can0->AddMessage(TString::Format("NIM%d purity = %.1f", (int)nim, pur).Data());
    }
  }
  for (int i_pt = 0; i_pt < gr_eff->GetN(); i_pt++) {
    double nim, eff;
    gr_eff->GetPoint(i_pt, nim, eff);
    double err_hi = gr_eff->GetErrorYhigh(i_pt);
    if (eff + 3 * err_hi < 0.7) {
      status = OnlMonCanvas::WARN;
      can0->AddMessage(TString::Format("NIM%d efficiency = %.1f", (int)nim, eff).Data());
    }
  }
  can0->SetStatus(status);

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
