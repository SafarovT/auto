#include "DFA.h"

namespace
{
	using namespace std;

	struct Args
	{
		string fileName;
		Grammar::Side grammarSide;
	};

	Args ParseArgs(int argc, char* argv[])
	{
		Args args;
		if (argc < 3)
		{
			throw invalid_argument("Usage: program.exe <filename.exe> <gramma_side>");
		}
		args.fileName = argv[1];
		string grammarSide = argv[2];
		if (grammarSide == "left")
		{
			args.grammarSide = Grammar::Side::Left;
		}
		else if (grammarSide == "right")
		{
			args.grammarSide = Grammar::Side::Right;
		}
		else
		{
			throw invalid_argument("Side should be left or right");
		}
		return args;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		Args args = ParseArgs(argc, argv);
		Grammar grammar(args.fileName, args.grammarSide);
		DFA dfa(grammar);
		dfa.Minimize();
		dfa.Print(cout);
		dfa.Display("output");
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}