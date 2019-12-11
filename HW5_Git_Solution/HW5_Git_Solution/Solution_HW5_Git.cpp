// HW5.cpp : Defines the entry point for the console application.
// Text Analyzer that utilizes hash tables.
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__, __LINE__)
#define new DBG_NEW
#endif

#include <stdio.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

#include "HW5.h"

using namespace std;

class FileReader
{
public:
	FileReader(string filename)
	{
		fileStream = new ifstream(filename);
		if (!fileStream->good())
		{
			throw invalid_argument("couldn't open file");
		}
	};
	~FileReader()
	{
		delete fileStream;
	};

	bool GetNextWord(string &word)
	{
		while (true)
		{
			// try to read the next word from the current line
			char *next = strtok_s((strtokContext == nullptr) ? currentLine : nullptr,
				" \t.,:;!?\"",
				&strtokContext);
			if (next != nullptr)
			{
				word.assign(next);
				return true;
			}
			else
			{
				*currentLine = '\0';
				strtokContext = nullptr;
			}

			string nextLine;
			getline(*fileStream, nextLine);
			if (!fileStream->good())
			{
				return false;
			}

			strcpy_s(currentLine, nextLine.c_str());
			for (unsigned int i = 0; currentLine[i] != '\0'; i++)
			{
				currentLine[i] = tolower(currentLine[i]);
			}
		}

		return false;
	}

private:
	ifstream *fileStream = nullptr;
	char currentLine[256] = {};
	char *strtokContext = nullptr;
};

void TestHashTable(TextAnalyzer *analyzer)
{
    pair<string, unsigned int> shouldFind[] = {
        { "and" , 90 },
        { "her" , 28 },
        { "mine" , 7 },
        { "beauty", 2 },
        { "modesty", 2 },
        { "paradise", 1 },
    };

    for (auto entry : shouldFind)
    {
        unsigned int resultCount = analyzer->GetCount(entry.first);
        if (entry.second != resultCount) 
        {
            printf("ERROR! ERROR! ERROR! ERROR!\t");
        }
        else
        {
            printf("SUCCESS!\t");
        }
        printf("%s\texpected:%d\tactual:%d\r\n", entry.first.c_str(), entry.second, resultCount);        
    }
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	FileReader fr = FileReader("HW5input.txt");
	string word;

    unique_ptr<TextAnalyzer> analyzer;

    analyzer = make_unique<TextAnalyzer>();
    analyzer = make_unique<TextAnalyzer>(256,
                                         [](std::string word)
    {
        static const int hashNumbers[] = { 1, 3, 5, 7, 11, 13, 17, 19, 23, 31, 41, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
        unsigned int value = 0;
        for (size_t i = 0; i < word.length(); i++)
        {
            value += word.at(i) * hashNumbers[i % (sizeof(hashNumbers) / sizeof(*hashNumbers))];
        }
        return value;
    });

    while (fr.GetNextWord(word))
    {
        analyzer->Insert(word);
    }

    analyzer->PrintHashEntries();

    analyzer->PrintHashStats();

    TestHashTable(analyzer.get());

	cout << "press enter to exit";
	getc(stdin);
	return 0;
}

