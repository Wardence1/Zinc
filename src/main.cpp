#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.hpp"

using namespace std;

const string TEMP_FILE_NAME = "TEMP_ASM.s";
const string TEMP_O_FILE_NAME = "TEMP_ASM.o";


int main(int argc, char** argv) {

    /* Error Handling */
    if (argc <= 2) {
        fprintf(stderr, "Useage: <input file name> <output file name>\n");
        return 1;
    }
    string Out_File_Name = argv[2];

    // Create Input file
    ifstream iFile(argv[1]);
    if (!iFile) {
        fprintf(stderr, "Couldn't read file: \"%s\"\n", argv[1]);
        return 1;
    }

    /* Lexer */
    lex(iFile);

    // Temp file
    ofstream tFile(TEMP_FILE_NAME);
    if (!tFile) {
        fprintf(stderr, "Couldn't create a temporary file.\n");
        return 1;
    }

    /*  */

    /* Write to the temp file */
    while (tFile.is_open()) {
        tFile << "section .text\nglobal _start\n_start:\nmov eax, 1\nmov ebx, 0\nint 80h\n";

        tFile.close();
    }

    /* Assemble the temp file into the output file */

    // Create the output file
    ofstream oFile(argv[2]);
    if (!oFile) {
        fprintf(stderr, "Couldn't open file: \"%s\"\n", argv[2]);
        return 1;
    }

    // Create the temp object file
    system(("nasm -f elf64 " + TEMP_FILE_NAME + " -o " + TEMP_O_FILE_NAME).c_str());
    // Delete the temp file
    //system(("rm " + TEMP_FILE_NAME).c_str());
    // Assemble the temp object file
    system(("ld " + TEMP_O_FILE_NAME + " -o " + Out_File_Name).c_str());
    // Delete the temp object file
    system(("rm " + TEMP_O_FILE_NAME).c_str());

    return 0;
}