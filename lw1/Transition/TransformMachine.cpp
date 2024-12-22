#include <fstream>
#include <tuple>
#include <vector>
#include "FileUtils.h"
#include "TransformMachine.h"

namespace
{
	using namespace std;

	const string MOORE_MACHINE_TYPE = "Moore";
	const string MEALY_MACHINE_TYPE = "Mealy";

	namespace MooreToMealy
	{
		using MooreMachine = vector<pair<vector<int>, int>*>;

		MooreMachine ReadFromFile(ifstream& input)
		{
			MooreMachine mooreMachine;
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

			return mooreMachine;
		}

		void WriteToFile(ofstream& output, MooreMachine& mooreMachine)
		{
			auto statesCount = mooreMachine.size();
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

	namespace MealyToMoore
	{
		using Transitions = vector<vector<tuple<int, int, int>*>>;
		using States = vector<tuple<int, int, int>*>;
		
		struct MealyMachine
		{
			Transitions transitions;
			States states;
		};


		MealyMachine ReadFromFile(ifstream& input)
		{
			MealyMachine mealyMachine;
			int statesCount, inputsCount;
			input >> statesCount >> inputsCount;

			for (int i = 0; i < statesCount; i++)
			{
				mealyMachine.transitions.push_back(vector<tuple<int, int, int>*>());
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

						mealyMachine.states.push_back(temp);
						mealyMachine.transitions[i].push_back(temp);
					}
					else
					{
						mealyMachine.transitions[i].push_back(new tuple<int, int, int>{ -1, -1, -1 });
					}
				}
			}

			return mealyMachine;
		}

		void WriteToFile(ofstream& output, MealyMachine& mealyMachine)
		{
			for (int i = 0; i < mealyMachine.states.size() - 1; i++)
			{
				for (int j = 0; j < mealyMachine.states.size() - i - 1; j++)
				{
					if (get<2>(*mealyMachine.states[j]) > get<2>(*mealyMachine.states[j + 1]))
					{
						auto temp = mealyMachine.states[j];
						mealyMachine.states[j] = mealyMachine.states[j + 1];
						mealyMachine.states[j + 1] = temp;
					}
				}
			}

			int l = 0;
			for (int i = 0; i < mealyMachine.states.size() - 1; i++)
			{
				int temp = get<2>(*mealyMachine.states[i]);
				get<2>(*mealyMachine.states[i]) = l;

				if (!(temp == get<2>(*mealyMachine.states[i + 1])))
				{
					l++;
				}
			}
			get<2>(*mealyMachine.states.back()) = l++;


			for (int i = 0; i <= get<2>(*mealyMachine.states.back()); i++)
			{
				auto temp = new tuple<int, int, int>{ -1,-1,-1 };

				for (auto state : mealyMachine.states)
				{
					if (get<2>(*state) == i)
					{
						temp = state;
					}
				}

				output << "Y" << get<1>(*temp) << "\t";
				for (int j = 0; j < mealyMachine.transitions[0].size(); j++)
				{
					auto state = mealyMachine.transitions[get<0>(*temp)][j];
					if (*state == tuple<int, int, int>{ -1, -1, -1 })
					{
						output << "-";
					}
					else
					{
						output << "q" << get<2>(*state);
					}
					if (j != mealyMachine.transitions[0].size() - 1)
					{
						output << "\t";
					}
				}
				output << "\n";
			}
		}
	}
}

void TransformMachine(FilePathes const& pathes)
{
	ifstream input;
	ofstream output;
	OpenFile(input, pathes.inputFile);
	OpenFile(output, pathes.outputFile);

	string machineType;
	getline(input, machineType);

	if (machineType == MOORE_MACHINE_TYPE)
	{
		auto mooreMachine = MooreToMealy::ReadFromFile(input);
		MooreToMealy::WriteToFile(output, mooreMachine);
	}
	else if (machineType == MEALY_MACHINE_TYPE)
	{
		auto mealyMachine = MealyToMoore::ReadFromFile(input);
		MealyToMoore::WriteToFile(output, mealyMachine);
	}
	else
	{
		throw invalid_argument("Unknown machine type");
	}

	CheckFileRuntime(input);
	CheckFileRuntime(output);
}