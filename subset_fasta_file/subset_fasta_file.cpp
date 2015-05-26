//Author: Sarah P. Flanagan
//Date: 14 May 2015
//Purpose: Remove unnecessary end of lines in the middle of sequences in a fasta file.
//Usage: 
//./subset_fasta_file
//-f: fasta file input
//-l: List of Fasta Record IDs
//-c: If included, it means that all records in list should be Combined into one file. Otherwise, one file per fasta ID.
//-h: Prints this message
//no arguments: interactive mode



#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

istream& universal_getline(istream& is, string& t)
{
	//this code is adapted from a post on stackoverflow:
	// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
	//written by user763305
	t.clear();
	istream::sentry se(is, true);
	streambuf* sb = is.rdbuf();//sets pointer to stream buffer object

	for (;;)
	{
		int c = sb->sbumpc();//get current character and advance to the next position
		switch (c)//tests for equality against a list of variables (like multiple if statements)
		{
		case '\n'://if the next character is '\n', return the line
			return is;
		case '\r'://if the character is '\n', see if the next one is '\n' or return the line
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:// Also handle the case when the last line has no line ending
			if (t.empty())//if there's nothing there, set it to be the end of file
				is.setstate(ios::eofbit);//set it to be the end of the file and return it
			return is;
		default://if none of the above, continue on.
			t += (char)c;
		}
	}

}

class fasta_record
{
public:
	string seq_id;
	string sequence;

	fasta_record()
	{
		seq_id = "";
		sequence = "";
	}
};

int read_fasta_record(ifstream &fasta_file, vector <fasta_record> &record)
{
	int count = 0;
	string line;
	while (!fasta_file.eof())
	{
		if (!fasta_file.eof())
		{
			universal_getline(fasta_file, line);
			if (line.substr(0, 1) == ">")
			{
				//it's the name
				record.push_back(fasta_record());
				count++;
				record[count - 1].seq_id = line.substr(1, line.size());
			}
			//if (line.substr(0, 1) == "A" || line.substr(0, 1) == "T" || line.substr(0, 1) == "C" || line.substr(0, 1) == "G" || line.substr(0, 1) == "N")
			else
			{//it's the sequence
				if (line.substr(0, 1) != "\n")
					record[count - 1].sequence.append(line);
			}
		}
	}
	cout << "Fasta file read!\n";
	return count;
}

int main(int argc, char* argv[])
{
	int end, i, j, num_ids, num_records;
	string fasta_in_name, fasta_out_name, list_name, line;
	ifstream fasta_in, list_in;
	ofstream fasta_out;
	vector<fasta_record> fasta_data;
	bool interactivemode, combine;
	string query, tempstring1, tempstring2;
	vector <string> scaffold_names;
	combine = false;

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nsubset_fasta_file:\n";
			cout << "Take a fasta file that has sequences split onto multiple lines and put each sequence on only one line\n";
			cout << "-f:\tfasta file input\n";
			cout << "-l:\tList of Fasta Record IDs\n";
			cout << "-c:\tIf included, it means that all records in list should be Combined into one file. Otherwise, one file per fasta ID.\n";
			cout << "-h:\tPrints this message\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
		if (query == "I" || query == "i")
			interactivemode = true;
	}

	if (argc > 1)
	{
		tempstring1 = argv[1];
		if (tempstring1 == "-h")
		{
			cout << "\nsubset_fasta_file:\n";
			cout << "Take a fasta file that has sequences split onto multiple lines and put each sequence on only one line\n";
			cout << "-f:\tfasta file input\n";
			cout << "-l:\tList of Fasta Record IDs\n";
			cout << "-c:\tIf included, it means that all records in list should be Combined into one file. Otherwise, one file per fasta ID.\n";
			cout << "-h:\tPrints this message\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
	}

	for (i = 1; i < argc - 1; i++)
	{
		interactivemode = false;
		tempstring1 = argv[i];
		tempstring2 = argv[i + 1];
		if (tempstring1 == "-f")
			fasta_in_name = tempstring2;
		if (tempstring1 == "-l")
			list_name = tempstring2;
		if (tempstring1 == "-c")
			combine = true;
	}

	if (interactivemode)
	{
		cout << "Please provide input fasta file name\n";
		cin >> fasta_in_name;
		cout << "Please provide name of file with list of fasta record IDs\n";
		cin >> list_name;
		cout << "Do you want all of the records in the list to be combined into one file? Default is one file per fasta ID. Y or N\n";
		cin >> tempstring1;
		if (tempstring1 == "Y" || tempstring1 == "y")
		{
			combine = true;
		}
	}

	if (combine)
		fasta_out_name = fasta_in_name.substr(0, fasta_in_name.size() - 5) + ".subset.fasta";
	cout << "\nInput file:\t" << fasta_in_name;
	cout << "\nList file:\t" << list_name;
	if (combine)
		cout << "\nOne file will be output, called " << fasta_out_name << ".\n";
	else
		cout << "\nMultiple files will be created, one per fasta ID.\n";
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
	else
		cout << "\n\nProceeding...\n";

	list_in.open(list_name);
	FileTest(list_in, list_name);
	while (!list_in.eof())
	{
		if (!list_in.eof())
		{
			universal_getline(list_in, line);
			cout << line << '\t';
			scaffold_names.push_back(line);
		}
	}
	list_in.close();
	num_ids = scaffold_names.size();
	cout << "\nFound " << num_ids << " fasta IDs.\n";

	

	fasta_in.open(fasta_in_name);
	FileTest(fasta_in, fasta_in_name);
	num_records = read_fasta_record(fasta_in, fasta_data);
	fasta_in.close();
	cout << fasta_in_name << " had " << num_records << " sequences.\n";
	if (combine)
	{
		cout << "\nWriting to new file, " << fasta_out_name << '\n';
		fasta_out.open(fasta_out_name);
	}
	int count = 0;
	for (i = 1; i < num_records; i++)
	{
		for (j = 0; j < num_ids; j++)
		{
			if (fasta_data[i].seq_id == scaffold_names[j])
			{
				cout << "\nMatched " << scaffold_names[j];
				if (combine)
				{
					if (count == 0)
						fasta_out << ">" << fasta_data[i].seq_id << '\n' << fasta_data[i].sequence;
					else
						fasta_out << "\n>" << fasta_data[i].seq_id << '\n' << fasta_data[i].sequence;
				}
				else
				{
					fasta_out_name = scaffold_names[j] + ".fasta";
					cout << "Writing to new file, " << fasta_out_name << '\n';
					fasta_out.open(fasta_out_name);
					fasta_out << ">" << fasta_data[i].seq_id << '\n' << fasta_data[i].sequence;
					fasta_out.close();
				}
				count++;
			}
		}
	}
	if (combine)
		fasta_out.close();
	cout << "\nFound " << count << " matches between the list and the fasta file.\n";
	if (interactivemode)
	{
		cout << "\nDone! Input integer to quit: ";
		cin >> end;
	}
	return 0;
}