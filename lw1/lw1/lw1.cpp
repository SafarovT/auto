#include <fstream>
#include <vector>
#include <sstream>
#include "Automaton.h"

using namespace std;

enum EndCode
{
    Success = 0,
    Error = 1,
};

void PrintStartupData()
{
    cout << "Доступные комманды: " << endl
        << "exit" << endl
        << "read <fileName>" << endl
        << "print" << endl
        << "toMealy" << endl
        << "toMoore" << endl
        << "minimize" << endl
        << "Введите команду: " << endl;
}

unique_ptr<Automaton> ReadAutomatonFromFile(const string& filename)
{
    ifstream infile(filename);
    if (!infile) {
        cout << "Не удалось открыть файл: " << filename << endl;
        return nullptr;
    }

    std::string type;
    infile >> type;
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (type == "Mealy")
    {
        auto automaton = std::make_unique<MealyAutomaton>();
        if (automaton->LoadFromFile(infile))
        {
            std::cout << "Чтение автомата Мили из файла успешно." << std::endl;
            return automaton;
        }
        else
        {
            std::cerr << "Ошибка при чтении автомата Мили из файла." << std::endl;
            return nullptr;
        }
    }
    else if (type == "Moore")
    {
        auto automaton = std::make_unique<MooreAutomaton>();
        if (automaton->LoadFromFile(infile))
        {
            std::cout << "Чтение автомата Мура из файла успешно." << std::endl;
            return automaton;
        }
        else
        {
            std::cerr << "Ошибка при чтении автомата Мура из файла." << std::endl;
            return nullptr;
        }
    }
    else
    {
        std::cerr << "Неизвестный тип автомата: " << type << std::endl;
        return nullptr;
    }
}

void HandleRead(const string& filename, unique_ptr<Automaton>& currentAutomaton)
{
    auto automaton = ReadAutomatonFromFile(filename);
    if (automaton) {
        currentAutomaton = move(automaton);
        cout << "Автомат успешно загружен." << endl;
    }
}

void HandleToMealy(unique_ptr<Automaton>& currentAutomaton)
{
    if (!currentAutomaton)
    {
        cout << "Автомат не загружен." << endl;
        return;
    }

    if (currentAutomaton->GetType() == "Mealy") {
        cout << "Автомат уже является автоматом Мили." << endl;
    }
    else {
        currentAutomaton->ToMealy(currentAutomaton);
    }
}

void HandleToMoore(unique_ptr<Automaton>& currentAutomaton)
{
    if (!currentAutomaton)
    {
        cout << "Автомат не загружен.\n";
        return;
    }

    if (currentAutomaton->GetType() == "Moore")
    {
        cout << "Автомат уже является автоматом Мура.\n";
    }
    else {
        currentAutomaton->ToMoore(currentAutomaton);
    }
}

void HandleMinimize(unique_ptr<Automaton>& currentAutomaton)
{
    if (!currentAutomaton)
    {
        cout << "Автомат не загружен.\n";
        return;
    }

    currentAutomaton->Minimize();
}

void HandlePrint(const unique_ptr<Automaton>& currentAutomaton)
{
    if (!currentAutomaton)
    {
        cout << "Автомат не загружен.\n";
        return;
    }

    currentAutomaton->Print();
}

bool HandleCommand(const string& command, unique_ptr<Automaton>& currentAutomaton) {
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "exit")
    {
        return false;
    }
    else if (cmd == "read")
    {
        string filename;
        iss >> filename;
        if (filename.empty())
        {
            cout << "Пожалуйста, передайти имя файла в качестве параметра комманды read." << endl;
        }
        else
        {
            HandleRead(filename, currentAutomaton);
        }
    }
    else if (cmd == "toMealy")
    {
        HandleToMealy(currentAutomaton);
    }
    else if (cmd == "toMoore")
    {
        HandleToMoore(currentAutomaton);
    }
    else if (cmd == "minimize")
    {
        HandleMinimize(currentAutomaton);
    }
    else if (cmd == "print")
    {
        HandlePrint(currentAutomaton);
    }
    else
    {
        cout << "Неизвестная команда: " << cmd << endl;
    }

    return true;
}

int main() {
    unique_ptr<Automaton> currentAutomaton = nullptr;
    string input;

   
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (!HandleCommand(input, currentAutomaton)) {
            break;
        }
    }

    cout << "Программа завершена." << endl;
    return EndCode::Success;
}
