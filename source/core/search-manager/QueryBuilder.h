/**
 * @file sf1r/search-manager/QueryBuilder.h
 * @author Yingfeng Zhang
 * @date Created <2009-09-21>
 * @brief Builder which create DocumentIterator according to user query requests
 */
#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <query-manager/SearchKeywordOperation.h>
#include <query-manager/ActionItem.h>
#include <query-manager/QueryTree.h>
#include "DocumentIterator.h"
#include "MultiPropertyScorer.h"
#include "WANDDocumentIterator.h"
#include "VirtualPropertyScorer.h"
#include "VirtualPropertyTermDocumentIterator.h"
#include "Filter.h"
#include <index-manager/IndexManager.h>
#include <document-manager/DocumentManager.h>

#include <ir/id_manager/IDManager.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/threadpool.hpp>

#include <vector>

using namespace izenelib::ir::idmanager;
using namespace izenelib::ir::indexmanager;
using namespace sf1r::QueryFiltering;

namespace sf1r
{
class FilterCache;
typedef DocumentIterator* DocumentIteratorPointer;
class QueryBuilder
{
    typedef std::map<std::string, PropertyTermInfo> property_term_info_map;

public:
    typedef boost::unordered_map<std::string, PropertyConfig> schema_map;
    typedef schema_map::const_iterator schema_iterator;

    QueryBuilder(
        const boost::shared_ptr<IndexManager> indexManager,
        const boost::shared_ptr<DocumentManager> documentManager,
        const boost::shared_ptr<IDManager> idManager,
        const boost::shared_ptr<RankingManager>& rankingManager,
        const schema_map& schemaMap,
        size_t filterCacheNum
    );

    ~QueryBuilder();

public:
    /***********************
    *@brief Generate the DocumentIterator according to query tree
    *@return if failed, NULL is returned
    */
    MultiPropertyScorer* prepare_dociterator(
        const SearchKeywordOperation& actionOperation,
        collectionid_t colID,
        const property_weight_map& propertyWeightMap_,
        const std::vector<std::string>& properties,
        const std::vector<unsigned int>& propertyIds,
        bool readPositions,
        const std::vector<std::map<termid_t, unsigned> >& termIndexMaps
    );

    /*
    *@brief Generate the WANDDocumentIterator
    *@return if failed, NULL is returned
    */
    WANDDocumentIterator* prepare_wand_dociterator(
        const SearchKeywordOperation& actionOperation,
        collectionid_t colID,
        const property_weight_map& propertyWeightMap,
        const std::vector<std::string>& properties,
        const std::vector<unsigned int>& propertyIds,
        bool readPositions,
        const std::vector<std::map<termid_t, unsigned> >& termIndexMaps
    );

    /*
    *@brief Generate Filter, filter will be released by the user.
    */
    void prepare_filter(
        const std::vector<QueryFiltering::FilteringType>& filtingList,
        boost::shared_ptr<EWAHBoolArray<uint32_t> >& pDocIdSet
    );

    void reset_cache();

    /**
     * @brief get corresponding id of the property, returns 0 if the property
     * does not exist.
     * @see CollectionMeta::numberPropertyConfig
     */
    propertyid_t getPropertyIdByName(const std::string& name) const;

    void post_prepare_ranker_(
        const std::string& virtualProperty,
        const std::vector<std::string>& indexPropertyList,
        unsigned indexPropertySize,
        const property_term_info_map& propertyTermInfoMap,
        DocumentFrequencyInProperties& dfmap,
        CollectionTermFrequencyInProperties& ctfmap,
        MaxTermFrequencyInProperties& maxtfmap,
        bool readTermPosition,
        std::vector<RankQueryProperty>& rankQueryProperties,
        std::vector<boost::shared_ptr<PropertyRanker> >& propertyRankers);

private:
    void prefetch_term_doc_readers_(
        const std::vector<pair<termid_t, string> >& termList,
        collectionid_t colID,
        const PropertyConfig& properyConfig,
        bool readPositions,
        bool& isNumericFilter,
        std::map<termid_t, std::vector<TermDocFreqs*> > & termDocReaders
    );

    void prepare_for_property_(
        MultiPropertyScorer* pScorer,
        size_t & success_properties,
        const SearchKeywordOperation& actionOperation,
        collectionid_t colID,
        const PropertyConfig& properyConfig,
        bool readPositions,
        const std::map<termid_t, unsigned>& termIndexMapInProperty
    );

    void prepare_for_virtual_property_(
        MultiPropertyScorer* pScorer,
        size_t & success_properties,
        const SearchKeywordOperation& actionOperation,
        collectionid_t colID,
        const PropertyConfig& properyConfig, //virtual property config
        bool readPositions,
        const std::map<termid_t, unsigned>& termIndexMapInProperty,
        const property_weight_map& propertyWeightMap
    );

    bool do_prepare_for_virtual_property_(
        QueryTreePtr& queryTree, 
        collectionid_t colID,
        const std::vector<std::string>& properties,
        std::vector<unsigned int>& propertyIds,
        std::vector<PropertyDataType>& propertyDataTypes,
        bool isNumericFilter,
        bool isReadPosition,
        std::map<termid_t, unsigned> termIndexMap,
        DocumentIteratorPointer& pDocIterator,
        std::vector<std::map<termid_t, std::vector<izenelib::ir::indexmanager::TermDocFreqs*> > >& termDocReadersList,
        bool hasUnigramProperty,
        bool isUnigramSearchMode,
        int parentAndOrFlag = 0
    );
   
    void prepare_for_wand_property_(
        WANDDocumentIterator* pWandScorer,
        size_t & success_properties,
        const SearchKeywordOperation& actionOperation,
        collectionid_t colID,
        const std::string& property,
        unsigned int propertyId,
        bool readPositions,
        const std::map<termid_t, unsigned>& termIndexMapInProperty
    );

    /// @param parentAndOrFlag, parent node type, available when isUnigramSearchMode = true.
    ///        1 AND, 0 OR, -1 Other/not defined.
    bool do_prepare_for_property_(
        QueryTreePtr& queryTree,
        collectionid_t colID,
        const std::string& property,
        unsigned int propertyId,
        PropertyDataType propertyDataType,
        bool isNumericFilter,
        bool readPositions,
        const std::map<termid_t, unsigned>& termIndexMapInProperty,
        DocumentIteratorPointer& pDocIterator,
        std::map<termid_t, VirtualPropertyTermDocumentIterator* >& virtualTermIters,
        std::map<termid_t, std::vector<izenelib::ir::indexmanager::TermDocFreqs*> >& termDocReaders,
        bool hasUnigramProperty = true,
        bool isUnigramSearchMode = false,
        const std::string& virtualProperty = "",
        int parentAndOrFlag = 0
    );

    void getTermIdsAndIndexesOfSiblings(
        QueryTreePtr& queryTree,
        const std::map<termid_t, unsigned>& termIndexMapInProperty,
        const std::string& property,
        std::vector<termid_t>& outTermIds,
        std::vector<termid_t>& outTermIndexes
    );

private:
    boost::shared_ptr<IndexManager> indexManagerPtr_;

    boost::shared_ptr<DocumentManager> documentManagerPtr_;

    boost::shared_ptr<IDManager> idManagerPtr_;

    boost::shared_ptr<RankingManager> rankingManagerPtr_;

    IndexReader* pIndexReader_;

    const schema_map& schemaMap_;

    boost::scoped_ptr<FilterCache> filterCache_;
};

}

#endif
