#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <utility>
#include <algorithm>
#include <optional>
#include <stdexcept>

// скрестить с минимизацией
// добавить отображение финальных состояний + не менять цифры

using namespace std;

const char OPEN_SYMBOL = '[';
const char CLOSE_SYMBOL = ']';

struct DFAState
{
    bool marked;
    vector<int> states;
    map<char, int> moves;
};

using DFA = map<int, DFAState>;
using NFA = map<int, map<char, vector<int>>>;

bool HasVector(const vector<int>& vector, int num)
{
    for (const auto& el : vector)
    {
        if (el == num)
        {
            return true;
        }
    }

    return false;
}

void PrintVector(vector<int> vector)
{
    cout << "[";
    for (const auto& el : vector)
    {
        cout << el << ",";
    }

    cout << "]";
}

optional<int> FindUnmarked(const DFA& dfa)
{
    size_t size = dfa.size();
    for (size_t i = 0; i < size; i++)
    {
        const DFAState& current = dfa.at(i);
        if (!current.marked)
        {
            return i;
        }
    }

    return nullopt;
}

optional<int> FindState(const vector<int>& state, const DFA& dfa)
{
    size_t size = dfa.size();
    for (size_t i = 0; i < size; i++)
    {
        const DFAState& current = dfa.at(i);
        if (current.states == state)
        {
            return i;
        }
    }

    return nullopt;
}

vector<int> EClosure(const vector<int>& state, const NFA& nfa)
{
    vector<int> eClosure;
    stack<int> myStack;

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
        int current = myStack.top();
        myStack.pop();

        auto it = nfa.find(current);
        if (it != nfa.end() && it->second.find('E') != it->second.end())
        {
            vector<int> eMoves = it->second.at('E');
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

vector<int> GetReachableStates(const vector<int>& state, char move, const NFA& nfa)
{
    vector<int> result;
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

vector<int> GetDFAFinalStates(const DFA& dfa, const vector<int>& finalStates)
{
    vector<int> finals;
    for (const auto& [stateID, dfaState] : dfa)
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

void SubsetConstruction(int initialState, const vector<int>& finalStates, const NFA& nfa, DFA& dfa, const vector<char>& alphabet)
{
    int currentDFAStateNumber = 0;
    vector<int> initialStateVector;
    initialStateVector.push_back(initialState);

    vector<int> eClosure = EClosure(initialStateVector, nfa);

    DFAState initState;
    initState.marked = false;
    initState.states = eClosure;

    dfa[currentDFAStateNumber] = initState;
    currentDFAStateNumber++;

    auto unmarkedState = FindUnmarked(dfa);
    while (unmarkedState != nullopt)
    {
        dfa[*unmarkedState].marked = true;

        for (auto w = alphabet.begin(); w != alphabet.end() - 1; w++)
        {
            vector<int> move = GetReachableStates(dfa[*unmarkedState].states, *w, nfa);
            vector<int> alphaMove = EClosure(move, nfa);

            auto foundState = FindState(alphaMove, dfa);
            if (foundState != nullopt)
            {
                dfa[*unmarkedState].moves[*w] = *foundState;
            }
            else
            {
                if (!alphaMove.empty())
                {
                    DFAState newState;
                    newState.marked = false;
                    newState.states = alphaMove;
                    dfa[currentDFAStateNumber] = newState;
                    dfa[*unmarkedState].moves[*w] = currentDFAStateNumber;
                    currentDFAStateNumber++;
                }
                else
                {
                    dfa[*unmarkedState].moves[*w] = -1;
                }
            }
        }

        unmarkedState = FindUnmarked(dfa);
    }
}

void MoveToData(istringstream& iss)
{
    while (iss.peek() != OPEN_SYMBOL)
    {
        iss.ignore();
    }

    iss.ignore();
}

void ReadFile(const string& fileName, int& initState, vector<int>& finalStates, int& totalStates, vector<char>& alphabet, NFA& stateTable)
{
    string line;
    ifstream input(fileName);

    if (!input.is_open())
    {
        throw invalid_argument("Ошибка при открытии файла");
    }

    std::getline(input, line);
    istringstream iss(line);

    MoveToData(iss);
    iss >> initState;

    std::getline(input, line);
    iss.str(line);

    MoveToData(iss);
    int finalState;
    iss >> finalState;
    finalStates.push_back(finalState);

    while (iss.peek() != CLOSE_SYMBOL)
    {
        if (iss.peek() == ',')
        {
            iss.ignore();
        }
        iss >> finalState;
        finalStates.push_back(finalState);
    }

    std::getline(input, line);
    iss.str(line);

    MoveToData(iss);
    iss >> totalStates;

    std::getline(input, line);
    char symbol;
    istringstream alphabetStream(line);

    while (alphabetStream >> symbol)
    {
        alphabet.push_back(symbol);
    }

    std::getline(input, line);

    for (int i = 1; i <= totalStates; i++)
    {
        istringstream stateIss(line);
        map<char, vector<int>> stateMoves;

        for (int j = 0; j < alphabet.size(); j++)
        {
            int state;
            string movesStr;
            vector<int> states;

            if (j == 0)
            {
                stateIss.ignore();
                stateIss.ignore();
            }
            else
            {
                stateIss.ignore();
            }

            stateIss >> movesStr;
            movesStr = movesStr.substr(1, movesStr.size() - 2);

            if (movesStr != "")
            {
                istringstream moveIss(movesStr);

                while (moveIss >> state)
                {
                    states.push_back(state);
                    if (moveIss.peek() == ',')
                    {
                        moveIss.ignore();
                    }
                }
            }

            sort(states.begin(), states.end());
            stateMoves[alphabet[j]] = states;
        }

        stateTable[i] = stateMoves;
        std::getline(input, line);
    }
}

void VisualizeDFA(const DFA& dfa, const vector<char>& alphabet)
{
    ofstream dotFile("output.dot");

    if (!dotFile.is_open()) {
        cerr << "Error: Unable to open file for writing: output.dot" << endl;
        return;
    }

    dotFile << "digraph DFA {\n";
    dotFile << "    rankdir=LR;\n";
    dotFile << "    node [shape = circle];\n";

    for (const auto& [stateID, state] : dfa)
    {
        for (const char& symbol : alphabet)
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
    string command = "dot -Tpng output.dot -o output.png";
    system(command.c_str());

    cout << "Result in: output.png" << endl;
}

void PrintDFA(const DFA& dfa, const vector<char>& alphabet)
{
    cout << "   ";
    for (auto k = alphabet.begin(); k != alphabet.end() - 1; k++)
    {
        cout << *k << "    ";
    }
    cout << endl;

    for (const auto& [stateID, state] : dfa)
    {
        cout << stateID << " ";
        for (auto k = alphabet.begin(); k != alphabet.end() - 1; k++)
        {
            cout << OPEN_SYMBOL;
            if (state.moves.at(*k) != -1)
            {
                cout << state.moves.at(*k);
            }
            cout << CLOSE_SYMBOL << "   ";
        }
        cout << endl;
    }
}

string ParseArgs(int argc, char* argv[])
{
    if (argc < 2)
    {
        throw invalid_argument("No file given.\nUsage: <program.exe> <file_name.txt>");
    }
    return argv[1];
}

int main(int argc, char* argv[])
{
    try
    {
        string fileName = ParseArgs(argc, argv);

        int initState;
        vector<int> finalStates;
        int totalStates;
        vector<char> alphabet;
        NFA stateTable;
        DFA dfa;

        ReadFile(fileName, initState, finalStates, totalStates, alphabet, stateTable);
        SubsetConstruction(initState, finalStates, stateTable, dfa, alphabet);

        cout << "Initial state: [0]" << endl;
        cout << "Final states: ";
        PrintVector(GetDFAFinalStates(dfa, finalStates));
        cout << endl;
        PrintDFA(dfa, alphabet);
        VisualizeDFA(dfa, alphabet);
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
