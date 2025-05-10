#pragma once
#include <string>
#include "../Logger/Logger.h"

#if defined(_WIN64) || defined(_WIN32)

#include "../../../Misc/UserInputW.h"

#else

#include "../../../Misc/UserInput.h"

#endif
class SVCLI
{
private:
    /* data */
public:
    ~SVCLI() = default;

    void run();
};
