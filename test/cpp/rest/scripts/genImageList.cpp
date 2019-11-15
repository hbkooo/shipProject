#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
using namespace std;
namespace fs = boost::filesystem;

void scanFilesRecursive(string &rootPath, const string &target)
{
    fs::path fullpath = fs::system_complete(fs::path(rootPath));

    if (!fs::exists(fullpath))
    {
        cout << fullpath << " is not exists." << endl;
        return;
    }
    unsigned num = 0;
    vector<string> container;
    fs::recursive_directory_iterator dirs(rootPath, fs::symlink_option::recurse);
    ofstream is_out(target, std::fstream::out | std::fstream::trunc);
    std::string relPath;
    for (auto &iter : dirs)
    {
        try
        {
            if (fs::is_directory(iter))
            {
                // std::cout << iter << "is dir" << std::endl;
                // container.push_back(iter.path().string());
            }
            else
            {
                relPath = iter.path().string().substr(15);
                // std::cout << relPath << std::endl;
                container.push_back(relPath);
                // is_out << iter.path().string() << std::endl;
                // std::cout << iter << " is a file" << std::endl;
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << ex.what() << std::endl;
            continue;
        }
        num++;
        if (num % 100000 == 0)
        {
            cout << num << endl;
            for (auto &name : container)
                is_out << name << endl;
            container.clear();
            this_thread::sleep_for(chrono::microseconds(500));
        }
    }
    for (auto &name : container)
        is_out << name << endl;
    container.clear();
    is_out.close();
    return;
}

void printHelp()
{
    cout << "genFileList [source dir] [target file]" << endl;
}

int main(int argc, char *argv[])
{

    string source(".");
    string target("image_list.base");
    if (argc == 2)
    {
        source = argv[1];
    }
    else if (argc == 3)
    {
        source = argv[1];
        target = argv[2];
    }
    else
        printHelp();
    cout << "scan dir    : " << source << endl;
    cout << "target file : " << target << endl;
    cout << "--------processing--------" << endl;
    scanFilesRecursive(source, target);
    return 0;
}
