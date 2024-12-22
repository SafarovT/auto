#pragma once
#include <string>
#include <fstream>
#include <stdexcept>

template <class T>
void OpenFile(T& file, std::string const& fileName)
{
    file.open(fileName);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file " + fileName);
    }
}

void CheckFileRuntime(std::ifstream& inputFile)
{
    if (inputFile.bad())
    {
        throw std::runtime_error("Failed to work with given file");
    }
}

void CheckFileRuntime(std::ofstream& outputFile)
{
    if (!outputFile.flush())
    {
        throw std::runtime_error("Failed to write in output file");
    }
}