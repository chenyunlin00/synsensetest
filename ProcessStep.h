#pragma once
#include <vector>
#include <functional>
#include <algorithm>


#include "CommonDef.h"

BEGIN_SYNSENSE_NAMESPACE

template<typename T, typename Container=std::vector<T>>
class ProcessStep {
public:
    virtual ~ProcessStep(){}
    virtual void execute(Container &vec) = 0;
};

template<typename T, typename Container=std::vector<T>>
class TraverseStep : public ProcessStep<T, Container> {
    typedef std::function<void (Container &)> CallbackFunc;
public:
    TraverseStep() = delete;
    TraverseStep(CallbackFunc callback) : _callback(callback) {}
    
    virtual void execute(Container &vec) {
        _callback(vec);
    }

protected:
    CallbackFunc _callback;
};

template<typename T, typename Container=std::vector<T>>
class FilterStep : public ProcessStep<T, Container> {
    typedef std::function<bool (const T&)> CallbackFunc;
public:
    FilterStep() = delete;
    FilterStep(CallbackFunc callback) : _callback(callback) {}

    virtual void execute(Container &vec) {
        for (auto itor = vec.begin(); itor != vec.end();) {
            if (_callback(*itor) == false) {
                itor = vec.erase(itor);
            }
            else {
                itor++;
            }
        }
    }

protected:
    CallbackFunc _callback;
};

template<typename T, typename Container=std::vector<T>>
class SortStep : public ProcessStep<T, Container> {
    typedef std::function<bool (const T&, const T&)> CallbackFunc;
public:
    SortStep() = delete;
    SortStep(CallbackFunc callback) : _callback(callback) {}

    virtual void execute(Container &vec) {
        SortInner(vec);
    }

protected:
    void SortInner(std::vector<T> &vec) {
        std::sort(vec.begin(), vec.end(), _callback);
    }

    void SortInner(std::list<T> &list) {
        list.sort(_callback);
    }

protected:
    CallbackFunc _callback;
};

END_SYNSENSE_NAMESPACE