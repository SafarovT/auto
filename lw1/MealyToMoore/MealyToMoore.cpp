#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>

namespace
{
	using namespace std;

	using MealyMachine = vector<vector<tuple<int, int, int>*>>;
	using States = vector<tuple<int, int, int>*>;

	const string PATH_TO_INPUT = "inputMealy.txt";
	const string PATH_TO_OUTPUT = "outputMealy.txt";

	void ReadFromFile(MealyMachine& mealyMachine, States& states)
	{
		ifstream input(PATH_TO_INPUT);
		if (!input.is_open())
		{
			throw runtime_error("Cant open input file");
		}
		int statesCount, inputsCount;
		input >> statesCount >> inputsCount;

		for (int i = 0; i < statesCount; i++)
		{
			mealyMachine.push_back(vector<tuple<int, int, int>*>());
			for (int j = 0; j < inputsCount; j++)
			{
				string state, symbol;
				input >> state;

				if (state[0] != '-')
				{
					input >> symbol;

					auto temp = new tuple<int, int, int>{ stoi(state.substr(1)),
						stoi(symbol.substr(1)),
						stoi(state.substr(1) + symbol.substr(1)) };

					states.push_back(temp);
					mealyMachine[i].push_back(temp);
				}
				else
				{
					mealyMachine[i].push_back(new tuple<int, int, int>{ -1, -1, -1 });
				}
			}
		}
	}

	void WriteToFile(MealyMachine const& mealyMachine, States& states)
	{
		ofstream output(PATH_TO_OUTPUT);

		for (int i = 0; i < states.size() - 1; i++)
		{
			for (int j = 0; j < states.size() - i - 1; j++)
			{
				if (get<2>(*states[j]) > get<2>(*states[j + 1]))
				{
					auto temp = states[j];
					states[j] = states[j + 1];
					states[j + 1] = temp;
				}
			}
		}

		int l = 0;
		for (int i = 0; i < states.size() - 1; i++)
		{
			int temp = get<2>(*states[i]);
			get<2>(*states[i]) = l;

			if (!(temp == get<2>(*states[i + 1])))
			{
				l++;
			}
		}
		get<2>(*states.back()) = l++;


		for (int i = 0; i <= get<2>(*states.back()); i++)
		{
			auto temp = new tuple<int, int, int>{ -1,-1,-1 };

			for (auto state : states)
			{
				if (get<2>(*state) == i)
				{
					temp = state;
				}
			}

			output << "Y" << get<1>(*temp) << "\t";
			for (int j = 0; j < mealyMachine[0].size(); j++)
			{
				auto state = mealyMachine[get<0>(*temp)][j];
				if (*state == tuple<int, int, int>{ -1, -1, -1 })
				{
					output << "-";
				}
				else
				{
					output << "q" << get<2>(*state);
				}
				if (j != mealyMachine[0].size() - 1)
				{
					output << "\t";
				}
			}
			output << "\n";
		}
	}
}

int main()
{
	MealyMachine mealyMachine;
	States states;
	try
	{
		ReadFromFile(mealyMachine, states);
		WriteToFile(mealyMachine, states);
	}
	catch (exception const& e)
	{
		cout << e.what() << endl;
	}

	return 0;
}

