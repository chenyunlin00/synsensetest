#pragma once
#include <stdint.h>
#include <vector>
#include <functional>
#include <algorithm>

#include "CommonDef.h"

BEGIN_SYNSENSE_NAMESPACE

template <typename T>
class Collection {
public:
   
    Collection() {
        _collection = new std::vector<T>;
    }
    
    Collection(const Collection &c) : Collection() {
        *_collection = *c._collection;
    }
    
    Collection(Collection &&c) : _collection(c._collection) {
        c._collection = nullptr;
    }

    virtual ~Collection() {
        SAFE_DELETE(_collection)
    }

    Collection &Transform(std::function<void(T &)> transformFunc) {
        for (auto itor = _collection->begin(); itor != _collection->end(); itor++) {
            transformFunc(*itor);
        }
        return *this;
    }

    Collection &Add(const std::vector<T> &vec) {
        _collection->insert(_collection->end(), vec.begin(), vec.end());
        return *this;
    }

    Collection &Add(const T &value) {
        _collection->push_back(value);
        return *this;
    }

    //if filterFunc return false value will remove from the collections
    //otherwise value will remain in the collections
    Collection &Remove(std::function<bool(const T& value)> filterFunc) {
        for (auto itor = _collection->begin(); itor != _collection->end();) {
            if (filterFunc(*itor) == false) {
                itor = _collection->erase(itor);
            } 
            else {
                itor++;
            }
        }
        return *this;
    }

    Collection &Sort(std::function<bool(const T&, const T&)> compareFunc) {
        std::sort(_collection->begin(), _collection->end(), compareFunc);
        return *this;
    }

    Collection &Clear() {
        _collection->clear();
        return *this;
    }

    std::vector<T> *Data() {
        return _collection;
    }

protected:
    std::vector<T> *_collection;
};

END_SYNSENSE_NAMESPACE