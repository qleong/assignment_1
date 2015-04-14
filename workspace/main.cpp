
#include <string>
#include <iostream>
#include <vector>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <wait.h>

#include "stringset.h"


const string cpp_name = "/usr/bin/cpp";
string yyin_cpp_command = "";
int y_flag = 0, l_flag = 0, at_flag = 0, d_flag = 0;
string d_string;
FILE *output;
const size_t LINESIZE = 1024;


bool acceptable_file_ext(string str){
	if(str.substr(str.find_last_of(".") + 1) == "oc"){
   		return true;
	}
   	return false;
}

bool scan_opts (int argc, char** argv) {
   int option;
   opterr = 0;
   for(;;) {
      option = getopt (argc, argv, "@:lyD:");
      if (option == EOF) break;
      switch (option) {
         case '@': 
         	at_flag = 1;
         	break;
         case 'l':
         	l_flag = 1;
         	break;
         case 'y':
         	y_flag = 1;
         	break;
         case 'D':
         	d_flag = 1;
         	d_string = optarg;
         	break;
         default:  
         	return false;
         	break;
      }
   }
   return true;
}

void cpp_popen(string fileName){
	string cpp_command = cpp_name;
	cpp_command += " ";
	cpp_command += fileName;
	//cout << "command is: " << cpp_command << endl;
	const char *cmd = cpp_command.c_str();
	output = popen(cmd, "r");
}

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp(buffer, '\n');
      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, filename);
      if (sscanf_rc == 2) {
         //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         // Add all of those tokens to the stringset 
         intern_stringset((const char *) token);
         //printf ("token %d.%d: [%s]\n",
         //        linenr, tokenct, token);
      }
      ++linenr;
   }
}


int main (int argc, char **argv) {
   
   // Use getopt to get command line arguments.
   if(!scan_opts(argc, argv)){
   		fprintf(stderr, "Bad option passed as command line argument. Aborting program...\n");
   		return EXIT_FAILURE;
   }
   
   // If a '.oc' file was not passed as the file argument, abort with an error message
   string fileName = argv[argc - 1];
   string check = argv[argc - 2];
   if(!acceptable_file_ext(fileName)){
   		fprintf(stderr, "Incorrect file type supplied. Aborting...\n");
   		return EXIT_FAILURE;
   }
   if(acceptable_file_ext(check)){
   		fprintf(stderr, "Wrong number of file arguments given. Aborting...\n");
   		return EXIT_FAILURE;
   }
   
   // Check what flags have been passed at the command line the flags that are relevant in this project are: '-D  -l -y -@flag'
   if(d_flag){
   		// cout << "The D argument is: " << d_string << endl;
   		fileName = "-D " + d_string + fileName;
   }
   
   
   // Pass the input file to /usr/bin/cpp using 'popen' function.
	cpp_popen(fileName);	
	if(output == NULL){
		fprintf(stderr, "cpp failed. Returned null pointer. Aborting...\n");
		return EXIT_FAILURE;
	}
   
   // Tokenize output of /usr/bin/cpp
   char *file = new char[fileName.length() + 1];
   strcpy(file, fileName.c_str());
   cpplines(output, file);
   
   
   //TODO: Construct target file called "program".str from basename of source program
	char *path = new char[fileName.length() + 1]; 
	strcpy(path, fileName.c_str());
	string f_name = basename(path);
	int last_index = f_name.find_last_of(".");
	string base_name = f_name.substr(0, last_index);
	base_name += ".str";
   
   // TODO: 'dump' the stringset contents into a file called 'program.str'
   FILE *out_file;
   out_file = fopen(base_name.c_str(), "w");
   dump_stringset(out_file);
   
   // Close file pointer
	pclose(output);
	fclose(out_file);
	return EXIT_SUCCESS;
}

