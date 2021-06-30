#include <iostream>

#include "Event.h"
#include "Collection.h"

using namespace synsense;
using namespace std;

void MyPrint(Event e) {
    cout << "id " << e.id << " timestamp " << e.timestamp << endl;
}

bool SortByIDDesc(const Event &a, const Event &b) {
    return a.id > b.id;
}

int main() {
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
    return 0;
}