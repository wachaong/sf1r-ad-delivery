/*
 * utils.h
 *
 *  Created on: Mar 31, 2014
 *      Author: alex
 */

#ifndef SF1R_LASER_CLUSTERING_UTILS_H_
#define SF1R_LASER_CLUSTERING_UTILS_H_
#include <string>
#include <vector>
#include <map>
#include "constant.h"
#include "util/hashFunction.h"
#include  "boost/filesystem/operations.hpp"
#include  "boost/filesystem/path.hpp"
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

namespace sf1r { namespace laser { namespace clustering {
inline hash_t Hash_(const std::string& cat)
{
    return (hash_t)izenelib::util::HashFunction<std::string>::generateHash32(cat);
}

template <class izenestream>
inline izenestream* openFile( std::string path, bool must)
{
    izenestream *izene_ptr = new izenestream(path.c_str());
    if(izene_ptr->Open())
        return izene_ptr;
    else
    {
        if (must)
            throw std::runtime_error("Could not old the file"+path);
        else
            return NULL;
    }

}
template <class izenestream>
inline void closeFile( izenestream*&  stream_ptr)
{
    if(stream_ptr != NULL)
    {
        stream_ptr->Close();
        delete stream_ptr;
        stream_ptr = NULL;
    }
}
template <class izenestream>
inline void closeFiles(std::map<hash_t, izenestream*>& cat_stream_map)
{
    typename std::map<hash_t, izenestream*>::iterator iter = cat_stream_map.begin();
    for(; iter != cat_stream_map.end(); iter++)
    {
        closeFile<izenestream> (iter->second);
    }
}

template<class object>
inline void serialization(std::string & serial_str, const object & obj)
{
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << obj;
    s.flush();
}

template<class object>
inline void deserialize(const std::string & serial_str, object & obj)
{
    boost::iostreams::basic_array_source<char> device(serial_str.data(), serial_str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s2(device);
    boost::archive::binary_iarchive ia(s2);
    ia >> obj;
}

template<class object>
inline void serializeFile(const std::string & serialFileName, object & obj)
{
    std::ofstream ifs(serialFileName.c_str());
    boost::archive::text_oarchive oa(ifs);
    oa << obj;
}

template<class Key,class Type>
inline void serializeCollectionFile(const std::string & serialFileName,
                                    const boost::unordered_map<Key, Type> & obj)
{
    std::ofstream ifs(serialFileName.c_str());
    if(!ifs)
    {
        return;
    }
    boost::archive::text_oarchive oa(ifs);
    //oa << obj;
    boost::serialization::stl::save_collection< boost::archive::text_oarchive,
    boost::unordered_map<Key, Type> >(oa, obj);
}

template<class Key, class Type>
inline void deserializeCollectionFile(const std::string & deserialFileName,
                                       boost::unordered_map<Key, Type> & obj)
{
    std::ifstream ifs(deserialFileName.c_str(), std::ios::binary);
    if(!ifs)
    {
        return;
    }
    boost::archive::text_iarchive ia(ifs);
    boost::serialization::stl::load_collection<
    boost::archive::text_iarchive,
    boost::unordered_map<Key, Type>,
    boost::serialization::stl::archive_input_map<
    boost::archive::text_iarchive, boost::unordered_map<Key, Type> >,
    boost::serialization::stl::no_reserve_imp<boost::unordered_map<Key, Type> >
    >(ia, obj);
//    ia >> obj;
}


template<class object>
inline void deserializeFile(const std::string & deserialFileName, object & obj)
{
    std::ifstream ifs(deserialFileName.c_str(), std::ios::binary);
    boost::archive::text_iarchive ia(ifs);
    ia >> obj;
}

} } }
#endif /* _CLUSTERING_UTILS_H_ */
