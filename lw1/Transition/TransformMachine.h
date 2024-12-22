#pragma once
#include <string>

struct FilePathes
{
	std::string inputFile;
	std::string outputFile;
};

void TransformMachine(FilePathes const& pathes);