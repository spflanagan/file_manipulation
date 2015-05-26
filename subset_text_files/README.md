# subset_text_files
#This program reads in a file full of data and then pulls out only the relevant entries from a list


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
