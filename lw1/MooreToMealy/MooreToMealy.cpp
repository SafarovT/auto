#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace
{
	using namespace std;

	const string PATH_TO_INPUT = "inputMoore.txt";
	const string PATH_TO_OUTPUT = "outputMoore.txt";

	using MooreMachine = vector<pair<vector<int>, int>*>;

	int ReadFromFile(MooreMachine& mooreMachine)
	{
		ifstream input(PATH_TO_INPUT);

		int statesCount, inputsCount;
		input >> statesCount >> inputsCount;

		for (int i = 0; i < statesCount; i++)
		{
			string signal;
			input >> signal;

			mooreMachine.push_back(new pair<vector<int>, int>());
			mooreMachine.back()->second = -1;
			if (signal[0] != '-')
			{
				mooreMachine.back()->second = stoi(signal.substr(1));
			}


			for (int j = 0; j < inputsCount; j++)
			{
				string state;
				input >> state;

				if (state[0] != '-')
				{
					mooreMachine[i]->first.push_back(stoi(state.substr(1)));
				}
				else
				{
					mooreMachine[i]->first.push_back(-1);
				}
			}
		}

		return statesCount;
	}

	void WriteToFile(MooreMachine const& mooreMachine, int statesCount)
	{
		ofstream output(PATH_TO_OUTPUT);

		for (int i = 0; i < statesCount; i++)
		{
			auto states = mooreMachine[i]->first;
			for (auto state : states)
			{
				if (state != -1)
				{
					output << "S" << state;
				}
				else
				{
					output << "- ";
					continue;
				}

				bool is_null = false;
				auto temp = mooreMachine[state];
				for (auto t : temp->first)
				{
					if (t == -1 && temp->second == -1)
					{
						is_null = true;
					}
				}

				if (!is_null)
				{
					output << "\t" << "Y" << mooreMachine[state]->second;
				}
				else
				{
					output << "\t-\t";
				}

				if (mooreMachine[i]->first.back() != state)
				{
					output << "\t";
				}
			}
			output << endl;;
		}
	}
}

int main()
{
	MooreMachine mooreMachine;
	auto statesCount = ReadFromFile(mooreMachine);
	WriteToFile(mooreMachine, statesCount);

	return 0;
}