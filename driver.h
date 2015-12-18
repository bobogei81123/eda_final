#ifndef DRIVER_H_GAURD
#define DRIVER_H_GAURD

#include <bits/stdc++.h>
using namespace std;

typedef vector<bool> vbool;
typedef vector<string> slist;

class Gate {
public:
    Gate () { psuedo = false; }
    string name;
    vector<string> inputs;
    vector<string> outputs;
    bool psuedo;

    void debug_print();
    virtual vbool feed(const vbool &in) {}
} ;

class AndGate : public Gate {
    virtual vbool feed(const vbool &in);
};

class OrGate : public Gate {
    virtual vbool feed(const vbool &in);
};

class NandGate : public Gate {
    virtual vbool feed(const vbool &in);
};

class NorGate : public Gate {
    virtual vbool feed(const vbool &in);
};

class NotGate : public Gate {
    virtual vbool feed(const vbool &in);
};

class Module : public Gate {
public:
    pair<string, int> fault;
    string fault_io;
    bool fault_type;
    vector<Gate*> gates;
    virtual vbool feed(const vbool &in);

    void check();
    bool find_output(string);

    map<string, bool> mem_map;
    map<string, Gate*> which_output;
};

class Driver {
    public:
    vector<Module> modules;

};



#endif
