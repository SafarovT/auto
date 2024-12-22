#include <iostream>
#include <stdexcept>
#include "TransformMachine.h"

namespace
{
	using namespace std;

	FilePathes ReadFileNames()
	{
		FilePathes pathes;

		cout << "Enter an input file path: ";
		cin >> pathes.inputFile;

		cout << "Enter an output file path: ";
		cin >> pathes.outputFile;

		return pathes;
	}
}

int main()
{
	try
	{
		TransformMachine(ReadFileNames());
	}
	catch (exception const& e)
	{
		cout << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}