#include "usb.h"
#include <sstream>
#include <iostream>

UTILS::Cstring::Cstring() {}

UTILS::Cstring::Cstring(std::string& value) 
{
	this->assign(value);
}

std::vector<UTILS::Cstring> UTILS::Cstring::split(char delimiter)
{
    std::vector<UTILS::Cstring> strings;
    std::istringstream in(this->data());
    std::string s;

    while (std::getline(in, s, delimiter))
    {
        strings.push_back(s);
    }
    return strings;
}

int UTILS::Converter::ParseInt(UTILS::Cstring& value) 
{
    auto result = std::stoi(value);
    return result;
}

long UTILS::Converter::ParseLong(UTILS::Cstring& value) 
{
    auto result = std::stol(value);
    return result;
}

unsigned long long UTILS::Converter::ParseToHex(UTILS::Cstring& value) 
{
    unsigned long long x;
    std::stringstream in;
    in << std::hex << value;
    in >> x;
    auto result = static_cast<unsigned long long>(x);
    return result;
}

GUID UTILS::Converter::ParseToGUID(UTILS::Cstring& value) 
{
    GUID guid;
    std::vector<UTILS::Cstring> codes = value.split('-');
    int len = static_cast<int>(codes.size());

    guid.Data1 = static_cast<unsigned long>(Converter::ParseToHex(codes[0]));
    guid.Data2 = static_cast<unsigned short>(Converter::ParseToHex(codes[1]));
    guid.Data3 = static_cast<unsigned short>(Converter::ParseToHex(codes[2]));

    unsigned short ptrMSB = static_cast<unsigned short>(Converter::ParseToHex(codes[3]));
    unsigned long long ptrLSB = Converter::ParseToHex(codes[4]);

    unsigned char* ptr = guid.Data4;
    unsigned char* psrc = static_cast<unsigned char*>((void*)&ptrMSB);
    psrc++;
    for (int i = 2; i > 0; i--)
    {
        *(ptr++) = *(psrc--);
    }

    psrc = static_cast<unsigned char*>((void*)&ptrLSB);
    psrc += 5;

    for (int i = len; i >= 0; i--)
    {
        *(ptr++) = *(psrc--);
    }

    return guid;
}
