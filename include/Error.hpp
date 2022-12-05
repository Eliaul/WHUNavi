#pragma once

#include <exception>

namespace LinerAlgebra
{
    class SizeExcept : public std::exception
    {
    public:
        const char* what() const throw()
        {
            return "Matrices' size mismatch!";
        }
    };
}