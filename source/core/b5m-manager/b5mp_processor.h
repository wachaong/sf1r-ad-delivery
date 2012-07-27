#ifndef SF1R_B5MMANAGER_B5MPPROCESSOR_H_
#define SF1R_B5MMANAGER_B5MPPROCESSOR_H_

#include <string>
#include <vector>
#include "b5m_types.h"
#include "b5m_helper.h"
#include "product_db.h"
#include "offer_db.h"
#include "history_db_helper.h"
#include <common/ScdMerger.h>
#include <am/sequence_file/ssfr.h>

namespace sf1r {

    class LogServerConnectionConfig;
    ///B5mpProcessor is responsibility to generate b5mp scds and also b5mo_mirror scd
    class B5mpProcessor{
        typedef izenelib::am::ssf::Writer<> PoMapWriter;
        typedef std::set<std::string> ItemsT;
        typedef boost::unordered_map<std::string, ItemsT >  ProductOfferT;
    public:
        B5mpProcessor(B5MHistoryDBHelper* hdb, const std::string& mdb_instance,
            const std::string& last_mdb_instance, LogServerConnectionConfig* config);

        bool Generate();

    private:

        void ProductMerge_(ScdMerger::ValueType& value, const ScdMerger::ValueType& another_value);

        void ProductOutput_(Document& doc, int& type);

    private:
        B5MHistoryDBHelper*   historydb_;
        std::string mdb_instance_;
        std::string last_mdb_instance_;
        PoMapWriter* po_map_writer_;
        LogServerConnectionConfig* log_server_cfg_;
    };

}

#endif

