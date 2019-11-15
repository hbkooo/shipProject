#include "../src/common/uuid.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
using namespace std;

int main()
{
    boost::uuids::random_generator generator;

    boost::uuids::uuid uuid1 = generator();
    std::cout << uuid1 << std::endl;

    boost::uuids::uuid uuid2 = generator();
    std::cout << uuid2 << std::endl;
    //test convert
    string s1 = boost::lexical_cast<std::string>(uuid1);
    cout<<s1<<endl;
    return 0;
}