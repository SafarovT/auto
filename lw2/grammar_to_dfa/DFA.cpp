#include "DFA.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <set>

#include <iostream>

using namespace std;

namespace
{
    const char OPEN_SYMBOL = '[';
    const char CLOSE_SYMBOL = ']';
    const char EPSILLON = 'E';
    const char NEW_STATE = 'Z';

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

    void PrintVector(vector<char> vector, ostream& output)
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
        char initState;
        vector<char> finalStates;
        int totalStates;
        vector<char> alphabet;
        DFA::NFAData transitions;
    };

    void PrintNfaInfo(const NFAInfo& nfaInfo)
    {
        cout << "Final states: ";
        PrintVector(nfaInfo.finalStates, cout);
        cout << endl << "Alphabet: ";
        PrintVector(nfaInfo.alphabet, cout);
        cout << endl << "Init state: " << nfaInfo.initState << endl << "Total states: " << nfaInfo.totalStates << endl;
        for (const auto& el : nfaInfo.transitions)
        {
            cout << el.first << ": ";
            for (const auto& tr : el.second)
            {
                cout << tr.first << " -> (";
                for (const auto& de : tr.second)
                {
                    cout << de << "|";
                }
                cout << "), ";
            }
            cout << endl;
        }
    }

    NFAInfo ConvertLeftGrammarToNFA(const Grammar& grammar)
    {
        NFAInfo nfaInfo;

        // Получаем переходы из грамматики
        auto grammarTransitions = grammar.GetTransitions();

        // Инициализируем начальное состояние (обычно это первая нетерминальная переменная)
        if (grammarTransitions.empty())
        {
            throw invalid_argument("Грамматика не содержит переходов.");
        }

        // Обновляем количество состояний в NFA
        nfaInfo.totalStates = grammarTransitions.size() + 1;

        // Инициализируем алфавит NFA
        for (const auto& [nonTerminal, transitions] : grammarTransitions)
        {
            for (const auto& [symbol, _] : transitions)
            {
                if (std::find(nfaInfo.alphabet.begin(), nfaInfo.alphabet.end(), symbol) == nfaInfo.alphabet.end())
                {
                    nfaInfo.alphabet.push_back(symbol);
                }
            }
        }

        // Обрабатываем переходы грамматики и формируем таблицу переходов NFA
        for (const auto& [nonTerminal, transitions] : grammarTransitions)
        {
            char fromState = nonTerminal;

            for (const auto& [symbol, nextNonTerminal] : transitions)
            {
                if (nextNonTerminal == Grammar::FINAL_SYMBOL)
                {
                    nfaInfo.transitions[NEW_STATE][symbol].push_back(nonTerminal);
                }
                else
                {
                    char toState = nextNonTerminal;
                    nfaInfo.transitions[toState][symbol].push_back(fromState);
                }
            }
        }

        // Устанавливаем начальное состояние (первый нетерминал в грамматике)
        nfaInfo.initState = NEW_STATE;
        nfaInfo.finalStates.push_back(grammar.GetTransitions()[0].first);

        PrintNfaInfo(nfaInfo);

        return nfaInfo;
    }

    NFAInfo ConvertRightGrammarToNFA(const Grammar& grammar)
    {
        NFAInfo nfaInfo;

        // Получаем переходы из грамматики
        auto grammarTransitions = grammar.GetTransitions();

        // Инициализируем начальное состояние (обычно это первая нетерминальная переменная)
        if (grammarTransitions.empty())
        {
            throw invalid_argument("Грамматика не содержит переходов.");
        }

        char finalState = Grammar::FINAL_SYMBOL;
        nfaInfo.finalStates.push_back(finalState);

        // Обновляем количество состояний в NFA
        nfaInfo.totalStates = grammar.GetTransitions().size() + 1;

        // Инициализируем алфавит NFA
        for (const auto& [nonTerminal, transitions] : grammarTransitions)
        {
            for (const auto& [symbol, _] : transitions)
            {
                if (std::find(nfaInfo.alphabet.begin(), nfaInfo.alphabet.end(), symbol) == nfaInfo.alphabet.end())
                {
                    nfaInfo.alphabet.push_back(symbol);
                }
            }
        }

        // Обрабатываем переходы грамматики и формируем таблицу переходов NFA
        for (const auto& [nonTerminal, transitions] : grammarTransitions)
        {
            char fromState = nonTerminal;

            for (const auto& [symbol, nextNonTerminal] : transitions)
            {
                if (nextNonTerminal == Grammar::FINAL_SYMBOL)
                {
                    nfaInfo.transitions[fromState][symbol].push_back(finalState);
                }
                else
                {
                    char toState = nextNonTerminal;
                    nfaInfo.transitions[fromState][symbol].push_back(toState);
                }
            }
        }

        // Устанавливаем начальное состояние (первый нетерминал в грамматике)
        nfaInfo.initState = grammarTransitions[0].first;
        nfaInfo.transitions[finalState] = {};

        PrintNfaInfo(nfaInfo);

        return nfaInfo;
    }

    NFAInfo ConvertGrammarToNFA(const Grammar& grammar)
    {
        return grammar.GetSide() == Grammar::Side::Left
            ? ConvertLeftGrammarToNFA(grammar)
            : ConvertRightGrammarToNFA(grammar);
    }
}

DFA::DFA(const Grammar& grammar)
{
    auto data = ConvertGrammarToNFA(grammar);
    m_alphabet = data.alphabet;

    SubsetConstruction(data.initState, data.finalStates, data.transitions);
    m_finalStates = GetDFAFinalStates(data.finalStates);
}

void DFA::Minimize()
{

}

void DFA::Print(ostream& output) const
{
    output << "Final states: ";
    PrintVector(m_finalStates, output);
    output << endl << "   ";
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
        if (std::find(m_finalStates.begin(), m_finalStates.end(), stateID) != m_finalStates.end()) {
            dotFile << "    " << stateID << " [shape=doublecircle];\n";
        }
        else {
            dotFile << "    " << stateID << " [shape=circle];\n";
        }
    }

    // Определяем переходы
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
    m_alphabet.push_back('E');

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