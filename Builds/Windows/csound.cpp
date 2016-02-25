#include <stdio.h>
#include <windows.h>
#include "csound.hpp"
#include <iostream>

/* command line version of Csound that ships with Cabbageon Windows. */

using namespace std;
//set opcodedir for this session only
void setOpcodeDirEnv();
string getExePath();

int main(int argc, char *argv[])
{
//Create an instance of Csound
Csound* csound = new Csound();

cout << 
"\n============================================================\n\
Cabbage - Command Line Interface for Csound. If you were not \n\
expecting to see this message you should modify your system path.\n\
Otherwise please ignore this message\n\
================================================================\n"; 

setOpcodeDirEnv();

if(argc==1)
  {
    cout << "\nERROR. You need to pass a file name\n"; 
    return 0;
  }
//compile instance of csound.
csound->Compile(argc, argv);

//prepare Csound for performance
csound->Start();

//perform entire score
csound->Perform();  

//free Csound object
delete csound;

return 0;
}

string getExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);

}

void setOpcodeDirEnv()
{
        string env = "OPCODE6DIR="+getExePath();
        cout << env << endl;
        _putenv(env.c_str());
        string setCLI = "set "+env;
        system(setCLI.c_str());
}