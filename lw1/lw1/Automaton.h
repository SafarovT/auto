#pragma once
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <set>
#include <string>
#include <fstream>

class Automaton
{
public:
    virtual ~Automaton() {}
    virtual void Print() const = 0;
    virtual void ToMealy(std::unique_ptr<Automaton>& ptr) = 0;
    virtual void ToMoore(std::unique_ptr<Automaton>& ptr) = 0;
    virtual void Minimize() = 0;
    virtual std::string GetType() const = 0;
};

class MooreAutomaton : public Automaton
{
};

class MealyAutomaton : public Automaton
{
public:
    void Print() const override
    {
        std::cout << "Автомат Мили" << std::endl;
        std::cout << "Состояния: ";
        for (const auto& state : states) std::cout << state << " ";
        std::cout << std::endl << "Входные символы: ";
        for (const auto& sym : inputSymbols) std::cout << sym << " ";
        std::cout << std::endl << "Выходные символы: ";
        for (const auto& sym : outputSymbols) std::cout << sym << " ";
        std::cout << std::endl << "Начальное состояние: " << startState << "" << std::endl;

        std::cout << "Переходы:" << std::endl;
        for (const auto& [key, value] : transitions)
        {
            std::cout << key.first << " --" << key.second << "--> " << value.first
                << " / " << value.second << "" << std::endl;
        }
    }

    void ToMealy(std::unique_ptr<Automaton>& ptr) override
    {
        std::cout << "Автомат уже является автоматом Мили." << std::endl;
    }

    void ToMoore(std::unique_ptr<Automaton>& ptr) override
    {
        auto moore = std::make_unique<MooreAutomaton>();

        std::unordered_map<std::string, std::string> stateOutputMap;
        int newStateCounter = 0;

        moore->startState = this->startState + "_m0";
        moore->states.insert(moore->startState);
        moore->stateOutputs[moore->startState] = "O_" + this->startState;

        for (const auto& [key, value] : transitions)
        {
            const std::string& currentState = key.first;
            const std::string& output = value.second;

            std::string newState;
            std::string mapKey = currentState + "_" + output;

            if (stateOutputMap.find(mapKey) == stateOutputMap.end())
            {
                newState = currentState + "_m" + std::to_string(newStateCounter++);
                stateOutputMap[mapKey] = newState;
                moore->states.insert(newState);
                moore->stateOutputs[newState] = output;
            }
            else
            {
                newState = stateOutputMap[mapKey];
            }

            moore->transitions[{currentState, key.second}] = newState;
        }

        moore->inputSymbols = this->inputSymbols;

        moore->startState = stateOutputMap[this->startState + "_" + ""];

        ptr = std::move(moore);
    }

    void Minimize() override;

    std::string GetType() const override
    {
        return "Mealy";
    }

    bool LoadFromFile(std::ifstream& infile)
    {
        std::string line;

        // Чтение строк до "States:"
        while (std::getline(infile, line))
        {
            if (line.find("States:") != std::string::npos) break;
        }
        if (line.find("States:") == std::string::npos)
        {
            return false;
        }

        // Чтение состояний
        std::istringstream ssStates(line.substr(line.find(':') + 1));
        std::string state;
        while (ssStates >> state)
        {
            states.insert(state);
        }

        // Чтение "InputSymbols:"
        if (!std::getline(infile, line) || line.find("InputSymbols:") == std::string::npos) return false;
        std::istringstream ssInputs(line.substr(line.find(':') + 1));
        std::string input;
        while (ssInputs >> input)
        {
            inputSymbols.insert(input);
        }

        // Чтение "OutputSymbols:"
        if (!std::getline(infile, line) || line.find("OutputSymbols:") == std::string::npos) return false;
        std::istringstream ssOutputs(line.substr(line.find(':') + 1));
        std::string output;
        while (ssOutputs >> output)
        {
            outputSymbols.insert(output);
        }

        // Чтение "StartState:"
        if (!std::getline(infile, line) || line.find("StartState:") == std::string::npos) return false;
        startState = line.substr(line.find(':') + 1);
        startState.erase(0, startState.find_first_not_of(" \t"));

        // Чтение "Transitions:"
        if (!std::getline(infile, line) || line.find("Transitions:") == std::string::npos) return false;
        // Чтение переходов до конца файла или до пустой строки
        while (std::getline(infile, line))
        {
            if (line.empty()) break;
            std::istringstream ssTransition(line);
            std::string currentState, inputSymbol, nextState, outputSymbol;
            if (!(ssTransition >> currentState >> inputSymbol >> nextState >> outputSymbol))
            {
                std::cout << "Неверный формат перехода: " << line << std::endl;
                return false;
            }
            transitions[{currentState, inputSymbol}] = { nextState, outputSymbol };
        }

        return true;
    }

    using StringSet = std::set<std::string>;
    using StringPair = std::pair<std::string, std::string>;
    StringSet states;
    StringSet inputSymbols;
    StringSet outputSymbols;
    std::string startState;

    // Ключ: пара (текущее состояние, входной символ)
    // Значение: пара (следующее состояние, выходной символ)
    std::map<StringPair, StringPair> transitions;
};

class MooreAutomaton : public Automaton
{
public:
    void Print() const override
    {
        std::cout << "Автомат Мура" << std::endl;
        std::cout << "Состояния: ";
        for (const auto& state : states) std::cout << state << " ";
        std::cout << std::endl << "Входные символы: ";
        for (const auto& sym : inputSymbols) std::cout << sym << " ";
        std::cout << std::endl << "Выходы состояний:" << std::endl;
        for (const auto& [state, output] : stateOutputs)
        {
            std::cout << state << " : " << output << std::endl;
        }
        std::cout << "Начальное состояние: " << startState << std::endl;
        std::cout << "Переходы:" << std::endl;
        for (const auto& [key, nextState] : transitions)
        {
            std::cout << key.first << " --" << key.second << "--> " << nextState << std::endl;
        }
    }

    void ToMealy(std::unique_ptr<Automaton>& ptr) override
    {
        auto mealy = std::make_unique<MealyAutomaton>();

        mealy->states = this->states;
        mealy->inputSymbols = this->inputSymbols;
        mealy->startState = this->startState;

        for (const auto& [state, output] : this->stateOutputs) {
            mealy->outputSymbols.insert(output);
        }

        for (const auto& [key, nextState] : this->transitions) {
            const std::string& currentState = key.first;
            const std::string& inputSymbol = key.second;
            const std::string& outputSymbol = this->stateOutputs[nextState];

            mealy->transitions[{currentState, inputSymbol}] = { nextState, outputSymbol };
        }

        ptr = std::move(mealy);
    }

    void ToMoore(std::unique_ptr<Automaton>& ptr) override
    {
        std::cout << "Автомат уже является автоматом Мура." << std::endl;
    }

    void Minimize() override;

    std::string GetType() const override
    {
        return "Moore";
    }

    bool LoadFromFile(std::ifstream& infile)
    {
        std::string line;

        // Чтение строк до "States:"
        while (std::getline(infile, line))
        {
            if (line.find("States:") != std::string::npos) break;
        }
        if (line.find("States:") == std::string::npos) return false;

        // Чтение состояний
        std::istringstream ssStates(line.substr(line.find(':') + 1));
        std::string state;
        while (ssStates >> state)
        {
            states.insert(state);
        }

        // Чтение "InputSymbols:"
        if (!std::getline(infile, line) || line.find("InputSymbols:") == std::string::npos) return false;
        std::istringstream ssInputs(line.substr(line.find(':') + 1));
        std::string input;
        while (ssInputs >> input)
        {
            inputSymbols.insert(input);
        }

        // Чтение "StateOutputs:"
        if (!std::getline(infile, line) || line.find("StateOutputs:") == std::string::npos) return false;
        // Чтение выходов состояний до "StartState:"
        while (std::getline(infile, line))
        {
            if (line.find("StartState:") != std::string::npos) break;
            if (line.empty()) continue;
            std::istringstream ssOutput(line);
            std::string stateName, output;
            if (!(ssOutput >> stateName >> output))
            {
                std::cout << "Неверный формат выхода состояния: " << line << std::endl;
                return false;
            }
            stateOutputs[stateName] = output;
        }
        if (line.find("StartState:") == std::string::npos) return false;

        // Чтение "StartState:"
        startState = line.substr(line.find(':') + 1);
        // Удаление возможных пробелов
        startState.erase(0, startState.find_first_not_of(" \t"));

        // Чтение "Transitions:"
        if (!std::getline(infile, line) || line.find("Transitions:") == std::string::npos) return false;
        // Чтение переходов до конца файла или до пустой строки
        while (std::getline(infile, line))
        {
            if (line.empty()) break;
            std::istringstream ssTransition(line);
            std::string currentState, inputSymbol, nextState;
            if (!(ssTransition >> currentState >> inputSymbol >> nextState)) {
                std::cout << "Неверный формат перехода: " << line << std::endl;
                return false;
            }
            transitions[{currentState, inputSymbol}] = nextState;
        }

        return true;
    }

    using StringSet = std::set<std::string>;
    StringSet states;
    StringSet inputSymbols;
    std::map<std::string, std::string> stateOutputs;
    std::string startState;
    // Ключ: пара (текущее состояние, входной символ)
    // Значение: следующее состояние
    std::map<std::pair<std::string, std::string>, std::string> transitions;
};