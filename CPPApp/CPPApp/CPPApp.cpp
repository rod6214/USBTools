// CPPApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "usb.h"
//#include <Windows.h>
//#include <winusb.h>
//#include <future>
////#include <json/json.h>
//#include <stdlib.h>
//#include <string>
//#include <direct.h>
//#include <fstream>



#define GET_STATUS         0
#define CLEAR_FEATURE      1
#define SET_FEATURE        3
#define SET_ADDRESS        5
#define GET_DESCRIPTOR     6
#define SET_DESCRIPTOR     7
#define GET_CONFIGURATION  8
#define SET_CONFIGURATION  9
#define GET_INTERFACE     10
#define SET_INTERFACE     11
#define SYNCH_FRAME       12


int main()
{
    Json::Value root;
    char* buffer = _getcwd(NULL, 0);
    std::string path(buffer == NULL ? "" : buffer);
    path.append("\\config.json");
    std::ifstream file(path, std::ifstream::binary);

    file >> root;

    std::string my_encoding = root.get("guid", "UTF-32").asString();
    
    UTILS::Cstring str = my_encoding;

    auto vid = root.get("vid", "UTF-32").asString();
    UTILS::Cstring tt = vid;

    auto ff = UTILS::Converter::ParseToHex(tt);
    
    //UTILS::Cstring str = root.get("guid", "UTF-32").asString();
    //root.get("guid", "UTF-32").asString()
    /*auto strings = str.split('-');

    auto result = UTILS::Converter::ParseToHex(strings[0]);

    GUID guid = UTILS::Converter::ParseToGUID(str);*/
    //for_sequence()
    //char* values = std::strtok(my_encoding.data(), ",");

    //std::cout << root["test"] << std::endl;
    //std::string my_encoding = root.get("my-encoding", "UTF-32").asString();
    /*auto sts = std::unique_ptr<ThreadStatus>(new ThreadStatus());
    auto fns = std::unique_ptr<InternalFunctions>(new InternalFunctions(sts));
    auto test = std::unique_ptr<USB>(new USB(fns));
    test->BeginRead();

    for (int i = 0; i < 3; i++)
    {
        std::cout << "step " << i << std::endl;
    }

    test->EndRead();
    test->Wait();
    test.release();*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
