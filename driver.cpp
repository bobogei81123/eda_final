#include "driver.h"

template<typename T>
ostream& operator << (ostream &o, vector<T> v) {
    cout << '[';
    for (auto x: v) 
        cout << x << ", ";
    cout << ']';
    return o;
}

void Gate::debug_print() {
    cout << "Module : " << name << endl;
    cout << "Input : " << inputs << endl;
    cout << "Output : " << outputs << endl;
}


vbool AndGate::feed(const vbool &in) {
    bool res = true;
    for (auto x: in)
        res &= x;
    return {res};
}

vbool OrGate::feed(const vbool &in) {
    bool res = false;
    for (auto x: in)
        res |= x;
    return {res};
}

vbool NandGate::feed(const vbool &in) {
    bool res = true;
    for (auto x: in)
        res &= x;
    return {!res};
}

vbool NorGate::feed(const vbool &in) {
    bool res = false;
    for (auto x: in)
        res |= x;
    return {!res};
}

vbool NotGate::feed(const vbool &in) {
    return {!in[0]};
}

void Module::check() {
    regex re("G\\d+");
    for (int i=0; i<(int)inputs.size(); i++) {
        if (!regex_match(inputs[i], re)) {
            swap(inputs[i], inputs.back());
            inputs.pop_back();
        }
    }
    for (auto g: gates) {
        for (auto o: g->outputs) {
            which_output[o] = g;
        }
        if (g->psuedo) {
            for (auto o: g->outputs) {
                inputs.push_back(o);
            }
            for (auto i: g->inputs) {
                if (!regex_match(i, re)) continue;
                outputs.push_back(i);
            }
        }
    }
    sort(inputs.begin(), inputs.end());
    sort(outputs.begin(), outputs.end());
}

bool Module::find_output(string wire) {
    if (mem_map.find(wire) != mem_map.end()) {
        return mem_map[wire];
    }


    Gate *g = which_output[wire];
    vbool in;
    bool is_fg = (g->name == fault.first);
    if (is_fg and fault.second == g->inputs.size()) {
        mem_map[wire] = fault_type;
    }
    for (int i=0; i<(int)g->inputs.size(); i++) { if (is_fg and i == fault.second) 
            in.push_back(fault_type);
        else
            in.push_back(find_output(g->inputs[i]));
    }

    auto out = g->feed(in);
    for (int i=0; i<(int)g->outputs.size(); i++) {
        mem_map[g->outputs[i]] = out[i];
    }

    return mem_map[wire];
}

vbool Module::feed(const vbool &in) {

    mem_map.clear();
    for (int i=0; i<(int)in.size(); i++) {
        mem_map[inputs[i]] = in[i];
    }
    if (fault_io != "")
        mem_map[fault_io] = fault_type;

    for (auto x: outputs) {
        find_output(x);
    }

    vector<bool> res;
    for (auto x: outputs) {
        res.push_back(mem_map[x]);
    }
    return res;
}
