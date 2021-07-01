#pragma once
#include <vector>
#include <functional>
#include <algorithm>


#include "CommonDef.h"

BEGIN_SYNSENSE_NAMESPACE

template<typename T>
class ProcessStep {
public:
    virtual ~ProcessStep(){}
    virtual void execute(std::vector<T> &vec) = 0;
};

template<typename T>
class TraverseStep : public ProcessStep<T> {
    typedef std::function<void (std::vector<T> &)> CallbackFunc;
public:
    TraverseStep() = delete;
    TraverseStep(CallbackFunc callback) : _callback(callback) {

    }
    virtual void execute(std::vector<T> &vec) {
        _callback(vec);
    }

protected:
    CallbackFunc _callback;
};

template<typename T>
class FilterStep : public ProcessStep<T> {
    typedef std::function<bool (const T&)> CallbackFunc;
public:
    FilterStep() = delete;
    FilterStep(CallbackFunc callback) : _callback(callback) {

    }
    virtual void execute(std::vector<T> &vec) {
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

template<typename T>
class SortStep : public ProcessStep<T> {
    typedef std::function<bool (const T&, const T&)> CallbackFunc;
public:
    SortStep() = delete;
    SortStep(CallbackFunc callback) : _callback(callback) {

    }
    virtual void execute(std::vector<T> &vec) {
        std::sort(vec.begin(), vec.end(), _callback);
    }

protected:
    CallbackFunc _callback;
};

END_SYNSENSE_NAMESPACE