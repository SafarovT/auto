#pragma once
#include <string>
#include <vector>
#include <map>

struct MooreState
{
    int id;
    char output;
    std::map<char, int> transitions;
};

struct MealyState
{
    int id;
    std::map<char, std::pair<int, char>> transitions;
};

namespace Moore
{
    using State = MooreState;
    using Machine = std::vector<State>;

    Machine Minimize(Machine& machine);
    std::vector<MealyState> ToMealy(Machine& machine);
    Machine ReadFromFile(std::string const& inputFilePath);
}

namespace Mealy
{
    using State = MealyState;
    using Machine = std::vector<State>;

    Machine Minimize(Machine& machine);
    std::vector<MooreState> ToMoore(Machine& machine);
    Machine ReadFromFile(std::string const& inputFilePath);
}
