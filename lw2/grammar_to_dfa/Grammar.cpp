#include "Grammar.h"
#include <fstream>
#include <sstream>

using namespace std;

Grammar::Grammar(const string& fileName, Side side)
    : m_side(side)
{
    ifstream file(fileName);
    if (!file.is_open())
    {
        throw runtime_error("Cannot open file: " + fileName);
    }

    string line;
    while (getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty())
            continue;

        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos)
        {
            throw runtime_error("Invalid grammar format: " + line);
        }

        char nonTerminal = line[0];
        if (!isupper(nonTerminal))
        {
            throw runtime_error("Non-terminal must be an uppercase letter: " + line);
        }

        string productions = line.substr(arrowPos + 2);
        stringstream ss(productions);
        string production;
        vector<pair<char, char>> transitions;

        while (getline(ss, production, '|'))
        {
            production.erase(0, production.find_first_not_of(" \t"));
            production.erase(production.find_last_not_of(" \t") + 1);

            if (production.empty())
                continue;

            if (m_side == Side::Left)
            {
                // Левая грамматика: S -> A0
                if (production.size() == 1 && isalnum(production[0]))
                {
                    transitions.emplace_back(production[0], FINAL_SYMBOL);
                }
                else if (production.size() == 2 && isupper(production[0]) && isalnum(production[1]))
                {
                    transitions.emplace_back(production[1], production[0]);
                }
                else
                {
                    throw runtime_error("Invalid left grammar production: " + production);
                }
            }
            else
            {
                // Правая грамматика: S -> 0A
                if (production.size() == 1 && isalnum(production[0]))
                {
                    transitions.emplace_back(production[0], FINAL_SYMBOL);
                }
                else if (production.size() == 2 && isalnum(production[0]) && isupper(production[1]))
                {
                    transitions.emplace_back(production[0], production[1]);
                }
                else
                {
                    throw runtime_error("Invalid right grammar production: " + production);
                }
            }
        }

        m_transitions.emplace_back(nonTerminal, transitions);
    }
}

void Grammar::Print(const string& fileName) const
{
    ofstream output(fileName);
    if (!output.is_open())
    {
        throw runtime_error("Error opening output file");
    }
    for (const auto& row : m_transitions)
    {
        output << row.first << " -> ";
        for (const auto& item : row.second)
        {
            if (m_side == Grammar::Side::Right)
                output << item.first << item.second;
            else if (m_side == Grammar::Side::Left)
                output << item.second << item.first;
            output << " | ";
        }
        output << endl;
    }
}

vector<Grammar::Transitions> Grammar::GetTransitions() const
{
	return m_transitions;
}

Grammar::Side Grammar::GetSide() const
{
	return m_side;
}