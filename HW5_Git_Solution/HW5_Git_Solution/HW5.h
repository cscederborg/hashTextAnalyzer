#pragma once

#include<iostream>
#include<string>
#include<cctype>
#include<iomanip>

using namespace std;

class TextAnalyzer
{
public:

	TextAnalyzer();
	TextAnalyzer(unsigned int, unsigned int(*func)(string));

	~TextAnalyzer();

	void Insert(string);
	unsigned int GetCount(string);
	void PrintHashEntries();
	void PrintHashStats();


protected:

	static unsigned int AlphaHash(string);

	struct AlphaNode
	{
		AlphaNode* next;
		string word;
		unsigned int count = 0;
	};

	unsigned int totalWords = 0, totalSlots = 0, uniqueWords = 0;
	unsigned int entriesPerSlot = 0;
	unsigned int* entryCountPtr = NULL;

	AlphaNode **nodeHash;

	unsigned int(*customHash)(string);

};

TextAnalyzer::TextAnalyzer()
{
	totalSlots = 27;
	//Create double pointer and initialize all values to null
	nodeHash = new AlphaNode*[totalSlots];
	for (int i = 0; i < totalSlots; i++)
	{
		nodeHash[i] = NULL;
	}

	customHash = &AlphaHash;

	//Create the count array and initialize all values to 0
	entryCountPtr = new unsigned int[totalSlots];
	for (int i = 0; i < totalSlots; i++)
	{
		entryCountPtr[i] = 0;
	}

}

TextAnalyzer::TextAnalyzer(unsigned int index, unsigned int(*func)(string))
{
	nodeHash = new AlphaNode*[index];
	for (int i = 0; i < index; i++)
	{
		nodeHash[i] = NULL;
	}

	customHash = func;

	entryCountPtr = new unsigned int[index];
	for (int i = 0; i < index; i++)
	{
		entryCountPtr[i] = 0;
	}

	totalSlots = index;
}

//Default Hash function
//Takes in the input string and returns the key
unsigned int TextAnalyzer::AlphaHash(string input)
{
	
	//Checks if the first letter is alphabetical
	if (!isalpha(input[0]))
	{
		return 0;
	}
	else
	{
		//Capitalizes the first letter
		char cap = toupper(input[0]);
		//Returns a value 1-27 based on ASCII values
		return (cap - '@');
	}
}

void TextAnalyzer::Insert(string input)
{
	int index = customHash(input);
	if (index <= totalSlots)
	{
		index = index % (totalSlots + 1);
	}
	else
	{
		index = index % totalSlots;
	}
	

	bool unique = true;

	//Case: No words have been added yet to that slot
	if (nodeHash[index] == NULL)
	{
		nodeHash[index] = new AlphaNode;
		nodeHash[index]->next = NULL;
		nodeHash[index]->word = input;
		nodeHash[index]->count++;
		entryCountPtr[index]++;
		uniqueWords++;
	}
	else //Case: 1 or more words are in that slot
	{
		AlphaNode* temp = nodeHash[index];
		while (temp->next != NULL)
		{
			
			//Checking if the word is unique by comparing as the LL is traversed
			if (temp->word == input)
			{
				unique = false;
				break;
			}
			
			temp = temp->next;
			
		}

		if (temp->word == input)
		{
			unique = false;
		}

		if (unique)
		{
			//Create the new alpha node
			temp->next = new AlphaNode;
			temp->next->next = NULL;
			temp->next->word = input;
			temp->next->count++;
			entryCountPtr[index]++;
			uniqueWords++;
			
		}
		else
		{
			temp->count++;
		}
		
	}

	totalWords++;
}

unsigned int TextAnalyzer::GetCount(string input)
{
	unsigned int searchIndex = customHash(input);
	if (searchIndex <= totalSlots)
	{
		searchIndex = searchIndex % (totalSlots + 1);
	}
	else
	{
		searchIndex = searchIndex % totalSlots;
	}

	searchIndex = searchIndex % (totalSlots + 1);

	AlphaNode* temp = nodeHash[searchIndex];

	while (temp->next != NULL)
	{

		//Checking if the word is unique by comparing as the LL is traversed
		if (temp->word == input)
		{
			break;
		}
		temp = temp->next;
	}

	return temp->count;
	
}

void TextAnalyzer::PrintHashEntries()
{

	AlphaNode* temp = nodeHash[0];

	for (int i = 0; i < totalSlots; i++)
	{
		temp = nodeHash[i];

		for (int r = 0; r < entryCountPtr[i]; r++)
		{
			cout << temp->word << " " << temp->count << endl;
			temp = temp->next;
		}
	}
}

void TextAnalyzer::PrintHashStats()
{
	int slotCounter = 0;
	double entryAvg = 0;
	int entryMax = 0;


	cout << "Hash table entries per slot: ";
	for (int i = 0; i < totalSlots; i++)
	{
		//Outputting the entries per slot
		cout << entryCountPtr[i] << " ";

		//Counting how many slots were not used
		if (entryCountPtr[i] != 0)
		{
			slotCounter++;
		}

		//Calculating Average (Addition Step)
		entryAvg += entryCountPtr[i];

		//Keeping track of the max value
		if (entryCountPtr[i] > entryMax)
		{
			entryMax = entryCountPtr[i];
		}
	}

	//Finish calculation of the average
	entryAvg = entryAvg / totalSlots;
	

	//Outputting slots used, then max and average, then uniqueWords, then totalWords
	cout << endl << slotCounter << " hash table slots used out of " << totalSlots << " total." << endl;

	cout << entryMax << " max entries in a table slot. " << fixed << setprecision(6) << entryAvg << " average entries in a table slot." << endl;

	cout << uniqueWords << " unique words found." << endl;

	cout << totalWords << " total words counted." << endl;

}

TextAnalyzer::~TextAnalyzer()
{
	for (int i = 0; i < totalSlots; i++)
	{
		AlphaNode* temp = nodeHash[i];
		AlphaNode* runner = temp;
		while (NULL != runner)
		{
			runner = runner->next;
			delete temp;
			temp = runner;
		}
	}

	//Free both arrays, entryCount is not dynamic, and the HashTable has just been freed
	delete[] entryCountPtr;
	delete[] nodeHash;
}