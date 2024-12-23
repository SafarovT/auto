#pragma once
#include <vector>
#include <string>
#include <stdexcept>

class Grammar
{
public:
	enum class Side
	{
		Left,
		Right,
	};
	static const char FINAL_SYMBOL = 'H';

	using Transitions = std::pair<char, std::vector<std::pair<char, char>>>;

	Grammar(const std::string& fileName, Side side);

	void Print(const std::string& fileName) const;

	std::vector<Transitions> GetTransitions() const;
	Side GetSide() const;

private:
	Side m_side;
	std::vector<Transitions> m_transitions;
};