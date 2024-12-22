#include "Machine.h"
#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include <queue>

namespace
{
	using namespace std;

    const char SEPARATOR = ';';

	namespace MooreUtils
	{
        void SplitByOutput(vector<set<int>>& partitions, const vector<Moore::State>& states)
        {
            map<char, set<int>> outputGroups;

            for (const auto& state : states)
            {
                outputGroups[state.output].insert(state.id);
            }

            for (const auto& group : outputGroups)
            {
                partitions.push_back(group.second);
            }
        }

        bool SplitByTransitions(vector<set<int>>& partitions, const vector<Moore::State>& states)
        {
            vector<set<int>> newPartitions;
            bool partitionChanged = false;

            for (const auto& partition : partitions)
            {
                map<map<char, int>, set<int>> transitionGroups;

                for (int state : partition)
                {
                    map<char, int> transitionKey;
                    for (const auto& transition : states[state].transitions)
                    {
                        for (size_t i = 0; i < partitions.size(); ++i)
                        {
                            if (partitions[i].count(transition.second))
                            {
                                transitionKey[transition.first] = i;
                                break;
                            }
                        }
                    }
                    transitionGroups[transitionKey].insert(state);
                }

                if (transitionGroups.size() > 1)
                {
                    partitionChanged = true;
                }

                for (const auto& group : transitionGroups)
                {
                    newPartitions.push_back(group.second);
                }
            }

            partitions = newPartitions;
            return partitionChanged;
        }

        Moore::Machine RemoveUnreachableStates(const Moore::Machine& mooreAutomaton)
        {
            vector<MooreState> reachableStates;
            set<int> visited;
            queue<int> toVisit;

            toVisit.push(0);
            visited.insert(0);

            while (!toVisit.empty()) {
                int currentStateId = toVisit.front();
                toVisit.pop();

                for (const MooreState& state : mooreAutomaton)
                {
                    if (state.id == currentStateId)
                    {
                        reachableStates.push_back(state);

                        for (const auto& [input, nextState] : state.transitions)
                        {
                            if (visited.find(nextState) == visited.end())
                            {
                                toVisit.push(nextState);
                                visited.insert(nextState);
                            }
                        }
                        break;
                    }
                }
            }

            return reachableStates;
        }

        vector<set<int>> MinimizeMooreAutomaton(const vector<Moore::State>& states)
        {
            vector<set<int>> partitions;

            SplitByOutput(partitions, states);

            while (SplitByTransitions(partitions, states))
            {
            }

            return partitions;
        }

        vector<Moore::State> GetMinimizedStates(const vector<Moore::State>& states, const vector<set<int>>& minimizedPartitions)
        {
            vector<Moore::State> minimizedStates;
            map<int, int> stateToMinState;
            int newId = 0;

            for (const auto& partition : minimizedPartitions)
            {
                int representative = *partition.begin();
                Moore::State mState;
                mState.id = newId;
                mState.output = states[representative].output;

                for (const auto& transition : states[representative].transitions)
                {
                    int targetState = transition.second;
                    int targetPartition = -1;
                    for (size_t i = 0; i < minimizedPartitions.size(); ++i)
                    {
                        if (minimizedPartitions[i].count(targetState))
                        {
                            targetPartition = i;
                            break;
                        }
                    }
                    mState.transitions[transition.first] = targetPartition;
                }

                for (int state : partition)
                {
                    stateToMinState[state] = newId;
                }

                minimizedStates.push_back(mState);
                newId++;
            }

            return minimizedStates;
        }

        void Print(const vector<Moore::State>& minimizedStates)
        {
            cout << "Minimized Moore machine:" << endl;

            cout << "States:" << endl;
            for (const auto& mState : minimizedStates)
            {
                cout << mState.id << " ";
            }
            cout << endl;

            cout << "Outputs:" << endl;
            for (const auto& mState : minimizedStates)
            {
                cout << "State " << mState.id << ": " << mState.output << endl;
            }

            cout << "Transitions:" << endl;
            for (const auto& mState : minimizedStates)
            {
                cout << "State " << mState.id << ":" << endl;
                for (const auto& transition : mState.transitions)
                {
                    cout << "  " << transition.first << " -> " << transition.second << endl;
                }
            }
        }

        void GenerateDotFile(const vector<Moore::State>& automaton, const string& filename)
        {
            ofstream file;
            OpenFile(file, filename);

            file << "digraph MooreAutomaton {\n";
            file << "    rankdir=LR;\n";
            file << "    node [shape=circle];\n";

            for (const Moore::State& state : automaton)
            {
                file << "    " << state.id << " [label=\"" << state.id << " / " << state.output << "\"];\n";
            }

            for (const Moore::State& state : automaton)
            {
                for (const auto& [input, nextState] : state.transitions)
                {
                    file << "    " << state.id << " -> " << nextState << " [label=\"" << input << "\"];\n";
                }
            }

            file << "}\n";
            CheckFileRuntime(file);
            file.close();
        }

        void Visualize(const vector<Moore::State>& automaton, const string& outputImage)
        {
            string dotFilename = "moore_automaton.dot";

            GenerateDotFile(automaton, dotFilename);

            string command = "dot -Tpng " + dotFilename + " -o " + outputImage;
            system(command.c_str());

            cout << "Result in: " << outputImage << endl;
        }
	}

	namespace MealyUtils
	{
        void SplitByTransitions(vector<set<int>>& partitions, const vector<Mealy::State>& states)
        {
            map<map<char, char>, set<int>> transitionGroups;

            for (const auto& state : states)
            {
                map<char, char> outputs;
                for (const auto& transition : state.transitions)
                {
                    outputs[transition.first] = transition.second.second;
                }
                transitionGroups[outputs].insert(state.id);
            }

            for (const auto& group : transitionGroups)
            {
                partitions.push_back(group.second);
            }
        }

        bool SplitByNextStateAndOutput(vector<set<int>>& partitions, const vector<Mealy::State>& states)
        {
            vector<set<int>> newPartitions;
            bool partitionChanged = false;

            for (const auto& partition : partitions)
            {
                map<map<char, int>, set<int>> transitionGroups;

                for (int state : partition)
                {
                    map<char, int> transitionKey;
                    for (const auto& transition : states[state].transitions)
                    {
                        int nextState = transition.second.first;
                        char output = transition.second.second;

                        for (size_t i = 0; i < partitions.size(); ++i)
                        {
                            if (partitions[i].count(nextState))
                            {
                                transitionKey[transition.first] = i;
                                break;
                            }
                        }
                    }
                    transitionGroups[transitionKey].insert(state);
                }

                if (transitionGroups.size() > 1)
                {
                    partitionChanged = true;
                }

                for (const auto& group : transitionGroups)
                {
                    newPartitions.push_back(group.second);
                }
            }

            partitions = newPartitions;
            return partitionChanged;
        }

        vector<set<int>> MinimizeMealyAutomaton(const vector<Mealy::State>& states)
        {
            vector<set<int>> partitions;

            SplitByTransitions(partitions, states);

            while (SplitByNextStateAndOutput(partitions, states))
            {
            }

            return partitions;
        }

        Mealy::Machine RemoveUnreachableStates(const Mealy::Machine& mealyAutomaton) {
            vector<MealyState> reachableStates;
            set<int> visited;
            queue<int> toVisit;

            toVisit.push(0);
            visited.insert(0);

            while (!toVisit.empty())
            {
                int currentStateId = toVisit.front();
                toVisit.pop();

                for (const MealyState& state : mealyAutomaton) {
                    if (state.id == currentStateId) {
                        reachableStates.push_back(state);

                        // ќбрабатываем все переходы из текущего состо€ни€
                        for (const auto& [input, transition] : state.transitions) {
                            int nextState = transition.first;
                            if (visited.find(nextState) == visited.end()) {
                                toVisit.push(nextState);
                                visited.insert(nextState);
                            }
                        }
                        break;
                    }
                }
            }

            return reachableStates;
        }

        vector<Mealy::State> GetMinimizedStates(const vector<Mealy::State>& states, const vector<set<int>>& minimizedPartitions)
        {
            vector<Mealy::State> minimizedStates;
            map<int, int> stateToMinState;
            int newId = 0;

            for (const auto& partition : minimizedPartitions)
            {
                int representative = *partition.begin();
                Mealy::State mState;
                mState.id = newId;

                for (const auto& transition : states[representative].transitions)
                {
                    int targetState = transition.second.first;
                    char output = transition.second.second;

                    int targetPartition = -1;
                    for (size_t i = 0; i < minimizedPartitions.size(); ++i)
                    {
                        if (minimizedPartitions[i].count(targetState))
                        {
                            targetPartition = i;
                            break;
                        }
                    }
                    mState.transitions[transition.first] = { targetPartition, output };
                }

                for (int state : partition)
                {
                    stateToMinState[state] = newId;
                }

                minimizedStates.push_back(mState);
                newId++;
            }

            return minimizedStates;
        }

        void Print(const vector<Mealy::State>& minimizedStates)
        {
            cout << "Minimized Mealy machine:" << endl;

            cout << "States:" << endl;
            for (const auto& mState : minimizedStates)
            {
                cout << mState.id << " ";
            }
            cout << endl;

            cout << "Transitions:" << endl;
            for (const auto& mState : minimizedStates)
            {
                cout << "State " << mState.id << ":" << endl;
                for (const auto& transition : mState.transitions)
                {
                    cout << "  " << transition.first << " -> State " << transition.second.first << " (output: " << transition.second.second << ")" << endl;
                }
            }
        }

        void GenerateDotFile(const vector<Mealy::State>& automaton, const string& filename)
        {
            ofstream file;
            OpenFile(file, filename);

            file << "digraph MealyAutomaton {\n";
            file << "    rankdir=LR;\n";
            file << "    node [shape=circle];\n";

            for (const Mealy::State& state : automaton)
            {
                for (const auto& [input, transition] : state.transitions)
                {
                    int nextState = transition.first;
                    char output = transition.second;

                    file << "    " << state.id << " -> " << nextState << " [label=\"" << input << "/" << output << "\"];\n";
                }
            }

            file << "}\n";
            CheckFileRuntime(file);
            file.close();
        }

        void Visualize(const vector<Mealy::State>& automaton, const string& outputImage)
        {
            string dotFilename = "mealy_automaton.dot";

            GenerateDotFile(automaton, dotFilename);

            string command = "dot -Tpng " + dotFilename + " -o " + outputImage;
            system(command.c_str());

            cout << "Result in file: " << outputImage << endl;
        }
	}
}

Mealy::Machine Mealy::ReadFromFile(std::string const& inputFilePath)
{
    ifstream file;
    OpenFile(file, inputFilePath);

    vector<Mealy::State> states;
    string line, header;

    getline(file, header);

    map<int, Mealy::State> stateMap;

    while (getline(file, line))
    {
        stringstream ss(line);
        string idStr, inputStr, nextStateStr, outputStr;

        getline(ss, idStr, SEPARATOR);
        getline(ss, inputStr, SEPARATOR);
        getline(ss, nextStateStr, SEPARATOR);
        getline(ss, outputStr, SEPARATOR);

        int id = stoi(idStr);
        char input = inputStr[0];
        int nextState = stoi(nextStateStr);
        char output = outputStr[0];

        if (stateMap.find(id) == stateMap.end())
        {
            Mealy::State newState;
            newState.id = id;
            stateMap[id] = newState;
        }

        stateMap[id].transitions[input] = { nextState, output };
    }

    for (const auto& [id, state] : stateMap)
    {
        states.push_back(state);
    }

    CheckFileRuntime(file);
    file.close();
    return states;
}

Moore::Machine Moore::ReadFromFile(std::string const& inputFilePath)
{
    ifstream file;
    OpenFile(file, inputFilePath);

    vector<Moore::State> states;
    string line, header;

    getline(file, header);

    map<int, Moore::State> stateMap;

    while (getline(file, line))
    {
        stringstream ss(line);
        string idStr, outputStr, inputStr, nextStateStr;

        getline(ss, idStr, SEPARATOR);
        getline(ss, outputStr, SEPARATOR);
        getline(ss, inputStr, SEPARATOR);
        getline(ss, nextStateStr, SEPARATOR);

        int id = stoi(idStr);
        char output = outputStr[0];
        char input = inputStr[0];
        int nextState = stoi(nextStateStr);

        if (stateMap.find(id) == stateMap.end())
        {
            Moore::State newState;
            newState.id = id;
            newState.output = output;
            stateMap[id] = newState;
        }

        stateMap[id].transitions[input] = nextState;
    }

    for (const auto& [id, state] : stateMap)
    {
        states.push_back(state);
    }

    CheckFileRuntime(file);
    file.close();
    return states;
}

Mealy::Machine Moore::ToMealy(Moore::Machine& machine)
{
    vector<Mealy::State> mealyAutomaton;

    for (const Moore::State& mooreState : machine)
    {
        Mealy::State mealyState;
        mealyState.id = mooreState.id;

        for (const auto& [input, nextState] : mooreState.transitions)
        {
            auto nextStateOutput = machine[nextState].output;
            mealyState.transitions[input] = { nextState, nextStateOutput };
        }

        mealyAutomaton.push_back(mealyState);
    }

    MealyUtils::Print(mealyAutomaton);
    MealyUtils::Visualize(mealyAutomaton, "transformed_mealy.png");

    return mealyAutomaton;
}

Moore::Machine Moore::Minimize(Machine& machine)
{
    vector<set<int>> minimizedPartitions = MooreUtils::MinimizeMooreAutomaton(machine);

    sort(minimizedPartitions.begin(), minimizedPartitions.end(),
        [](const set<int>& a, const set<int>& b) -> bool
        {
            return *a.begin() < *b.begin();
        });

    Moore::Machine minimizedStates = MooreUtils::GetMinimizedStates(machine, minimizedPartitions);
    minimizedStates = MooreUtils::RemoveUnreachableStates(minimizedStates);

    MooreUtils::Print(minimizedStates);
    MooreUtils::Visualize(minimizedStates, "minimized_moore.png");

    return minimizedStates;
}

Moore::Machine Mealy::ToMoore(Machine& machine)
{
    vector<Moore::State> mooreAutomaton;
    map<pair<int, char>, int> stateMapping;
    vector<char> outputs;
    int newStateId = 0;

    for (const Mealy::State& mealyState : machine)
    {
        for (const auto& [input, transition] : mealyState.transitions)
        {
            int nextState = transition.first;
            char output = transition.second;

            if (stateMapping.find({ mealyState.id, output }) == stateMapping.end())
            {
                Moore::State newState;
                newState.id = newStateId++;
                newState.output = output;
                mooreAutomaton.push_back(newState);
                stateMapping[{mealyState.id, output}] = newState.id;
            }

            if (find(outputs.begin(), outputs.end(), output) == outputs.end())
            {
                outputs.push_back(output);
            }
        }
    }

    for (const Mealy::State& mealyState : machine)
    {
        for (const auto& [input, transition] : mealyState.transitions)
        {
            int nextState = transition.first;
            char output = transition.second;

            for (const auto& output : outputs)
            {
                if (stateMapping.find({ mealyState.id, output }) != stateMapping.end())
                {
                    int mooreCurrentStateId = stateMapping[{mealyState.id, output}];

                    char nextOutput = machine[nextState].transitions.begin()->second.second;
                    int mooreNextStateId = stateMapping[{nextState, nextOutput}];

                    mooreAutomaton[mooreCurrentStateId].transitions[input] = mooreNextStateId;
                }
            }
        }
    }

    MooreUtils::Print(mooreAutomaton);
    MooreUtils::Visualize(mooreAutomaton, "transformed_moore.png");

    return mooreAutomaton;
}

Mealy::Machine Mealy::Minimize(Machine& machine)
{
    vector<set<int>> minimizedPartitions = MealyUtils::MinimizeMealyAutomaton(machine);

    sort(minimizedPartitions.begin(), minimizedPartitions.end(),
        [](const set<int>& a, const set<int>& b) -> bool
        {
            return *a.begin() < *b.begin();
        });

    vector<Mealy::State> minimizedStates = MealyUtils::GetMinimizedStates(machine, minimizedPartitions);
    minimizedStates = MealyUtils::RemoveUnreachableStates(minimizedStates);

    MealyUtils::Print(minimizedStates);
    MealyUtils::Visualize(minimizedStates, "minimized_mealy.png");

    return minimizedStates;
}