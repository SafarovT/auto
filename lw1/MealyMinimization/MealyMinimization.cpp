#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

struct State
{
    int id;
    map<char, pair<int, char>> transitions;
};

void SplitByTransitions(vector<set<int>>& partitions, const vector<State>& states)
{
    map<map<char, pair<int, char>>, set<int>> transitionGroups;

    for (const auto& state : states)
    {
        transitionGroups[state.transitions].insert(state.id);
    }

    for (const auto& group : transitionGroups)
    {
        partitions.push_back(group.second);
    }
}

bool SplitByNextStateAndOutput(vector<set<int>>& partitions, const vector<State>& states)
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

vector<set<int>> MinimizeMealyAutomaton(const vector<State>& states)
{
    vector<set<int>> partitions;

    SplitByTransitions(partitions, states);

    while (SplitByNextStateAndOutput(partitions, states))
    {
    }

    return partitions;
}

struct MinimizedState
{
    int id;
    map<char, pair<int, char>> transitions;
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

void PrintMinimizedAutomaton(const vector<MinimizedState>& minimizedStates)
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

int main()
{
    vector<State> states =
    {
        {0, {{'0', {1, 'x'}}, {'1', {2, 'y'}}}},
        {1, {{'0', {1, 'x'}}, {'1', {3, 'y'}}}},
        {2, {{'0', {1, 'x'}}, {'1', {2, 'y'}}}},
        {3, {{'0', {3, 'z'}}, {'1', {3, 'z'}}}}
    };

    vector<set<int>> minimizedPartitions = MinimizeMealyAutomaton(states);

    sort(minimizedPartitions.begin(), minimizedPartitions.end(),
        [](const set<int>& a, const set<int>& b) -> bool
        {
            return *a.begin() < *b.begin();
        });

    vector<MinimizedState> minimizedStates = GetMinimizedStates(states, minimizedPartitions);

    PrintMinimizedAutomaton(minimizedStates);

    return 0;
}
