#pragma once
#include <vector>
#include <list>
#include <mutex>
#include <functional>
#include <memory>
#include <algorithm>
#include <queue>
#include <condition_variable>
#include <thread>
#include <iostream>

#include "CommonDef.h"
#include "ProcessStep.h"

BEGIN_SYNSENSE_NAMESPACE

template<typename T, typename Container=std::vector<T>>
class DataProcessor {
public:
    DataProcessor() {
        _isQueueEmpty = true;
        _isStop = false;  
        _loopThread = nullptr;     
    }

    DataProcessor(const DataProcessor &processor) = delete;
    DataProcessor & operator=(const DataProcessor &processor) = delete;

    virtual ~DataProcessor() {
        SAFE_DELETE(_loopThread)
        std::unique_lock<std::mutex> lock(_processStepsLock); 
        for (auto itor = _processSteps.begin(); itor != _processSteps.end(); itor++) {
            SAFE_DELETE(*itor)
        }
        _processSteps.clear();
    }

    void AddData(const Container &vec) {
        std::unique_lock<std::mutex> lock(_queueLock);
        _dataQueue.push_back(vec);
        _isQueueEmpty = false;
        _queueCV.notify_one();
    }
    void AddData(Container &&vec) {
        std::unique_lock<std::mutex> lock(_queueLock);
        _dataQueue.push_back(vec);
        _isQueueEmpty = false;
        _queueCV.notify_one();
    }

    DataProcessor &Traverse(std::function<void (Container &)> traverseFunc) {
        std::unique_lock<std::mutex> locker(_processStepsLock);
        ProcessStep<T, Container> *step = new TraverseStep<T, Container>(traverseFunc);
        _processSteps.push_back(step);
        return *this;
    }

    DataProcessor &Filter(std::function<bool (const T&)> filterFunc) {
        std::unique_lock<std::mutex> locker(_processStepsLock);
        ProcessStep<T, Container> *step = new FilterStep<T, Container>(filterFunc);
        _processSteps.push_back(step);
        return *this;
    }

    DataProcessor &Sort(std::function<bool (const T&, const T&)> sortFunc) {
        std::unique_lock<std::mutex> locker(_processStepsLock);
        ProcessStep<T, Container> *step = new SortStep<T, Container>(sortFunc);
        _processSteps.push_back(step);
        return *this;
    }

    DataProcessor &Run() {
        if (_loopThread != nullptr) {
            return *this;
        }
        _loopThread = new std::thread(&DataProcessor::Loop, this);
        return *this;
    }

    void Stop() {
        if (_isStop == true) {
            return;
        }
        _isStop = true;
        _queueCV.notify_all();
        if (_loopThread != nullptr) {
            _loopThread->join();
        }
    }

    void AddNextProcessor(DataProcessor *processor) {
        std::unique_lock<std::mutex> locker(_nextProcessorsLock);
        _nextProcessors.push_back(processor);
    }

protected:
    void Loop() {
        //std::cout << "enter loop" << std::endl;
        while (_isStop == false) {
            std::unique_lock<std::mutex> lock(_queueLock); 
            while (_isQueueEmpty == true && _isStop == false) { // 避免虚假唤醒
                _queueCV.wait(lock);
            }
            if (_isStop) {
                break;
            }
            std::vector<Container> tmpQueue(std::move(_dataQueue));
            _isQueueEmpty = true;
            lock.unlock();

            
            std::unique_lock<std::mutex> stepLock(_processStepsLock); 
            std::unique_lock<std::mutex> notifyLock(_nextProcessorsLock); 
            bool isOnlyOneNextP = false;
            if (!_nextProcessors.empty() 
                    && _nextProcessors.begin()+1 ==_nextProcessors.end()) {
                        isOnlyOneNextP = true;
                    }
            for (auto &vec: tmpQueue) {
                //run process chain
                for (auto &itor: _processSteps) {
                    (itor)->execute(vec);
                }

                //notify next processors
                if (isOnlyOneNextP) {
                    DataProcessor *next = _nextProcessors.front();
                    next->AddData(std::move(vec));
                } else {
                    for (auto &itor: _nextProcessors) {
                        (itor)->AddData(vec);
                    }
                }
            }
        }
        //std::cout << "exit loop" << std::endl;
    }

protected:
    std::mutex _nextProcessorsLock;
    std::vector<DataProcessor *> _nextProcessors;

    std::mutex _processStepsLock;
    std::list<ProcessStep<T, Container> *> _processSteps;

    std::mutex _queueLock;
    std::condition_variable _queueCV;
    bool _isQueueEmpty;
    std::vector<Container> _dataQueue;

    bool _isStop;
    std::thread *_loopThread;
};

END_SYNSENSE_NAMESPACE