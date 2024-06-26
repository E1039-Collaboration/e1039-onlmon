#ifndef _ONL_MON_V1495_TAIWAN_COMP__H_
#define _ONL_MON_V1495_TAIWAN_COMP__H_
#include "OnlMonClient.h"

class OnlMonV1495TaiwanComp: public OnlMonClient {
  typedef enum { HIT_BOTH = 1, HIT_V1495 = 2, HIT_TAIWAN = 3 } CompStatus_t;
  std::vector<std::string> m_list_det_name;
  std::vector<int        > m_list_det_id  ;

  TH1* h1_cnt;
  std::vector<TH2*> h2_comp; // X = element ID, Y = comparison status

 public:
  OnlMonV1495TaiwanComp();
  virtual ~OnlMonV1495TaiwanComp() {}
  OnlMonClient* Clone() { return new OnlMonV1495TaiwanComp(*this); }

  int InitOnlMon(PHCompositeNode *topNode);
  int InitRunOnlMon(PHCompositeNode *topNode);
  int ProcessEventOnlMon(PHCompositeNode *topNode);
  int EndOnlMon(PHCompositeNode *topNode);
  int FindAllMonHist();
  int DrawMonitor();

 private:
  void DrawStatusPlot(TH2* h2, const bool is_H1, TVirtualPad* pad, int& n_ng_v1495, int& n_ng_taiwan);
};

#endif /* _ONL_MON_V1495_TAIWAN_COMP__H_ */
