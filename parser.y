%defines
%define parse.error verbose

%{
#include <bits/stdc++.h>
#include "driver.h"

extern int yylex();
using namespace std;
void yyerror(const char *s) { cout << s << endl; }
map<string, Module*> module_map;


#include "bundle.cpp"
%}

%union {
    string *strval;
    slist *strs;
    Module *module;
    Gate *gate; 
} 
%token 
    <strval> ID
    MODBEGIN MODEND INPUT OUTPUT WIRE REG
    AND OR NAND NOR NOT ALWAYS EDGE BEG END

%type <module> module
%type <strs> vlist mod_args
%type <gate> gate_desc

%%
modules:
       | modules module
       ;

module:
      MODBEGIN ID 
      {
        $<module>$ = new Module();
        $<module>$->name = *$2;
        module_map[*$2] = $<module>$;
        delete $2;
        bundle::cur_mod = $<module>$;
      } [mod]
      mod_args ';' descs MODEND
      {
        $$ = $<module>mod;
        $$->debug_print();
        for (auto x: $$->gates)
            cout << x->name << ' ';
            cout << endl;
      }
      ;

descs:
    | descs desc

desc: INPUT vlist ';' 
    {
        for (auto x: *$2)
            bundle::cur_mod->inputs.push_back(x);
    }
    | OUTPUT vlist ';' 
    {
        for (auto x: *$2)
            bundle::cur_mod->outputs.push_back(x);
    }
    | REG vlist ';'
    | WIRE vlist ';'
    | gate_desc
    {
        bundle::cur_mod->gates.push_back($1);
    }
    | always_block

always_block: ALWAYS '@' '(' edge_cap ')' always_stmt

edge_cap: EDGE ID

always_stmt: statement
           | BEG statements END

statements: 
          | statements statement
    
statement:
         ID '=' ID ';'

gate_desc: AND ID mod_args ';'
         {
            $$ = new AndGate();
            $$->name = *$2;
            auto &t = (*$3);
            $$->inputs.push_back(t[2]);
            $$->inputs.push_back(t[1]);
            $$->outputs.push_back(t[0]);
         }
         | OR ID mod_args ';'
         {
            $$ = new OrGate();
            $$->name = *$2;
            auto &t = (*$3);
            $$->inputs.push_back(t[2]);
            $$->inputs.push_back(t[1]);
            $$->outputs.push_back(t[0]);
         }
         | NOR ID mod_args ';'
         {
            $$ = new NorGate();
            $$->name = *$2;
            auto &t = (*$3);
            $$->inputs.push_back(t[2]);
            $$->inputs.push_back(t[1]);
            $$->outputs.push_back(t[0]);
         }
         | NAND ID mod_args ';'
         {
            $$ = new NandGate();
            $$->name = *$2;
            auto &t = (*$3);
            $$->inputs.push_back(t[2]);
            $$->inputs.push_back(t[1]);
            $$->outputs.push_back(t[0]);
         }
         | NOT ID mod_args ';'
         {
            $$ = new NotGate();
            $$->name = *$2;
            auto &t = (*$3);
            $$->inputs.push_back(t[1]);
            $$->outputs.push_back(t[0]);
         }
         | ID ID mod_args ';'
         {
            $$ = new Gate();
            $$->name = *$2;
            $$->psuedo = true;
            auto &t = (*$3);
            $$->inputs.push_back(t[2]);
            $$->outputs.push_back(t[1]);
         }

         

/*gate:*/
    /*OR | NAND | NOR | ID*/

mod_args:
        '(' vlist ')'
        { $$ = $vlist; }

vlist: { $$ = new slist(); }
    | ID { $$ = new slist(); $$->push_back(*$1);}
    | vlist ',' ID {
        $$->push_back(*$3);
    }
%%
    

