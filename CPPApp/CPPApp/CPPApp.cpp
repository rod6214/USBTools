// CPPApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "usb.h"
#include <Windows.h>
#include <winusb.h>
#include <future>

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
    auto sts = std::unique_ptr<ThreadStatus>(new ThreadStatus());
    auto fns = std::unique_ptr<InternalFunctions>(new InternalFunctions(sts));
    auto test = std::unique_ptr<USB>(new USB(fns));
    test->BeginRead();

    for (int i = 0; i < 3; i++)
    {
        std::cout << "step " << i << std::endl;
    }

    test->EndRead();
    test->Wait();
    test.release();
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
