#include <iostream>
#include <set>
#include <queue>
#include <set>
#include <map>
#include <stack>

using namespace std;

struct nst
{
    vector<int> a[2], e;
    bool f = 0;
};

vector<nst> nfa;

struct dst
{
    int a[2] = { -1,-1 };
    bool f = 0;
};

vector<dst> dfa;

stack<int> st;

int nfa_size, dfa_size;
string dispregex;

struct nst init_nfa_state;

struct dst init_dfa_state;

void epsilon_closure(int state, set<int>& si)
{
    for (unsigned int i = 0; i < nfa[state].e.size(); i++)
    {
        if (si.count(nfa[state].e[i]) == 0) {
            si.insert(nfa[state].e[i]);
            epsilon_closure(nfa[state].e[i], si);
        }
    }
}

set<int> state_change(int c, set<int>& si) {
    set<int> temp;
    if (c == 1) {
        for (std::set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            for (unsigned int j = 0; j < nfa[*it].a[0].size(); j++) {
                temp.insert(nfa[*it].a[0][j]);
            }
        }
    }
    else {
        for (std::set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            for (unsigned int j = 0; j < nfa[*it].a[1].size(); j++) {
                temp.insert(nfa[*it].a[1][j]);
            }
        }
    }
    return temp;
}

void nfa_to_dfa(set<int>& si, queue<set<int> >& que, int start_state) {
    map<set<int>, int> mp;
    mp[si] = -1;
    set<int> temp1;
    set<int> temp2;
    int ct = 0;
    si.clear();
    si.insert(0);
    epsilon_closure(start_state, si);
    if (mp.count(si) == 0) {
        mp[si] = ct++;
        que.push(si);
    }
    int p = 0;
    bool f1 = false;
    while (que.size() != 0) {
        dfa.push_back(init_dfa_state);
        si.empty();
        si = que.front();
        f1 = false;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            if (nfa[*it].f == true)
                f1 = true;
        }
        dfa[p].f = f1;
        temp1 = state_change(1, si);
        si = temp1;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            epsilon_closure(*it, si);
        }
        if (mp.count(si) == 0) {
            mp[si] = ct++;
            que.push(si);
            dfa[p].a[0] = ct - 1;
        }
        else {
            dfa[p].a[0] = mp.find(si)->second;
        }
        temp1.clear();

        si = que.front();
        temp2 = state_change(2, si);
        si = temp2;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            epsilon_closure(*it, si);
        }
        if (mp.count(si) == 0) {
            mp[si] = ct++;
            que.push(si);
            dfa[p].a[1] = ct - 1;
        }
        else {
            dfa[p].a[1] = mp.find(si)->second;
        }
        temp2.clear();
        que.pop();
        p++;
    }
    for (int i = 0; i < p; i++) {
        if (dfa[i].a[0] == -1)dfa[i].a[0] = p;
        if (dfa[i].a[1] == -1)dfa[i].a[1] = p;
    }
    dfa.push_back(init_dfa_state);
    dfa[p].a[0] = p;
    dfa[p].a[1] = p;
    //cout<<p<<endl;
}