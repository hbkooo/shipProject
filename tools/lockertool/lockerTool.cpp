/*
 * @Author: mumumumuxiansheng@gmail.com 
 * @Date: 2018-03-12 18:18:05 
 * @Last Modified by:   mumumumuxiansheng@gmail.com 
 * @Last Modified time: 2018-03-12 18:18:05 
 */

#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream> 

#include "libAuth/lockController.h"
#include "common/easylog++.h"
#include "cmdline.h"

#define MaxSize 4032
#define FEATURE_ID 4

using namespace std;
using namespace auth;

INITIALIZE_EASYLOGGINGPP

bool isLogin = 0;
lockController mlockController(FEATURE_ID);
bool ret = true;
void logout();

void errorHelp(bool ret, string msg)
{
    if (ret == false)
    {
        logout();
        cout << msg << endl;
        exit(1);
    }
}

//Displays the content of mdata in a nice format with an ascii
//and a hexadecimal area.
void displayMemory(unsigned char *mdata, unsigned int mdataLen)
{
    //The number of bytes to be shown in each line
    const unsigned int lineLen = 16;

    //This is printed before the first char in each line
    const char *margin = "     ";

    unsigned int i /*lines*/, ii /*chars*/;

    for (i = 0; i < mdataLen; i += lineLen)
    {
        ostringstream asciiWriter;
        ostringstream hexWriter;

        for (ii = i; ii < lineLen + i; ii++)
        {
            if (ii < mdataLen)
            {
                //Print the active char to the hex view
                unsigned int val = static_cast<unsigned int>(mdata[ii]);
                if (val < 16)
                    hexWriter << '0';
                hexWriter << uppercase << hex << val << ' ';

                //... and to the ascii view
                if (mdata[ii] < 32 || mdata[ii] > 127) //Do not print some characters
                    asciiWriter << '.';                //because this may create
                else                                   //problems on UNIX terminals
                    asciiWriter << mdata[ii];
            }
            else
            {
                hexWriter << "   ";
                asciiWriter << " ";
            }

            //Display a separator after each 4th byte
            if (!((ii - i + 1) % 4) && (ii - i + 1 < lineLen))
                hexWriter << "| ";
        }

        //Write the line
        cout << margin << hexWriter.str() << " [" << asciiWriter.str() << "]" << endl;
    }
}

void readFile(const char *fileName, unsigned char *mData, int &size)
{
    if (mData == NULL)
        errorHelp(false, "read data failed!");
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL)
        errorHelp(false, "Cannot open file!");
    size = 0;
    while (!feof(fp))
    {
        unsigned char tmp = fgetc(fp);
        mData[size++] = tmp;
        if (size > MaxSize)
        {
            fclose(fp);
            errorHelp(false, "File is too large to write!");
        }
    }
    fclose(fp);
}

void login()
{
    if (!isLogin)
    {
        ret = mlockController.login();
        errorHelp(ret, "login failed!");
        isLogin = 1;
    }
}

void logout()
{
    if (isLogin)
    {
        ret = mlockController.logout();
        errorHelp(ret, "logout failed!");
        isLogin = 0;
    }
}

void readData()
{
    unsigned char mData[MaxSize];
    ret = mlockController.readMem(mData, MaxSize);
    errorHelp(ret, "read data failed.");
    displayMemory(mData, MaxSize);
}

void writeData(string filename)
{
    int size;
    unsigned char mData[MaxSize] = {0};
    readFile(filename.c_str(), mData, size);
    ret = mlockController.writeMem(mData, MaxSize);
    errorHelp(ret, "write data failed.");
    displayMemory(mData, size);
}

void encryptData(string filename)
{
    string output = "" + filename + ".encrypt";
    ret = mlockController.encryptFile(filename, output);
    errorHelp(ret, "encrypt failed");
}

void decryptData(string filename)
{
    string output = "" + filename + ".decrypt";
    ret = mlockController.decryptFile(filename, output);
    errorHelp(ret, "decrypt failed");
}

void printlockControllerInfo()
{
    hasp_size_t memSize;
    string lockControllerInfo;
    mlockController.getMemSize(memSize);
    mlockController.getInfo(lockControllerInfo);
    cout << "lockController_INFO - Internal memory size of the lockController is " << memSize << " bytes." << endl;
    cout << lockControllerInfo << endl;
}

void test()
{
    cout << "Config Content : \n"
         << auth::lockController::getConfig(4) << endl;
}

int main(int argc, char *argv[])
{
    cmdline::parser cmd;
    cmd.add("show", 's', "Show basic information of the lockController.");
    cmd.add("read", 'r', "Read and print internal data of the lockController.");
    cmd.add<string>("write", 'w', "File to write into the lockController.", false, "");
    cmd.add<string>("encrypt", 'e', "File to encrypt.", false, "");
    cmd.add<string>("decrypt", 'd', "File to decrypt.", false, "");
    cmd.add("test", 't', "Test the 'getConfig' funcation.");
    cmd.parse_check(argc, argv);
    if (argc <= 1)
        errorHelp(false, cmd.usage());

    if (cmd.exist("test"))
        test();

    login();
    if (cmd.exist("show"))
        printlockControllerInfo();
    if (cmd.exist("read"))
        readData();
    if (cmd.exist("write"))
        writeData(cmd.get<string>("write"));
    if (cmd.exist("encrypt"))
        encryptData(cmd.get<string>("encrypt"));
    if (cmd.exist("decrypt"))
        decryptData(cmd.get<string>("decrypt"));
    logout();

    return 0;
}
