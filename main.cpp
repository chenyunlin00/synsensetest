#include <iostream>
#include<chrono>

#include "Event.h"
#include "Collection.h"
#include "DataProcessor.h"

using namespace synsense;
using namespace std;

#define PROCESSOR_NUM 1000

long long g_testStartTime = 0;

void MyPrint(Event &e) {
    cout << "id " << e.id << " timestamp " << e.timestamp << endl;
}

void AddOne(vector<Event> &vec) {
    for (auto &e: vec) {
        e.id++;
        e.timestamp++;
    }
}

void CalculateDuration(vector<Event> &vec) {
    auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

    long long duration = timeNow.count() - g_testStartTime;
    cout << "duration = " << duration << " ms" << endl;
    cout << "id " << vec[0].id << " timestamp " << vec[0].timestamp << endl;
}

void MyPrintVec(vector<Event> &vec) {
    for (auto &e: vec) {
        cout << "id " << e.id << " timestamp " << e.timestamp << endl;
    }
}

void MyPrintVec2(vector<Event> &vec) {
    for (auto &e: vec) {
        cerr << "p2_2: id " << e.id << " timestamp " << e.timestamp << endl;
    }
}

bool SortByIDDesc(const Event &a, const Event &b) {
    return a.id > b.id;
}

void test1() {
    vector<Event> vec = {{0, 0}, {1, 1}, {1, 1}, {9, 9}, {2, 2}};
    vec.push_back({98, 99});
    cout << "======test1 start======" << endl;
    DataProcessor<Event> p1, p2_1, p2_2, p3;
    vector<DataProcessor<Event> *> processors = {&p1, &p2_1, &p2_2, &p3};

    //connect all the processors
    p1.AddNextProcessor(&p2_1);
    p1.AddNextProcessor(&p2_2);
    p2_1.AddNextProcessor(&p3);
    p2_2.AddNextProcessor(&p3);

    //run all the processors
    p1.Filter([](const Event &e) {
        if (e.id == 1) {
            return false;
        }
        return true;
    }).Run();
    p3.Traverse(MyPrintVec).Run();
    p2_1.Sort(SortByIDDesc).Run();
    p2_2.Traverse(AddOne).Run();
    
    //add init data
    p1.AddData(move(vec));
    p1.AddData({{999, 999}});

    //test add process function when thread is running
    p2_2.Traverse(MyPrintVec2); 

    //add data to middle layer
    p2_1.AddData({{333, 333} , {777, 777}, {666, 666}});

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    //cout << "Begin Stop All" << endl;
    for (auto &proc: processors) {
        proc->Stop();
    }
    //cout << "After Stop All" << endl;
}

void test2() {
    cout << "======test2 start======" << endl;
    DataProcessor<Event> *processors = new DataProcessor<Event>[PROCESSOR_NUM];
    for (int i=0; i<PROCESSOR_NUM-1; i++) {
        (processors[i]).AddNextProcessor(&(processors[i+1]));
    }
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Traverse(AddOne).Run();
    }

    processors[PROCESSOR_NUM-1].Traverse(CalculateDuration);

    auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

    g_testStartTime = timeNow.count();
    (processors[0]).AddData({{0, 0}});

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    //cout << "Begin Stop All" << endl;
    for (int i=0; i<PROCESSOR_NUM; i++) {
        (processors[i]).Stop();
    }
    delete []processors;

}

int main(int argc, char **argv) {
    test1();
    test2();
    return 0;
}