//subset_text_file
//Author: Sarah P. Flanagan, sflanagan@bio.tamu.edu
//Date: 13 March 2015
//
//Purpose of this program:
//

//Required information: 
//(1) a text file with data and 
//(2) a file with a list of strings to match
//The data file has a header and the names to match are in the first column
//both should be text files, data file should be tab delimited
//list file needs to have each query on its own line

//Output:
//One file with the subset of data requested by the file

//Usage:
//Takes a list of query strings and creates a new file with data for individual observations matching the queries 
//-i: text file with data (including path)
//-l: list of queries (including path)
//-o: output file name (including path). Default: subset_data.txt
//-h: Prints help message
//no arguments: interactive mode (as opposed to passing arguments to the program)

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cstring>

using namespace std;

bool FileTest(ifstream& file, string filename)
{
	cout << filename;
	if (file.is_open())
		cout << " open\n";
	else
	{
		while (!file.is_open())
		{
			cout << " not open. Please re-enter filename: ";
			getline(cin, filename, '\n');
			file.open(filename);
		}
	}
	return true;
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
	//this code is adapted from a post on stackoverflow:
	// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
	//written by user763305
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	streambuf* sb = is.rdbuf();

	for(;;) 
	{
		int c = sb->sbumpc();
		switch (c) 
		{
			case '\n':
				return is;
			case '\r':
				if(sb->sgetc() == '\n')
					sb->sbumpc();
				return is;
			case EOF:
				// Also handle the case when the last line has no line ending
				if(t.empty())
					is.setstate(std::ios::eofbit);
				return is;
			default:
				t += (char)c;
		}
	}
}

int main(int argc, char* argv[])
{
	int end, count, i;
	size_t t;
	string list_file_name, data_file_name, out_file_name;
	ifstream list_file, data_file;
	ofstream out_file;
	vector <string> queries;
	istringstream lin;
	string data_name, line;
	bool interactivemode = true;
	string tempstring1, tempstring2, query;

	list_file_name = "default";
	data_file_name = "default";
	out_file_name = "subset_data.txt";

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nsubset_text_file:\n";
			cout << "Takes a list of query strings and creates a new file with data for individual observations matching the queries   \n";
			cout << "-i:\ttext file with data (including path)\n";
			cout << "-l:\tlist of queries (including path)\n";
			cout << "-o:\toutput file name (including path). Default: ordered_contigs.txt\n";
			cout << "-h:\tPrints this message\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
	}

	if (argc > 1)
	{
		tempstring1 = argv[1];
		if (tempstring1 == "-h")
		{
			cout << "\nsubset_text_file:\n";
			cout << "Takes a list of query strings and creates a new file with data for individual observations matching the queries  \n";
			cout << "-i:\ttext file with data (including path)\n";
			cout << "-l:\tlist of queries (including path)\n";
			cout << "-o:\toutput file name (including path). Default: ordered_contigs.txt\n";
			cout << "-h:\tPrints this message\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
	}

	for (i = 1; i < argc - 1; i++)
	{
		tempstring1 = argv[i];
		tempstring2 = argv[i + 1];
		if (tempstring1 == "-i")
			data_file_name = tempstring2;
		if (tempstring1 == "-l")
			list_file_name = tempstring2;
		if (tempstring1 == "-o")
			out_file_name = tempstring2;
	}

	if (data_file_name == "default")
	{
		cout << "Input data text file name (including path):\n";
		cin >> data_file_name;
		interactivemode = true;
	}

	if (list_file_name == "default")
	{
		cout << "\nInput list of queries text file name (including path):\n";
		cin >> list_file_name;
		interactivemode = true;
	}

	if (out_file_name == "subset_data.txt")
	{
		cout << "\nProvide output file name (including path)? Default is subset_data.txt in this directory. If that is ok, enter 'y':\n";
		cin >> out_file_name;
		if (out_file_name == "y" || out_file_name == "Y")
			out_file_name = "subset_data.txt";
		interactivemode = true;
	}

	cout << "\n\nData File:\t" << data_file_name;
	cout << "\nList File:\t" << list_file_name;
	cout << "\nOutput File:\t" << out_file_name;

	if (interactivemode)
	{
		cout << "\n\nProceed? (y to proceed)\n";
		cin >> query;

		if (query != "y" && query != "Y")
		{
			cout << "\n\nEnter an integer to exit!!\n";
			cin >> i;
			return 0;
		}
	}

	cout << "\n\nProceeding...\n";


	list_file.open(list_file_name);
	FileTest(list_file, list_file_name);

	while (safeGetline(list_file, line))
	{
		queries.push_back(line);
	}
	list_file.close();

	data_file.open(data_file_name);
	FileTest(data_file, data_file_name);
	out_file.open(out_file_name);
	count = 0;
	while (safeGetline(data_file, line))
	{
		if (count == 0)
		{
			out_file << line;
			count++;
		}
		else
		{
			istringstream ss(line);
			ss >> data_name;
			for (t = 0; t < queries.size(); t++)
			{
				if (data_name.compare(queries[t])==0)
				{
					out_file << '\n' << line;
				}
			}
			count++;
		}
	}
	data_file.close();
	out_file.close();

	cout << "Done!\nInput integer to quit.\n";
	cin >> end;
	return 0;
}


