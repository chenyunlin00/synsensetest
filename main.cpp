#include <iostream>

#include "Event.h"
#include "Collection.h"
#include "DataProcessor.h"

using namespace synsense;
using namespace std;

void MyPrint(Event &e) {
    cout << "id " << e.id << " timestamp " << e.timestamp << endl;
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
    /**
    Collection<Event> coll;
    vector<Event> vec = {{0, 0}, {1, 1}, {1, 1}, {9, 9}, {2, 2}};
    coll.Add(vec);
    coll.Transform(MyPrint)
        .Filter([](const Event &e) { 
                if (e.id == 1) 
                    return false; 
                else 
                    return true; 
            })
        .Transform(MyPrint)
        .Add({100, 100})
        .Transform(MyPrint)
        .Sort(SortByIDDesc)
        .Transform(MyPrint)
        .Transform([](Event &e) {
            e.id++;
            e.timestamp++;
        })
        .Transform(MyPrint);

    cout << "======copy test start=====" << endl;
    Collection<Event> bcoll(coll);
    bcoll.Transform(MyPrint);
    cout << "======move test start======" << endl;
    Collection<Event> ccoll(move(coll));
    ccoll.Transform(MyPrint);
    cout << "======Data() test start======" << endl;
    vector<Event> *pVec = ccoll.Data();
    for (auto itor = pVec->begin(); itor != pVec->end(); itor++) {
        MyPrint(*itor);
    }
    cout << "======Clear() test start======" << endl;
    ccoll.Clear();
    cout << "coll size is " << ccoll.Data()->size() << endl;
    **/

    vector<Event> vec = {{0, 0}, {1, 1}, {1, 1}, {9, 9}, {2, 2}};
    vector<Event> vec2(move(vec));
    vec.push_back({98, 99});
    for (auto i: vec) {
        cout << i.timestamp << endl;
    }
    
    cout << "======test start======" << endl;
    DataProcessor<Event> p1, p2_1, p2_2, p3;
    p1.AddNextProcessor(&p2_1);
    p1.AddNextProcessor(&p2_2);
    p2_1.AddNextProcessor(&p3);
    p2_2.AddNextProcessor(&p3);

    //p1.Traverse(MyPrintVec).Run();
    p3.Traverse(MyPrintVec).Run();
    p2_1.Run();
    p2_2.Run();

    p1.AddData(vec);
    p1.AddData({{999, 999}});

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    cout << "Begin Stop All" << endl;
    p1.Stop();
    p2_1.Stop();
    p2_2.Stop();
    p3.Stop();
    cout << "After Stop All" << endl;
    return 0;
}