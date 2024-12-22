#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

struct State
{
    int id;
    char output;
    map<char, int> transitions;
};

void SplitByOutput(vector<set<int>>& partitions, const vector<State>& states)
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

bool SplitByTransitions(vector<set<int>>& partitions, const vector<State>& states)
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

vector<set<int>> MinimizeMooreAutomaton(const vector<State>& states)
{
    vector<set<int>> partitions;

    SplitByOutput(partitions, states);

    while (SplitByTransitions(partitions, states))
    {
    }

    return partitions;
}

struct MinimizedState
{
    int id;
    char output;
    map<char, int> transitions;
};

vector<MinimizedState> GetMinimizedStates(const vector<State>& states, const vector<set<int>>& minimizedPartitions)
{
    vector<MinimizedState> minimizedStates;
    map<int, int> stateToMinState;
    int newId = 0;

    for (const auto& partition : minimizedPartitions)
    {
        int representative = *partition.begin();
        MinimizedState mState;
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

void PrintMinimizedAutomaton(const vector<MinimizedState>& minimizedStates)
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

int main()
{
    vector<State> states =
    {
        {0, 'a', {{'0', 1}, {'1', 2}}},
        {1, 'b', {{'0', 1}, {'1', 3}}},
        {2, 'a', {{'0', 1}, {'1', 2}}},
        {3, 'b', {{'0', 3}, {'1', 3}}}
    };

    vector<set<int>> minimizedPartitions = MinimizeMooreAutomaton(states);

    sort(minimizedPartitions.begin(), minimizedPartitions.end(),
        [](const set<int>& a, const set<int>& b) -> bool
        {
            return *a.begin() < *b.begin();
        });

    vector<MinimizedState> minimizedStates = GetMinimizedStates(states, minimizedPartitions);

    PrintMinimizedAutomaton(minimizedStates);

    return 0;
}
