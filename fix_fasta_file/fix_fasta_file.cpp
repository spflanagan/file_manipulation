//Author: Sarah P. Flanagan
//Date: 14 May 2015
//Purpose: Remove unnecessary end of lines in the middle of sequences in a fasta file.
//Usage: 
//./fix_fasta_file
//-f: fasta file input
//-o: Optional output file name (including path). Default: [input_fasta_name].new.fasta
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
	int end, i, num_records;
	string fasta_in_name, fasta_out_name;
	ifstream fasta_in;
	ofstream fasta_out;
	vector<fasta_record> fasta_data;
	bool interactivemode;
	string query, tempstring1, tempstring2;
	fasta_out_name = "default";

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nfix_fasta_file:\n";
			cout << "Take a fasta file that has sequences split onto multiple lines and put each sequence on only one line\n";
			cout << "-f:\tfasta file input\n";
			cout << "-o:\tOptional output file name (including path). Default: [input_fasta_name].new.fasta\n";
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
			cout << "\nfix_fasta_file:\n";
			cout << "Take a fasta file that has sequences split onto multiple lines and put each sequence on only one line\n";
			cout << "-f:\tfasta file input\n";
			cout << "-o:\tOptional output file name (including path). Default: [input_fasta_name].new.fasta\n";
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
		if (tempstring1 == "-o")
			fasta_out_name = tempstring2;
	}

	if (interactivemode)
	{
		cout << "Please provide input fasta file name\n";
		cin >> fasta_in_name;
		cout << "Default output name is: " << fasta_in_name.substr(0, fasta_in_name.size() - 5) + "new.fasta" << ". Is this acceptable? Y or N\n";
		if (tempstring1 == "N" || tempstring1 == "n")
		{
			cout << "Provide output file name:\n";
			cin >> fasta_out_name;
		}
		else
			fasta_out_name = fasta_in_name.substr(0, fasta_in_name.size() - 5) + "new.fasta";
	}

	if (fasta_out_name == "default")
	{
		fasta_out_name = fasta_in_name.substr(0, fasta_in_name.size() - 5) + "new.fasta";
	}
	cout << "\nInput file:\t" << fasta_in_name;
	cout << "\nOutput file:\t" << fasta_out_name;
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


	fasta_out_name = fasta_in_name.substr(0, fasta_in_name.size() - 5) + "new.fasta";
	
	fasta_in.open(fasta_in_name);
	FileTest(fasta_in, fasta_in_name);
	num_records = read_fasta_record(fasta_in, fasta_data);
	fasta_in.close();
	cout << fasta_in_name << " had " << num_records << " sequences.\n";

	cout << "Writing to new file, " << fasta_out_name << '\n';
	fasta_out.open(fasta_out_name);
	fasta_out << ">" << fasta_data[0].seq_id << '\n' << fasta_data[0].sequence;
	for (i = 1; i < num_records; i++)
	{
		fasta_out << "\n>" << fasta_data[i].seq_id << '\n' << fasta_data[i].sequence;
	}
	fasta_out.close();
	
	if (interactivemode)
	{
		cout << "\nDone! Input integer to quit: ";
		cin >> end;
	}
	return 0;
}