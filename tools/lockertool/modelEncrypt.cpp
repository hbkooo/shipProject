#include "libAuth/modelCrypto.h"
#include <iostream>

using namespace std;
using namespace auth;

void help()
{
    cout << "Error. Usage: modelEncrypt [inputFilePath] [outputFileDir]." << endl;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        help();
        return 1;
    }
    
    ModelCrypto modelCrypto;
    if (!modelCrypto.encrypt2(argv[1], argv[2]))
    {
        cerr << argv[1] << " model encrypt failed." << endl;
    }
    return 0;
}