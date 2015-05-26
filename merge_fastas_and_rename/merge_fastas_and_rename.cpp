//Author: Sarah P. Flanagan
//Date: 2 April 2015
//Purpose: to read in multiple fasta files and rename them for use in the paml analysis pipeline

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

int read_fasta_record(ifstream &fasta_file, vector <fasta_record> &record, string spp, int location = 0)
{
	int count = location;
	int seq_count;
	string line, base_seq_name;
	stringstream new_name;
	spp = spp + "|";
	while (universal_getline(fasta_file, line))
	{
		if (!fasta_file.eof())
		{
			if (line.substr(0, 1) == ">")
			{
				//it's the name
				record.push_back(fasta_record());
				count++;
				istringstream seq(line.substr(1, line.size()));
				seq >> base_seq_name;
				base_seq_name = spp + base_seq_name;
				seq_count = 1;
				for (size_t t = 1; t < record.size() - 1; t++)
				{
					if (base_seq_name.compare(record[t].seq_id.substr(record[t].seq_id.size() - 2, record[t].seq_id.size())) == 0)
						seq_count++;
				}
				new_name << base_seq_name << "_" << seq_count;
			//	cout << new_name.str() << " ";
				record.at(record.size() - 1).seq_id = new_name.str();
				new_name.str(string());
			}
			else
			{//it's the sequence
				if (line.substr(0, 1) != "\n")
					record.at(record.size() - 1).sequence.append(line);
			}
		}
	}
	cout << "Fasta file read!\n";
	return count;
}

int main(int argc, char* argv[])
{
	int end, count, linecount, i;
	size_t t;
	string fasta_name, abbr, out_name, line, seq_id;
	stringstream lin;
	vector<string> fasta_file_names, spp_abbr;
	vector <fasta_record> records;
	ofstream outfile;
	string tempstring1, tempstring2, query;
	bool interactivemode = false;

	
	out_name = "combined_reads.fasta";

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nmerge_fastas_and_rename:\n";
			cout << "combine multiple fasta files \n";
			cout << "-f:\tFasta file name, including path. Must be separated by commas (no spaces in between)\n";
			cout << "-n:\tList of species codes, in the same order as fasta file names, separated by commas (no spaces in between)\n";
			cout << "-o:\toutput file name (including path). Default: combined_reads.fastq\n";
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
			cout << "\nmerge_fastas_and_rename:\n";
			cout << "combine multiple fasta files \n";
			cout << "-f:\tFasta file name, including path. Must be separated by commas (no spaces in between)\n";
			cout << "-n:\tList of species codes, in the same order as fasta file names, separated by commas (no spaces in between)\n";
			cout << "-o:\toutput file name (including path). Default: combined_reads.fastq\n";
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
			fasta_file_names.push_back(tempstring2);
		if (tempstring1 == "-n")
			abbr = tempstring2;
		if (abbr != "f" || abbr != "F" || abbr != "False" || abbr != "false" || abbr != "FALSE")
			spp_abbr.push_back(abbr);
		if (tempstring1 == "-o")
			out_name = tempstring2;
	}

	if (interactivemode)
	{
		cout << "Provide Fasta file names,, separated by commas:\n";
		cin >> fasta_name;
		istringstream names(fasta_name);
		while (getline(names, line, ','))
			fasta_file_names.push_back(line);
		cout << "Provide species abbreviations, separated by commas:\n";
		cin >> abbr;
		istringstream spp(abbr);
		while (getline(spp, line, ','))
			spp_abbr.push_back(line);
		cout << "Provide output directory:\n";
		cin >> out_name;
	}

	if (fasta_file_names.size() == 0)
	{
		cout << "\nProvide at least one fasta file name (separate multiples with a comma)\n";
		cin >> fasta_name;
		istringstream names(fasta_name);
		while (getline(names, line, ','))
			fasta_file_names.push_back(line);
		interactivemode = true;
	}
	if (spp_abbr.size() == 0)
	{
		cout << "Provide species abbreviations, separated by commas:\n";
		cin >> abbr;
		istringstream spp(abbr);
		while (getline(spp, line, ','))
			spp_abbr.push_back(line);
		interactivemode = true;
	}
	if (out_name == "combined_reads.fasta")
	{
		cout << "\nOutput file name defaults to combined_reads.fasta. Is this acceptable? Y or N\n";
		cin >> tempstring1;
		if (tempstring1 == "N" || tempstring1 == "n")
		{
			cout << "Provide preferred output file name:\n";
			cin >> out_name;
		}
		interactivemode = true;
	}
	for (t = 0; t < fasta_file_names.size(); t++)
	{
		cout << "\nFasta file, including path:\t" << fasta_file_names[t];
		cout << "\nSpecies abbreviation:\t" << spp_abbr[t];
	}
	cout << "\nOutput directory name:\t" << out_name;
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
	fasta_file_names.push_back("E://ubuntushare//ORF_ovary_testes//Trinity_CAMovary_ORFs.fasta");
	fasta_file_names.push_back("E://ubuntushare//ORF_ovary_testes//Trinity_CHAovary_ORFs.fasta");
	fasta_file_names.push_back("E://ubuntushare//ORF_ovary_testes//Trinity_Sfloovary_ORFs.fasta");
	fasta_file_names.push_back("E://ubuntushare//ORF_ovary_testes//TrinityORF_SScoovary.fasta");

	spp_abbr.push_back("cam");
	spp_abbr.push_back("cha");
	spp_abbr.push_back("sfl");
	spp_abbr.push_back("ssc");

	int total_num_seqs = 0;
	int seq_counter = 0;
	for (t = 0; t < fasta_file_names.size(); t++)
	{
		ifstream fasta_file;
		fasta_file.open(fasta_file_names[t]);
		FileTest(fasta_file, fasta_file_names[t]);
		seq_counter = read_fasta_record(fasta_file, records, spp_abbr[t], total_num_seqs);
		total_num_seqs = total_num_seqs + seq_counter;
		fasta_file.close();
	}
	cout << total_num_seqs << " sequences found\n";
	outfile.open(out_name);
	cout << "Writing sequences to " << out_name << '\n';
	count = 0;
	for (t = 0; t < records.size(); t++)
	{
		if (count == 0)
			outfile << ">" << records[t].seq_id << '\n' << records[t].sequence;
		else
			outfile << "\n>" << records[t].seq_id << '\n' << records[t].sequence;
		count++;
	}
	outfile.close();
	
	cout << "\nDone! Input integer to quit:\t";
	cin >> end;
	return 0;
}
