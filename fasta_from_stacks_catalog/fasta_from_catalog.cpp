//Author: Sarah P. Flanagan, sflanagan@bio.tamu.edu
//Date: 8 April 2015
//Purpose: Create fasta file from a list of RAD tags
//Input: (1) a list with the RAD locus ID (2) batch_x.catalog.tags.tsv from stacks



#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;


void FileTest(ifstream& file, string filename)
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

int main(int argc, char* argv[])
{
	int end, i, ii, locus, fasta_count;
	ifstream list_file, tags_file;
	ofstream fasta_file;
	string list_file_name, tags_file_name, fasta_file_name;
	vector <int> loci;
	string line, seq, chrom, temp, bp;
	string tempstring1, tempstring2, query;
	bool interactivemode = false;

	list_file_name = "";
	tags_file_name = "batch_1.catalog.tags.tsv";
	fasta_file_name = "output.fasta";

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nfasta_from_catalog:\n";
			cout << "Create fasta file from a list of RAD tag IDs and the stacks catalog file \n";
			cout << "-c:\tStacks catalog file, including path (default is batch_1.catalog.tags.tsv in this directory)\n";
			cout << "-l:\tList of RAD tag Locus IDs to select (including path)\n";
			cout << "-o:\toutput file name (including path). Default: output.fasta\n";
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
			cout << "\nfasta_from_catalog:\n";
			cout << "Create fasta file from a list of RAD tag IDs and the stacks catalog file \n";
			cout << "-c:\tStacks catalog file, including path (default is batch_1.catalog.tags.tsv in this directory)\n";
			cout << "-l:\tList of RAD tag Locus IDs to select (including path)\n";
			cout << "-o:\toutput file name (including path). Default: output.fasta\n";
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
		if (tempstring1 == "-l")
			list_file_name = tempstring2;
		if (tempstring1 == "-c")
			tags_file_name = tempstring2;
		if (tempstring1 == "-o")
			fasta_file_name = tempstring2;
	}

	if (interactivemode)
	{
		cout << "Default catalog file name is batch_1.catalog.tags.tsv in this directory. Is this acceptable? Y or N\n";
		cin >> tempstring1;
		if (tempstring1 == "N" || tempstring1 == "n")
		{
			cout << "Provide Stacks catalog file name:\n";
			cin >> tags_file_name;
		}
		cout << "Provide file containing list of RAD Locus IDs:\n";
		cin >> list_file_name;
		cout << "Default output file name is output.fasta in this directory. Is this acceptable? Y or N\n";
		cin >> tempstring1;
		if (tempstring1 == "N" || tempstring1 == "n")
		{
			cout << "Provide output file name:\n";
			cin >> fasta_file_name;
		}
	}

	if (list_file_name == "")
	{
		cout << "\nYou must provide a file name (with path) for the list of RAD tags. Provide it here:\n";
		cin >> list_file_name;
		interactivemode = true;
	}
	cout << "\nStacks Catalog file:\t" << tags_file_name;
	cout << "\nFile with RAD Locus IDs:\t" << list_file_name;
	cout << "\nOutput directory name:\t" << fasta_file_name;
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

	list_file.open(list_file_name);
	FileTest(list_file, list_file_name);
	while (universal_getline(list_file, line))
	{
		if (!list_file.eof())
		{
			stringstream ss(line);
			ss >> i;
			loci.push_back(i);
		}
	}
	list_file.close();

	tags_file.open(tags_file_name);
	FileTest(tags_file, tags_file_name);
	fasta_file.open(fasta_file_name);
	fasta_count = 0;
	chrom = bp ="";
	while (universal_getline(tags_file, line))
	{
		if (!tags_file.eof())
		{
			stringstream ss(line);
			ss >> i >> ii >> locus;
			for (size_t t = 0; t < loci.size(); t++)
			{
				if (loci[t] == locus)
				{
					int count = 0;
					while (getline(ss, temp, '\t'))
					{
						ss >> temp;
						if (count == 1)
							chrom = temp;
						if (count == 2)
							bp = temp;
						if (temp.substr(0, 1) == "A" || temp.substr(0, 1) == "C" 
							|| temp.substr(0, 1) == "G" || temp.substr(0, 1) == "T")
							seq = temp;
					} 
					if (fasta_count == 0)
						fasta_file << ">" << locus << "|" << chrom << "." << bp << '\n' << seq;
					else
						fasta_file << "\n>" << locus << "|" << chrom << "." << bp << '\n' << seq;
					fasta_count++;
				}
			}
		}
	}
	fasta_file.close();
	tags_file.close();
	cout << "Done! Input integer to quit:\n";
	cin >> end;
	return 0;
}
