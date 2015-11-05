#ifndef ANABASELINE_BASELINEDEF_H
#define ANABASELINE_BASELINEDEF_H

#include "NTupleReader.h"
#include "customize.h"

#include "Math/VectorUtil.h"

#include <sstream>
#include <iostream>
#include <fstream>

class BaselineVessel
{
private:
    const std::string spec;
    topTagger::type3TopTagger * type3Ptr;

public:
    std::string jetVecLabel;
    std::string CSVVecLabel;
    std::string METLabel;
    std::string METPhiLabel;
    std::string muonsFlagIDLabel;

    BaselineVessel(const std::string specialization = "") : spec(specialization),
      jetVecLabel("jetsLVec"), CSVVecLabel("recoJetsBtag_0"), METLabel("met"), METPhiLabel("metphi") { }

    void passBaseline(NTupleReader &tr);
    void prepareTopTagger();
    bool GetnTops(NTupleReader *tr) const;
    bool passNoiseEventFilterFunc(NTupleReader &tr);

    topTagger::type3TopTagger * GetType3Ptr() const {return type3Ptr;};
    inline void operator()(NTupleReader &tr) { passBaseline(tr); }
};

inline void passBaselineFunc(NTupleReader &tr)
{
  BaselineVessel blv;
  blv.prepareTopTagger();
  blv.passBaseline(tr);
  blv.GetnTops(&tr);
}

namespace stopFunctions
{
    class CleanJets
    {
    public:        
        void operator()(NTupleReader& tr) {internalCleanJets(tr);}

        void setMuonIso(const std::string muIsoFlag);
        void setElecIso(const std::string elecIsoFlag);
        void setJetCollection(std::string jetVecLabel);
        void setBTagCollection(std::string bTagLabel);
        void setMuonsFlagID(std::string muonsFlagIDLabel);
        void setElesFlagID(std::string elesFlagIDLabel);
        void setEnergyFractionCollections(std::string chargedEMfrac, std::string neutralEMfrac, std::string chargedHadfrac);
        void setForceDr(bool forceDr);
        void setDisable(bool disable);
        void setRemove(bool remove);
        void setElecPtThresh(double minPt);
        void setMuonPtThresh(double minPt);
        //This option is used to clean up to 1 jet in the minDr cone around the muon if the jet is lower pt than the muon
        //It is designed only for use with the z->inv background to remove muon related radiation from the event
        void setPhotoCleanThresh(double photoCleanThresh);

        //NOTE!!! Must add Hadron and EM fraction vectors here

        CleanJets()
        {
            setMuonIso("mini");
            setElecIso("mini");
            setJetCollection("jetsLVec");
            setBTagCollection("recoJetsBtag_0");
            setMuonsFlagID("muonsFlagMedium");
            setElesFlagID("");
            setEnergyFractionCollections("recoJetschargedHadronEnergyFraction", "recoJetsneutralEmEnergyFraction", "recoJetschargedEmEnergyFraction");    
            setForceDr(false);
            setRemove(false);
            setDisable(false);
            setElecPtThresh(0.0);
            setMuonPtThresh(0.0);
            setPhotoCleanThresh(-999.9);
        }
        
    private:
        std::string muIsoStr_, elecIsoStr_, jetVecLabel_, bTagLabel_, chargedEMFracLabel_, neutralEMFracLabel_, chargedHadFracLabel_;
        std::string muonsFlagIDLabel_, elesFlagIDLabel_;
        AnaConsts::IsoAccRec muIsoReq_;
        AnaConsts::ElecIsoAccRec elecIsoReq_;
        double elecPtThresh_;
        double muonPtThresh_;
        double photoCleanThresh_;
        bool remove_;
        bool disable_;
        bool forceDr_;

        int cleanLeptonFromJet(const TLorentzVector& lep, const int& lepMatchedJetIdx, const std::vector<TLorentzVector>& jetsLVec, std::vector<bool>& keepJet, const std::vector<double>& neutralEmEnergyFrac, std::vector<TLorentzVector>* cleanJetVec, const double& jldRMax, const double photoCleanThresh = -999.9);
        void internalCleanJets(NTupleReader& tr);
    };

    inline void cleanJets(NTupleReader& tr)
    {
      CleanJets cjh;
      cjh(tr);
    }
}

#endif
