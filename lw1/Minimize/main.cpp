#include "Machine.h"
#include <string>
#include <iostream>

namespace
{
    using namespace std;

    bool IsSubstring(const string& str, const string& sub)
    {
        return str.find(sub) != string::npos;
    }

    string ReadInput()
    {
        string input;
        cin >> input;
        return input;
    }
}

int main()
{
    cout << "Enter input file path: ";
    auto inputFilePath = ReadInput();
    cout << "Choose mode 'min' or 'trans': ";
    auto mode = ReadInput();
    bool isMoore = IsSubstring(inputFilePath, "_moore_");
    bool isMealy = IsSubstring(inputFilePath, "_mealy_");

    Mealy::Machine mealyMachine;
    Moore::Machine mooreMachine;
    if (isMoore)
    {
        mooreMachine = Moore::ReadFromFile(inputFilePath);
    }
    else if (isMealy)
    {
        mealyMachine = Mealy::ReadFromFile(inputFilePath);
    }
    else
    {
        cout << "Unknown machine type" << endl;
        return EXIT_FAILURE;
    }

    do
    {
        if (mode == "min")
        {
            if (isMoore)
            {
                mooreMachine = Moore::Minimize(mooreMachine);
            }
            else if (isMealy)
            {
                mealyMachine = Mealy::Minimize(mealyMachine);
            }
        }
        else if (mode == "trans")
        {
            if (isMoore)
            {
                mealyMachine = Moore::ToMealy(mooreMachine);
                isMealy = true;
                isMoore = false;
            }
            else if (isMealy)
            {
                mooreMachine = Mealy::ToMoore(mealyMachine);
                isMealy = false;
                isMoore = true;
            }
        }
        cout << "Choose mode 'min' or 'trans': ";
        mode = ReadInput();
    } while (mode != "exit");    

    return EXIT_SUCCESS;
}
