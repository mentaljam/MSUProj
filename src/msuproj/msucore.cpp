#include <msucore.h>


const std::string msumr::comma2dot(std::string str)
{
    unsigned int size = str.size();
    for (unsigned int i = 0; i < size; ++i)
        if (str[i] == ',')
            str[i] = '.';

    return str;
}
