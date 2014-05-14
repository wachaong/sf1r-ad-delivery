#ifndef AD_SPONSORED_AUCTIONLOG_MGR_H
#define AD_SPONSORED_AUCTIONLOG_MGR_H

#include "AdCommonDataType.h"
#include <util/izene_serialization.h>
#include <util/singleton.h>
#include <vector>
#include <map>
#include <boost/unordered_map.hpp>
#include <string>
#include <set>

namespace sf1r
{

namespace sponsored
{

struct AdViewSlotInfo
{
    AdViewSlotInfo()
        :impression_num(0), click_num(0)
    {
    }
    int impression_num;
    int click_num;
    // cost (unit is 0.01yuan) -> total number for this cost.
    std::map<int, int> keyword_cpc;
    DATA_IO_LOAD_SAVE(AdViewSlotInfo, & impression_num & click_num & keyword_cpc);
private:
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & impression_num;
            ar & click_num;
            ar & keyword_cpc;
        }
};

// the ad view info for all slot position
typedef std::vector<AdViewSlotInfo> AdViewInfoT;
// view info for each day, clear the data of old day and push that of new day.
typedef std::map<std::string, AdViewInfoT>  AdViewInfoDayListT;
typedef boost::unordered_map<std::string, AdViewInfoDayListT> AdStatContainerT;

struct KeywordViewInfo
{
    KeywordViewInfo()
        :searched_num(0)
    {
    }
    int searched_num;
    struct KeywordViewSlotInfo
    {
        KeywordViewSlotInfo()
            :click_num(0)
        {
        }
        int click_num;
        std::map<int, int> cost_list;
        DATA_IO_LOAD_SAVE(KeywordViewSlotInfo, & click_num & cost_list);
    private:
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar & click_num;
                ar & cost_list;
            }
    };

    // different cost distribution at each position.
    std::vector<KeywordViewSlotInfo> view_info;
    DATA_IO_LOAD_SAVE(KeywordViewInfo, & searched_num & view_info);
private:
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & searched_num;
            ar & view_info;
        }
};
typedef std::map<std::string, KeywordViewInfo> KeywordViewInfoDayListT;
typedef std::vector<KeywordViewInfoDayListT> KeywordStatContainerT;

struct GlobalInfo
{
    std::vector<int> click_num_list;
    DATA_IO_LOAD_SAVE(GlobalInfo, & click_num_list);
private:
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & click_num_list;
        }
};

typedef std::map<std::string, GlobalInfo> GlobalInfoDayListT;

class AdAuctionLogMgr
{
public:
    //static AdAuctionLogMgr* get()
    //{
    //    return izenelib::util::Singleton<AdAuctionLogMgr>::get();
    //}

    AdAuctionLogMgr();
    void init(const std::string& datapath);
    void updateAdSearchStat(const std::set<BidKeywordId>& keyword_list,
        const std::vector<std::string>& ranked_ad_list);
    void updateAuctionLogData(const std::string& ad_id, const BidPhraseT& keyword_list,
        int click_cost_in_fen, uint32_t click_slot);

    double getAdCTR(const std::string& adid);
    // clicked number for a day on the keyword.
    std::size_t getAvgKeywordClickedNum(BidKeywordId keyid);
    // total clicked number for a day.
    std::size_t getAvgTotalClickedNum();

    int getKeywordAvgCost(BidKeywordId keyid, std::size_t slot);
    int getAdAvgCost(const std::string& adid);
    double getKeywordCTR(BidKeywordId kid);
    void getKeywordBidLandscape(std::vector<BidKeywordId>& keyword_list,
        std::vector<std::vector<std::pair<int, double> > >& cost_click_list);
    void getAdLandscape(std::vector<std::string>& ad_list,
        std::vector<std::vector<std::pair<int, double> > >& cost_click_list);

    void load();
    void save();

private:
    AdStatContainerT  ad_stat_data_;
    KeywordStatContainerT keyword_stat_data_;
    GlobalInfoDayListT  global_stat_data_;
    std::string data_path_;
};

}


}

#endif
