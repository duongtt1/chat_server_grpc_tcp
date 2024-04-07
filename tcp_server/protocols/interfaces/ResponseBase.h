#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>

class ResponseBase {
public:
    uint8_t    statusCode;

    ResponseBase( ) { }
    ~ResponseBase() { }
};

