#include<bits/stdc++.h>
#include "driver.h"
#include <regex>

extern int yyparse();
extern FILE *yyin;
extern map<string, Module*> module_map;

using namespace std;

int main(int argc, char **argv){
    assert(argc == 3);
    string filename(argv[1]);
    FILE *fp = fopen(argv[1], "r");
    yyin = fp;
    regex re("([^.]+).");
    smatch match;
    regex_search(filename, match, re);
    yyparse();

    vector<bool> v;
    ifstream fin;
    fin.open(argv[2]);
    int t;
    while (fin >> t) {
        v.push_back(t);
    }
    auto top = module_map[match[1]];
    top->check();

    auto normal_output = top->feed(v);
    vector<string> detected, undetected;

    auto get_fname = [](string wr, string gate, bool type) -> string {
        return wr + (gate == "" ? "" : "_" + gate) + "_sa" + (type ? "1" : "0");
    };

    for (int i=0; i<(int)top->inputs.size(); i++) {
        top->fault_io = top->inputs[i];
        top->fault_type = true;
        auto out = top->feed(v);
        if (out == normal_output) {
            undetected.push_back(get_fname(top->inputs[i], "", true));
        } else {
            detected.push_back(get_fname(top->inputs[i], "", true));
        }
        top->fault_type = false;
        out = top->feed(v);
        if (out == normal_output) {
            undetected.push_back(get_fname(top->inputs[i], "", false));
        } else {
            detected.push_back(get_fname(top->inputs[i], "", false));
        }
    }

    for (int i=0; i<(int)top->outputs.size(); i++) {
        top->fault_io = top->outputs[i];
        top->fault_type = true;
        auto out = top->feed(v);
        if (out == normal_output) {
            undetected.push_back(get_fname(top->outputs[i], "", true));
        } else {
            detected.push_back(get_fname(top->outputs[i], "", true));
        }
        top->fault_type = false;
        out = top->feed(v);
        if (out == normal_output) {
            undetected.push_back(get_fname(top->outputs[i], "", false));
        } else {
            detected.push_back(get_fname(top->outputs[i], "", false));
        }
    }

    top->fault_io = "";
    for (auto g: top->gates) {
        if (g->psuedo) continue;
        int ws = g->inputs.size();
        for (int i=0; i<ws; i++) {
            top->fault = {g->name, i};
            top->fault_type = false;
            auto out = top->feed(v);
            if (out == normal_output) {
                undetected.push_back(get_fname(g->inputs[i], g->name, false));
            } else {
                detected.push_back(get_fname(g->inputs[i], g->name, false));
            }
            top->fault_type = true;
            out = top->feed(v);
            if (out == normal_output) {
                undetected.push_back(get_fname(g->inputs[i], g->name, true));
            } else {
                detected.push_back(get_fname(g->inputs[i], g->name, true));
            }
        }

        ws = g->outputs.size();
        for (int i=0; i<ws; i++) {
            top->fault = {g->name, i};
            top->fault_type = false;
            auto out = top->feed(v);
            if (out == normal_output) {
                undetected.push_back(get_fname(g->outputs[i], g->name, false));
            } else {
                detected.push_back(get_fname(g->outputs[i], g->name, false));
            }
            top->fault_type = true;
            out = top->feed(v);
            if (out == normal_output) {
                undetected.push_back(get_fname(g->outputs[i], g->name, true));
            } else {
                detected.push_back(get_fname(g->outputs[i], g->name, true));
            }
        }
    }

    cout << "Detected = " << detected.size() << endl;
    for (auto x: detected) {
        cout << x << ' ';
    } cout << endl;

    cout << "Undetected = " << undetected.size() << endl;
    for (auto x: undetected) {
        cout << x << ' ';
    } cout << endl;

}

