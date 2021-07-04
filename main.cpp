#include <iostream>
#include <chrono>
#include <list>

#include "Event.h"
#include "Collection.h"
#include "DataProcessor.h"

using namespace synsense;
using namespace std;

#define PROCESSOR_NUM (1000)
#define TESTDATA_NUM  (PROCESSOR_NUM*10)

long long g_testStartTime = 0;

void MyPrint(Event &e) {
    cout << "id " << e.id << " timestamp " << e.timestamp << endl;
}

template <typename Container=vector<Event>>
void AddOne(Container &vec) {
    for (auto &e: vec) {
        e.id++;
        e.timestamp++;
    }
}

template <typename Container=vector<Event>>
void CalculateDuration(Container &vec) {
    auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

    long long duration = timeNow.count() - g_testStartTime;
    cout << "duration = " << duration << " ms" << endl;
  //  cout << "id " << vec[0].id << " timestamp " << vec[0].timestamp << endl;
}

template <typename Container=vector<Event>>
void MyPrintVec(Container &vec) {
    for (auto &e: vec) {
        cout << "id " << e.id << " timestamp " << e.timestamp << endl;
    }
}

template <typename Container=vector<Event>>
void MyPrintVec2(Container&vec) {
    for (auto &e: vec) {
        cerr << "p2_2: id " << e.id << " timestamp " << e.timestamp << endl;
    }
}

bool SortByIDDesc(const Event &a, const Event &b) {
    return a.id > b.id;
}

template <typename T, typename Container>
void testBasicFunction() {
    Container vec = {{0, 0}, {1, 1}, {1, 1}, {9, 9}, {2, 2}};
    vec.push_back({98, 99});
    cout << "======test1 start======" << endl;
    DataProcessor<T, Container> p1, p2_1, p2_2, p3;
    vector<DataProcessor<T, Container> *> processors = {&p1, &p2_1, &p2_2, &p3};

    //connect all the processors
    p1.AddNextProcessor(&p2_1);
    p1.AddNextProcessor(&p2_2);
    p2_1.AddNextProcessor(&p3);
    p2_2.AddNextProcessor(&p3);

    //run all the processors
    p1.Filter([](const T &e) {
        if (e.id == 1) {
            return false;
        }
        return true;
    }).Run();
    p3.Traverse(MyPrintVec<Container>).Run();
    p2_1.Sort(SortByIDDesc).Run();
    p2_2.Traverse(AddOne<Container>).Run();
    
    //add init data
    p1.AddData(move(vec));
    p1.AddData({{999, 999}});

    //test add process function when thread is running
    p2_2.Traverse(MyPrintVec2<Container>); 

    //add data to middle layer
    p2_1.AddData({{333, 333} , {777, 777}, {666, 666}});

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //cout << "Begin Stop All" << endl;
    for (auto &proc: processors) {
        proc->Stop();
    }
    //cout << "After Stop All" << endl;
}

template <typename T, typename Container>
void testTraversePerf(const char *name) {
    cout << "======testTraversePerf start====== using " << name << endl;
    DataProcessor<T, Container> *processors = new DataProcessor<T, Container>[PROCESSOR_NUM];
    for (int i=0; i<PROCESSOR_NUM-1; i++) {
        (processors[i]).AddNextProcessor(&(processors[i+1]));
    }
    Container data;
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Traverse(AddOne<Container>).Run();
    }
    for (int i=0; i<TESTDATA_NUM; i++) {
        T t(i, i);
        data.push_back(t);
    }

    processors[PROCESSOR_NUM-1].Traverse(CalculateDuration<Container>);

    auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

    g_testStartTime = timeNow.count();
    (processors[0]).AddData(move(data));

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    //cout << "Begin Stop All" << endl;
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Stop();
    }
    delete []processors;

}

template <typename T, typename Container>
void testFilterPerf(const char *name) {
    cout << "======testFilterPerf start====== using " << name << endl;
    DataProcessor<T, Container> *processors = new DataProcessor<T, Container>[PROCESSOR_NUM];
    for (int i=0; i<PROCESSOR_NUM-1; i++) {
        (processors[i]).AddNextProcessor(&(processors[i+1]));
    }
    int ratio = TESTDATA_NUM/PROCESSOR_NUM;
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Filter([=](const T &e) {
            if (e.id >= i*ratio && e.id < (i+1)*ratio) {
                return false;
            } else {
                return true;
            }
        }).Run();
    }

    Container data;
    for (int i=0; i<TESTDATA_NUM; i++) {
        T t(i, i);
        data.push_back(t);
    }

    processors[PROCESSOR_NUM-1].Traverse(CalculateDuration<Container>);

    auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

    g_testStartTime = timeNow.count();
    (processors[0]).AddData(move(data));

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    //cout << "Begin Stop All" << endl;
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Stop();
    }
    delete []processors;

}

int main(int argc, char **argv) {
    testBasicFunction<Event, vector<Event>>();
    testBasicFunction<Event, list<Event>>();
    testTraversePerf<Event, vector<Event>>("vector");
    testTraversePerf<Event, list<Event>>("list");
    testFilterPerf<Event, vector<Event>>("vector");
    testFilterPerf<Event, list<Event>>("list");
    return 0;
}