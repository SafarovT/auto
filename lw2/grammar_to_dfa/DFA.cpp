#include "DFA.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <set>

using namespace std;

namespace
{
    const char OPEN_SYMBOL = '[';
    const char CLOSE_SYMBOL = ']';
    const char EPSILLON = 'E';

    bool HasVector(const vector<char>& vector, char symb)
    {
        for (const auto& el : vector)
        {
            if (el == symb)
            {
                return true;
            }
        }

        return false;
    }

    void PrintVector(vector<int> vector, ostream& output)
    {
        output << "[";
        for (const auto& el : vector)
        {
            output << el << ",";
        }

        output << "]";
    }

    vector<char> EClosure(const vector<char>& state, const DFA::NFAData& nfa)
    {
        vector<char> eClosure;
        stack<char> myStack;

        for (const auto& el : state)
        {
            myStack.push(el);
        }

        for (const auto& el : state)
        {
            eClosure.push_back(el);
        }

        while (!myStack.empty())
        {
            char current = myStack.top();
            myStack.pop();

            auto it = nfa.find(current);
            if (it != nfa.end() && it->second.find('E') != it->second.end())
            {
                vector<char> eMoves = it->second.at('E');
                for (const auto& el : eMoves)
                {
                    if (!HasVector(eClosure, el))
                    {
                        eClosure.push_back(el);
                        myStack.push(el);
                    }
                }
            }
        }

        sort(eClosure.begin(), eClosure.end());

        return eClosure;
    }
    
    vector<char> GetReachableStates(const vector<char>& state, char move, const DFA::NFAData& nfa)
    {
        vector<char> result;
        for (const auto& el : state)
        {
            const auto& transitions = nfa.at(el);
            auto it = transitions.find(move);
            if (it != transitions.end())
            {
                for (const auto& reachable : it->second)
                {
                    if (!HasVector(result, reachable))
                    {
                        result.push_back(reachable);
                    }
                }
            }
        }

        return result;
    }

    struct NFAInfo
    {
        int initState;
        vector<int> finalStates;
        int totalStates;
        vector<char> alphabet;
        DFA::NFAData transitions;
    };

    NFAInfo ConvertGrammarToNFA(const Grammar& grammar)
    {
        NFAInfo nfaInfo;

        auto transitions = grammar.GetTransitions();

        nfaInfo.initState = 0;
        nfaInfo.totalStates = static_cast<int>(transitions.size());
        nfaInfo.alphabet = {};

        set<char> alphabetSet;

        for (const auto& transition : transitions)
        {
            char state = transition.first;

            for (const auto& rule : transition.second)
            {
                char symbol = rule.first;
                char nextState = rule.second;

                nfaInfo.transitions[state][symbol].push_back(nextState);

                if (symbol != Grammar::FINAL_SYMBOL)
                {
                    alphabetSet.insert(symbol);
                }
            }
        }

        nfaInfo.alphabet.assign(alphabetSet.begin(), alphabetSet.end());

        if (grammar.GetSide() == Grammar::Side::Right)
        {
            for (const auto& [state, transitions] : nfaInfo.transitions)
            {
                for (const auto& [symbol, nextStates] : transitions)
                {
                    if (symbol == Grammar::FINAL_SYMBOL)
                    {
                        nfaInfo.finalStates.push_back(state);
                    }
                }
            }
        }

        return nfaInfo;
    }
}

DFA::DFA(const Grammar& grammar)
{

}

void DFA::Minimize() const
{

}

void DFA::Print(ostream& output) const
{
    output << "   ";
    for (auto k = m_alphabet.begin(); k != m_alphabet.end() - 1; k++)
    {
        output << *k << "    ";
    }
    output << endl;

    for (const auto& [stateID, state] : m_data)
    {
        output << stateID << " ";
        for (auto k = m_alphabet.begin(); k != m_alphabet.end() - 1; k++)
        {
            output << OPEN_SYMBOL;
            if (state.moves.at(*k) != -1)
            {
                output << state.moves.at(*k);
            }
            output << CLOSE_SYMBOL << "   ";
        }
        output << endl;
    }
}

void DFA::Display(const string& fileName) const
{
    ofstream dotFile("output.dot");

    if (!dotFile.is_open()) {
        cerr << "Error: Unable to open file for writing: output.dot" << endl;
        return;
    }

    dotFile << "digraph DFA {\n";
    dotFile << "    rankdir=LR;\n";
    dotFile << "    node [shape = circle];\n";

    for (const auto& [stateID, state] : m_data)
    {
        for (const char& symbol : m_alphabet)
        {
            auto it = state.moves.find(symbol);
            if (it != state.moves.end() && it->second != -1)
            {
                dotFile << "    " << stateID << " -> " << it->second << " [label=\"" << symbol << "\"];\n";
            }
        }
    }

    dotFile << "}";

    dotFile.close();
    string command = "dot -Tpng output.dot -o " + fileName + ".png";
    system(command.c_str());

    cout << "Result in: " << fileName << ".png" << endl;
}

optional<char> DFA::FindUnmarked() const
{
    for (const auto& [stateID, current] : m_data)
    {
        if (!current.marked)
        {
            return stateID;
        }
    }

    return nullopt;
}

optional<char> DFA::FindState(const vector<char>& state) const
{
    for (const auto& [stateID, current] : m_data)
    {
        if (current.states == state)
        {
            return stateID;
        }
    }

    return nullopt;
}

vector<char> DFA::GetDFAFinalStates(const vector<char>& finalStates) const
{
    vector<char> finals;
    for (const auto& [stateID, dfaState] : m_data)
    {
        for (auto const& el : finalStates)
        {
            if (HasVector(dfaState.states, el))
            {
                finals.push_back(stateID);
            }
        }
    }

    return finals;
}

void DFA::SubsetConstruction(char initialState, const vector<char>& finalStates, const DFA::NFAData& nfa)
{
    char currentDFAStateNumber = 'A';
    vector<char> initialStateVector;
    initialStateVector.push_back(initialState);

    vector<char> eClosure = EClosure(initialStateVector, nfa);

    DFA::DFAState initState;
    initState.marked = false;
    initState.states = eClosure;

    m_data[currentDFAStateNumber] = initState;
    currentDFAStateNumber++;

    auto unmarkedState = FindUnmarked();
    while (unmarkedState != nullopt)
    {
        m_data[*unmarkedState].marked = true;

        for (auto w = m_alphabet.begin(); w != m_alphabet.end() - 1; w++)
        {
            vector<char> move = GetReachableStates(m_data[*unmarkedState].states, *w, nfa);
            vector<char> alphaMove = EClosure(move, nfa);

            auto foundState = FindState(alphaMove);
            if (foundState != nullopt)
            {
                m_data[*unmarkedState].moves[*w] = *foundState;
            }
            else
            {
                if (!alphaMove.empty())
                {
                    DFAState newState;
                    newState.marked = false;
                    newState.states = alphaMove;
                    m_data[currentDFAStateNumber] = newState;
                    m_data[*unmarkedState].moves[*w] = currentDFAStateNumber;
                    currentDFAStateNumber++;
                }
                else
                {
                    m_data[*unmarkedState].moves[*w] = -1;
                }
            }
        }

        unmarkedState = FindUnmarked();
    }
}