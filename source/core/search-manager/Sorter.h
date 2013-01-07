/**
 * @file sf1r/search-manager/Sorter.h
 * @author Yingfeng Zhang
 * @date Created <2009-10-10>
 * @brief Providing utilities for sorting documents according to properties
 */
#ifndef _SORTER_H
#define _SORTER_H

#include "SortPropertyComparator.h"
#include <util/ustring/UString.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <list>
#include <map>
#include <string>

using namespace std;

namespace sf1r
{

class DocumentManager;
class IndexManager;
class IndexBundleConfiguration;
class PropSharedLockSet;
class NumericPropertyTableBuilder;

namespace faceted
{
class CTRManager;
}

/*
* @brief Sortproperty  indicates a property according to which the query results are sorted
*/
class SortProperty
{
public:
    enum SortPropertyType
    {
        SCORE,  ///sort by ranking score
        AUTO,   ///sort by ranking score and property
        CUSTOM, ///sort by customized comparator
        CTR     ///sort by click through rate
    };
public:
    SortProperty(const SortProperty& src);

    SortProperty(const string& property, PropertyDataType propertyType, bool reverse = false);

    SortProperty(const string& property, PropertyDataType propertyType, SortPropertyType type, bool reverse = false);
    ///pComparator is generated outside by customer
    SortProperty(const string& property, PropertyDataType propertyType, SortPropertyComparator* pComparator, bool reverse = false);

    SortProperty(const string& property, PropertyDataType propertyType, SortPropertyComparator* pComparator, SortPropertyType type, bool reverse);

    ~SortProperty();

public:
    string& getProperty()
    {
        return property_;
    }

    PropertyDataType getPropertyDataType()
    {
        return propertyDataType_;
    }

    SortPropertyType getType()
    {
        return type_;
    }

    bool isReverse()
    {
        return reverse_;
    }

    SortPropertyComparator* getComparator()
    {
        return pComparator_;
    }

    int compare(const ScoreDoc& doc1,const ScoreDoc& doc2) const
    {
        return pComparator_->compare(doc1, doc2);
    }

private:
    ///name of property to be sorted
    string property_;
    ///type of data of this sorted proeprty
    PropertyDataType propertyDataType_;

    SortPropertyType type_;
    ///whether order results ASC or DESC
    bool reverse_;
    ///practical comparator
    SortPropertyComparator* pComparator_;

    friend class Sorter;
};

/*
* @brief Sorter main interface exposed.
*/
class Sorter
{
public:
    Sorter(NumericPropertyTableBuilder* numericTableBuilder);

    ~Sorter();

public:

    void addSortProperty(const string& property, PropertyDataType propertyType, bool reverse = false);

    void addSortProperty(SortProperty* pSortProperty);

    bool requireScorer()
    {
        for(size_t i = 0; i < nNumProperties_; ++i)
        {
            if(ppSortProperties_[i]->getProperty()== "RANK")
                return true;
        }
        return false;
    }

    bool lessThan(const ScoreDoc& doc1,const ScoreDoc& doc2)
    {
        size_t i=0;
        for(; i < nNumProperties_; ++i)
        {
            //int c = (reverseMul_[i]) * ppSortProperties_[i]->compare(doc1, doc2);
            int c = ppSortProperties_[i]->compare(doc1, doc2);
            if(c != 0)
            {
                //return c < 0;
                ///Tricky optmization, while difficult to maintain
                return c ^ (reverseMul_[i]);
            }
            //c = (pSortProperty->isReverse()) ? pSortProperty->pComparator_->compare(doc2, doc1)
            //       : pSortProperty->pComparator_->compare(doc1, doc2);
        }

        return doc1.docId > doc2.docId;
//    return c < 0;
    }

    ///This interface would be called after an instance of Sorter is established,
    /// it will generate SortPropertyComparator for internal usage
    void createComparators(PropSharedLockSet& propSharedLockSet);

private:
    SortPropertyComparator* createNumericComparator_(
        const std::string& propName,
        PropSharedLockSet& propSharedLockSet);

private:
    NumericPropertyTableBuilder* numericTableBuilder_;

    std::list<SortProperty*> sortProperties_;

    SortProperty** ppSortProperties_;

    int* reverseMul_;

    size_t nNumProperties_;

    friend class SearchManager;
    friend class SearchManagerPreProcessor;
};

}

#endif
