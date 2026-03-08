#ifndef _ONL_MON_RECO_GPU__H_
#define _ONL_MON_RECO_GPU__H_
#include "OnlMonClient.h"

class OnlMonRecoGpu: public OnlMonClient {
 public:

 private:
  //TH1* h1_ele     [N_DET];
  //TH1* h1_ele_in  [N_DET];
  //TH1* h1_time    [N_DET];
  //TH1* h1_time_in [N_DET];
  //TH2* h2_time_ele[N_DET];

 public:
  OnlMonRecoGpu();
  virtual ~OnlMonRecoGpu() {}
  OnlMonClient* Clone() { return new OnlMonRecoGpu(*this); }

  int InitOnlMon(PHCompositeNode *topNode);
  int InitRunOnlMon(PHCompositeNode *topNode);
  int ProcessEventOnlMon(PHCompositeNode *topNode);
  int EndOnlMon(PHCompositeNode *topNode);
  int FindAllMonHist();
  int DrawMonitor();
};

#endif /* _ONL_MON_RECO_GPU__H_ */
