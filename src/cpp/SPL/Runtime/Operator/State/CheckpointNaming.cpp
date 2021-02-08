/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Implementation of SPL::CheckpointNaming class
 */

#include <SPL/Runtime/Operator/State/CheckpointNaming.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <exception>
#include <sstream>

using namespace std;
using namespace SPL;

std::string CheckpointNaming::getCkptStoreEntryName(const std::string& domainID,
                                                    const std::string& instanceID,
                                                    const uint64_t& jobID,
                                                    const uint32_t& operatorIndex)
{
    assert(!domainID.empty());
    assert(!instanceID.empty());

    return domainID + "/" + instanceID + "/" + boost::lexical_cast<std::string>(jobID) + "/" +
           boost::lexical_cast<std::string>(operatorIndex);
}

std::string CheckpointNaming::getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                          const std::string& instanceID,
                                                          const uint64_t& jobID)
{
    assert(!domainID.empty());
    assert(!instanceID.empty());

    return domainID + "/" + instanceID + "/" + boost::lexical_cast<std::string>(jobID);
}

std::string CheckpointNaming::getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                          const std::string& instanceID,
                                                          const std::string& jobID)
{
    assert(!domainID.empty());
    assert(!instanceID.empty());

    return domainID + "/" + instanceID + "/" + jobID;
}

std::string CheckpointNaming::getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                          const std::string& instanceID)
{
    assert(!domainID.empty());
    assert(!instanceID.empty());

    return domainID + "/" + instanceID;
}

void CheckpointNaming::parseCkptStoreEntryName(const std::string& storeEntryName,
                                               bool& hasDomainID,
                                               bool& hasInstanceID,
                                               bool& hasJobID,
                                               bool& hasOperatorIndex,
                                               std::string& domainID,
                                               std::string& instanceID,
                                               uint64_t& jobID,
                                               uint32_t& operatorIndex)
{
    hasDomainID = false;
    hasInstanceID = false;
    hasJobID = false;
    hasOperatorIndex = false;
    try {
        boost::char_separator<char> sep("/");
        boost::tokenizer<boost::char_separator<char> > tokens(storeEntryName, sep);
        boost::tokenizer<boost::char_separator<char> >::iterator tok_iter = tokens.begin();
        if (tok_iter != tokens.end()) {
            domainID = *tok_iter;
            hasDomainID = true;
        } else {
            return;
        }
        ++tok_iter;
        if (tok_iter != tokens.end()) {
            instanceID = *tok_iter;
            hasInstanceID = true;
        } else {
            return;
        }
        ++tok_iter;
        if (tok_iter != tokens.end()) {
            jobID = boost::lexical_cast<uint64_t>(*tok_iter);
            hasJobID = true;
        } else {
            return;
        }
        ++tok_iter;
        if (tok_iter != tokens.end()) {
            operatorIndex = boost::lexical_cast<uint32_t>(*tok_iter);
            hasOperatorIndex = true;
        } else {
            return;
        }
    } catch (std::exception const& e) {
        THROW(DataStore, "parseCkptStoreEntryName() failed: received exception: " << e.what());
    }
}

std::string CheckpointNaming::getDataKey(const int64_t& id)
{
    std::stringstream str;
    str << "N" << hex << id;
    return str.str();
}

std::string CheckpointNaming::getIncrementalDataKey(const int64_t& id)
{
    std::stringstream str;
    str << "D" << hex << id;
    return str.str();
}

std::string CheckpointNaming::getIncrementalIndexKey(const int64_t& id)
{
    std::stringstream str;
    str << "I" << hex << id;
    return str.str();
}

std::string CheckpointNaming::getMetaDataKey()
{
    return "M";
}

int64_t CheckpointNaming::parseDataKey(const std::string& dataKey, enum KeyType& type)
{
    int64_t id;
    char typeChar;
    std::stringstream str(dataKey);
    str >> typeChar >> hex >> id;
    switch (typeChar) {
        case 'N':
            type = NORM_DATA_KEY;
            break;
        case 'D':
            type = INCREMENTAL_DATA_KEY;
            break;
        case 'I':
            type = INCREMENTAL_INDEX_KEY;
            break;
    };
    return id;
}
