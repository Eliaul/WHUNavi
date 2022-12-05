#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <ranges>
#include <format>
#include <random>
#include <initializer_list>
#include "LazyEvaluation.hpp"
#include "Error.hpp"

namespace LinerAlgebra
{
#define _TMP template <size_t, size_t, typename>

    constexpr size_t Dynamic = 0;

    constexpr bool isLittle(size_t row, size_t col)
    {
        return (row * col != 0 && row * col < 280) ? 1 : 0;
    }

    template <_TMP typename Derived, size_t _Row, size_t _Col, typename _Ty, size_t judge = isLittle(_Row, _Col)>
    class Base;

    #pragma region "Matrix in heap"
    template <_TMP typename Derived, size_t _Row, size_t _Col, typename _Ty>
    class Base<Derived, _Row, _Col, _Ty, 0>
    {
    protected:
        std::vector<_Ty> elements;
        size_t row;
        size_t col;
        Base() : elements(_Row * _Col), row(_Row), col(_Col) {}
        Base(size_t row, size_t col);

    public:
        constexpr static bool IsDynamic() { return _Row * _Col == 0; }
    };
    #pragma endregion

    #pragma region "Matrix in stack"
    template <_TMP typename Derived, size_t _Row, size_t _Col, typename _Ty>
    class Base<Derived, _Row, _Col, _Ty, 1>
    {
    protected:
        std::array<_Ty, _Row * _Col> elements;
        size_t row;
        size_t col;
        Base() : elements(), row(_Row), col(_Col) {}
        Base(size_t row, size_t col) : elements(), row(_Row), col(_Col) {}
    public:
        constexpr static bool IsDynamic() { return false; }
    };
    #pragma endregion

    #pragma region "Heap matrix function"
    template <_TMP typename Derived, size_t _Row, size_t _Col, typename _Ty>
    inline Base<Derived, _Row, _Col, _Ty, 0>::Base(size_t row, size_t col) : Base()
    {
        if constexpr (IsDynamic())
        {
            elements.resize(row * col);
            this->row = row;
            this->col = col;
        }
    }
    #pragma endregion

    #pragma region "Stack matrix function"

    #pragma endregion


}