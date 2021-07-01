#include <iostream>

#include "Event.h"
#include "Collection.h"
#include "DataProcessor.h"

using namespace synsense;
using namespace std;

void MyPrint(Event &e) {
    cout << "id " << e.id << " timestamp " << e.timestamp << endl;
}

void AddOne(vector<Event> &vec) {
    for (auto &e: vec) {
        e.id++;
        e.timestamp++;
    }
}

void MyPrintVec(vector<Event> &vec) {
    for (auto &e: vec) {
        cout << "id " << e.id << " timestamp " << e.timestamp << endl;
    }
}

bool SortByIDDesc(const Event &a, const Event &b) {
    return a.id > b.id;
}

int main() {
    vector<Event> vec = {{0, 0}, {1, 1}, {1, 1}, {9, 9}, {2, 2}};
    vec.push_back({98, 99});
    cout << "======test start======" << endl;
    DataProcessor<Event> p1, p2_1, p2_2, p3;
    vector<DataProcessor<Event> *> processors = {&p1, &p2_1, &p2_2, &p3};
    p1.AddNextProcessor(&p2_1);
    p1.AddNextProcessor(&p2_2);
    p2_1.AddNextProcessor(&p3);
    p2_2.AddNextProcessor(&p3);

    //p1.Traverse(MyPrintVec).Run();
    p1.Filter([](const Event &e) {
        if (e.id == 1) {
            return false;
        }
        return true;
    }).Run();
    p3.Traverse(MyPrintVec).Run();
    p2_1.Sort(SortByIDDesc).Run();
    p2_2.Traverse(AddOne).Run();

    p1.AddData(move(vec));
    p1.AddData({{999, 999}});

    p2_1.AddData({{333, 333} , {777, 777}, {666, 666}});

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    cout << "Begin Stop All" << endl;
    for (auto &proc: processors) {
        proc->Stop();
    }
    cout << "After Stop All" << endl;
    return 0;
}