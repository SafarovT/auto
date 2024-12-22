#include "Automaton.h"

void MealyAutomaton::Minimize()
{
    if (states.empty())
    {
        std::cout << "Автомат пуст." << std::endl;
        return;
    }

    std::map<std::vector<std::string>, std::set<std::string>> partitionsMap;

    for (const auto& state : states)
    {
        std::vector<std::string> outputs;
        for (const auto& input : inputSymbols)
        {
            auto it = transitions.find({ state, input });
            if (it != transitions.end())
            {
                outputs.push_back(it->second.second);
            }
            else
            {
                outputs.push_back("");
            }
        }
        partitionsMap[outputs].insert(state);
    }

    std::vector<std::set<std::string>> partitions;
    for (const auto& [key, group] : partitionsMap)
    {
        partitions.push_back(group);
    }

    bool updated = true;
    while (updated)
    {
        updated = false;
        std::vector<std::set<std::string>> newPartitions;

        for (const auto& group : partitions)
        {
            std::map<std::vector<int>, std::set<std::string>> splitterMap;

            for (const auto& state : group)
            {
                std::vector<int> signature;
                for (const auto& input : inputSymbols)
                {
                    auto it = transitions.find({ state, input });
                    if (it != transitions.end())
                    {
                        int classIndex = -1;
                        for (size_t i = 0; i < partitions.size(); ++i)
                        {
                            if (partitions[i].count(it->second.first))
                            {
                                classIndex = i;
                                break;
                            }
                        }
                        signature.push_back(classIndex);
                    }
                    else
                    {
                        signature.push_back(-1);
                    }
                }
                splitterMap[signature].insert(state);
            }

            if (splitterMap.size() > 1)
            {
                updated = true;
                for (const auto& [sig, subset] : splitterMap)
                {
                    newPartitions.push_back(subset);
                }
            }
            else
            {
                newPartitions.push_back(group);
            }
        }

        if (updated)
        {
            partitions = newPartitions;
        }
    }

    std::map<std::string, std::string> stateMapping;
    std::vector<std::set<std::string>> finalPartitions = partitions;
    std::vector<std::string> newStates;

    for (size_t i = 0; i < finalPartitions.size(); ++i)
    {
        std::string newState = "P" + std::to_string(i);
        newStates.push_back(newState);
        for (const auto& oldState : finalPartitions[i])
        {
            stateMapping[oldState] = newState;
        }
    }

    MealyAutomaton minimized;
    minimized.states = std::set<std::string>(newStates.begin(), newStates.end());
    minimized.inputSymbols = inputSymbols;
    minimized.outputSymbols = outputSymbols;
    minimized.startState = stateMapping[startState];

    for (const auto& [key, value] : transitions)
    {
        const std::string& oldState = key.first;
        const std::string& input = key.second;
        const std::string& nextOldState = value.first;
        const std::string& output = value.second;

        std::string newState = stateMapping[oldState];
        std::string newNextState = stateMapping[nextOldState];
        minimized.transitions[{newState, input}] = { newNextState, output };
    }

    minimized.outputSymbols.clear();
    for (const auto& [k, v] : minimized.transitions)
    {
        minimized.outputSymbols.insert(v.second);
    }

    *this = minimized;
    std::cout << "Автомат Мили успешно минимизирован." << std::endl;
}

void MooreAutomaton::Minimize()
{
    if (states.empty())
    {
        std::cout << "Автомат пуст." << std::endl;
        return;
    }

    std::map<std::string, std::set<std::string>> partitionsMap;

    for (const auto& state : states)
    {
        partitionsMap[stateOutputs[state]].insert(state);
    }

    std::vector<std::set<std::string>> partitions;
    for (const auto& [output, group] : partitionsMap)
    {
        partitions.push_back(group);
    }

    bool updated = true;
    while (updated)
    {
        updated = false;
        std::vector<std::set<std::string>> newPartitions;

        for (const auto& group : partitions)
        {
            std::map<std::vector<int>, std::set<std::string>> splitterMap;

            for (const auto& state : group)
            {
                std::vector<int> signature;
                for (const auto& input : inputSymbols)
                {
                    auto it = transitions.find({ state, input });
                    if (it != transitions.end())
                    {
                        int classIndex = -1;
                        for (size_t i = 0; i < partitions.size(); ++i)
                        {
                            if (partitions[i].count(it->second))
                            {
                                classIndex = i;
                                break;
                            }
                        }
                        signature.push_back(classIndex);
                    }
                    else
                    {
                        signature.push_back(-1);
                    }
                }
                splitterMap[signature].insert(state);
            }

            if (splitterMap.size() > 1)
            {
                updated = true;
                for (const auto& [sig, subset] : splitterMap)
                {
                    newPartitions.push_back(subset);
                }
            }
            else
            {
                newPartitions.push_back(group);
            }
        }

        if (updated)
        {
            partitions = newPartitions;
        }
    }

    std::map<std::string, std::string> stateMapping;
    std::vector<std::set<std::string>> finalPartitions = partitions;
    std::vector<std::string> newStates;

    for (size_t i = 0; i < finalPartitions.size(); ++i)
    {
        std::string newState = "P" + std::to_string(i);
        newStates.push_back(newState);
        for (const auto& oldState : finalPartitions[i])
        {
            stateMapping[oldState] = newState;
        }
    }

    MooreAutomaton minimized;
    minimized.states = std::set<std::string>(newStates.begin(), newStates.end());
    minimized.inputSymbols = inputSymbols;
    minimized.startState = stateMapping[startState];

    for (size_t i = 0; i < finalPartitions.size(); ++i)
    {
        const std::string& representative = *finalPartitions[i].begin();
        minimized.stateOutputs[newStates[i]] = stateOutputs[representative];
    }

    for (const auto& [key, nextState] : transitions)
    {
        const std::string& oldState = key.first;
        const std::string& input = key.second;
        const std::string& oldNextState = nextState;

        std::string newState = stateMapping[oldState];
        std::string newNextState = stateMapping[oldNextState];
        minimized.transitions[{newState, input}] = newNextState;
    }

    *this = minimized;
    std::cout << "Автомат Мура успешно минимизирован." << std::endl;
}