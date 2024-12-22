#pragma once
#include <iostream>
#include <map>
#include <optional>
#include "Grammar.h"

class DFA
{
public:
	struct DFAState
	{
		bool marked;
		std::vector<char> states;
		std::map<char, char> moves;
	};

	using DFAData = std::map<char, DFAState>;
	using NFAData = std::map<char, std::map<char, std::vector<char>>>;

	DFA(const Grammar& grammar);

	void Minimize() const;
	void Print(std::ostream& output) const;
	void Display(const std::string& fileName) const;

private:
	DFAData m_data;
	std::vector<char> m_alphabet;

	std::optional<char> FindUnmarked() const;
	std::optional<char> FindState(const std::vector<char>& state) const;
	std::vector<char> GetDFAFinalStates(const std::vector<char>& finalStates) const;
	void SubsetConstruction(char initialState, const std::vector<char>& finalStates, const NFAData& nfa);
};