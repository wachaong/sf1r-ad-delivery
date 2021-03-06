#ifndef SF1R_LASER_TOPN_CLUSTERNG_MODEL_H
#define SF1R_LASER_TOPN_CLUSTERNG_MODEL_H

#include "LaserModel.h"
#include "LaserModelDB.h"

namespace sf1r { namespace laser {
class AdIndexManager;
class LaserOnlineModel;
class LaserOfflineModel;

class TopnClusteringModel : public LaserModel
{
public:
    TopnClusteringModel(const AdIndexManager& adIndexer,
        const std::vector<std::vector<int> >& similarClustering,
        const std::string& workdir,
        const std::string& sysdir);
    ~TopnClusteringModel();

public:
    virtual bool candidate(
        const std::string& text,
        const std::size_t ncandidate,
        const std::vector<std::pair<int, float> >& context, 
        std::vector<std::pair<docid_t, std::vector<std::pair<int, float> > > >& ad,
        std::vector<float>& score) const;
    virtual float score( 
        const std::string& text,
        const std::vector<std::pair<int, float> >& user, 
        const std::pair<docid_t, std::vector<std::pair<int, float> > >& ad,
        const float score) const;
    
    virtual bool candidate(
        const std::string& text,
        const std::size_t ncandidate,
        const std::vector<float>& context, 
        std::vector<std::pair<docid_t, std::vector<std::pair<int, float> > > >& ad,
        std::vector<float>& score) const;
    virtual float score( 
        const std::string& text,
        const std::vector<float>& user, 
        const std::pair<docid_t, std::vector<std::pair<int, float> > >& ad,
        const float score) const;
    
    
    virtual bool context(const std::string& text, std::vector<std::pair<int, float> >& context) const
    {
        return false;
    }
    
    virtual bool context(const std::string& text, std::vector<float>& context) const
    {
        return false;
    }
    
    virtual void dispatch(const std::string& method, msgpack::rpc::request& req);
private:
    bool getAD(const std::size_t& clusteringId, 
        std::vector<std::pair<docid_t, std::vector<std::pair<int, float> > > >& ad) const;
    bool getSimilarAd(const std::size_t& clusteringId, 
        std::vector<std::pair<docid_t, std::vector<std::pair<int, float> > > >& ad) const;
    void getSimilarClustering(
        const std::size_t& clusteringId, 
        std::vector<int>& similar) const;

    void updatepUserDb(msgpack::rpc::request& req);
    void updateTopClusteringDb(msgpack::rpc::request& req);

    void localizeFromOrigDB(bool loadUDB, bool loadCDB);
protected:
    const AdIndexManager& adIndexer_;
    const std::vector<std::vector<int> >& similarClustering_;
    const std::string workdir_;
    const std::string sysdir_;
    LaserModelDB<std::string, LaserOnlineModel>* pUserDb_;
    LaserModelDB<std::string, LaserOnlineModel>* origUserDb_;
    LaserModelDB<std::string, std::vector<std::pair<int, float> > >* topClusteringDb_;
    LaserModelDB<std::string, std::vector<std::pair<int, float> > >* origClusteringDb_;
};
} }
#endif
