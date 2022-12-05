#pragma once

#include "Base.hpp"
#include "LazyEvaluation.hpp"
#include <iostream>

namespace LinerAlgebra
{
#define _MAT Matrix<_Row, _Col, _Ty>
#define _BASE Base<Matrix, _Row, _Col, _Ty>

    template <size_t _Row, size_t _Col, typename _Ty = double>
    class Matrix : public Base<Matrix, _Row, _Col, _Ty>
    {
    public:
        ~Matrix() { std::cout << "DeConstructor!"; }
        static_assert((_Row != 0 && _Col != 0) || (_Row == 0 && _Col == 0), "Parameters only none zero or both zero");
        using StorageType = decltype(Base<Matrix, _Row, _Col, _Ty>::elements);
        using ValueType = typename _Ty;
        using MatrixType = typename _MAT;
        constexpr size_t Row() const noexcept { return _BASE::row; }
        constexpr size_t Col() const noexcept { return _BASE::col; }
        constexpr size_t Capacity() const noexcept;
        constexpr size_t Size() const noexcept { return this->elements.size(); }
        Matrix() : Base<Matrix, _Row, _Col, _Ty>() {}
        Matrix(size_t row, size_t col) : Base<Matrix, _Row, _Col, _Ty>(row, col) {}
        Matrix(std::initializer_list<std::initializer_list<_Ty>> list);

#pragma region "Operator overloading"
        _Ty &operator()(size_t row, size_t col)
        {
            return *(this->elements.begin() + (row * Col() + col));
        }
        const _Ty operator()(size_t row, size_t col) const { return this->elements[row * Col() + col]; }
        _Ty &operator[](size_t index) { return *(this->elements.begin() + index); }
        const _Ty operator[](size_t index) const { return this->elements[index]; }
        template <lazy::arithmetic _Ty>
        auto operator+(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<ExprScalar<_Ty>>;
            return BinaryOperator<AddOperatorType, ExprT1, ExprT2>(addOpt, ExprT1(*this), ExprT2(other));
        }
        template <lazy::expression _Ty>
        auto operator+(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT = ExprStart<Matrix>;
            return BinaryOperator<AddOperatorType, ExprT, _Ty>(addOpt, ExprT(*this), other);
        }
        template <_TMP class _T, size_t _Row, size_t _Col, typename _Ty>
        auto operator+(const _T<_Row, _Col, _Ty> &other) const
        {
            if (!IsSizeMatch(other))
                throw SizeExcept();
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<_T<_Row, _Col, _Ty>>;
            return BinaryOperator<AddOperatorType, ExprT1, ExprT2>(addOpt, ExprT1(*this), ExprT2(other));
        }
        template <lazy::arithmetic _Ty>
        auto operator-(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<ExprScalar<_Ty>>;
            return BinaryOperator<SubtractOperatorType, ExprT1, ExprT2>(subOpt, ExprT1(*this), ExprT2(other));
        }
        template <lazy::expression _Ty>
        auto operator-(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT = ExprStart<Matrix>;
            return BinaryOperator<SubtractOperatorType, ExprT, _Ty>(subOpt, ExprT(*this), other);
        }
        template <_TMP class _T, size_t _Row, size_t _Col, typename _Ty>
        auto operator-(const _T<_Row, _Col, _Ty> &other) const
        {
            if (!IsSizeMatch(other))
                throw SizeExcept();
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<_T<_Row, _Col, _Ty>>;
            return BinaryOperator<SubtractOperatorType, ExprT1, ExprT2>(subOpt, ExprT1(*this), ExprT2(other));
        }
        template <lazy::arithmetic _Ty>
        auto operator*(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<ExprScalar<_Ty>>;
            return BinaryOperator<MultipleOperatorType, ExprT1, ExprT2>(mulOpt, ExprT1(*this), ExprT2(other));
        }
        template <lazy::arithmetic _Ty>
        auto operator/(const _Ty &other) const
        {
            using namespace lazy;
            using ExprT1 = ExprStart<Matrix>;
            using ExprT2 = ExprStart<ExprScalar<_Ty>>;
            return BinaryOperator<DivideOperatorType, ExprT1, ExprT2>(divOpt, ExprT1(*this), ExprT2(other));
        }
#pragma endregion

        constexpr Matrix &Resize(size_t row, size_t col);
        constexpr void Reserve(size_t row, size_t col);
        template <_TMP class _T, size_t _Row, size_t _Col, typename _Ty>
        bool IsSizeMatch(const _T<_Row, _Col, _Ty> &other) const noexcept { return Row() == other.Row() && Col() == other.Col(); }

        static Matrix Identity();
        static Matrix Identity(size_t rank);
        static Matrix Random();
        static Matrix Random(size_t row, size_t col);
    };

    template <lazy::arithmetic _Num, size_t _Row, size_t _Col, typename _Ty>
    auto operator*(const _Num& num, const Matrix<_Row, _Col, _Ty> &mat)
    {
        return mat * num;
    }

    template <typename _Ty = double>
    using DMatrix = Matrix<Dynamic, Dynamic, _Ty>;

    template <size_t _Row, size_t _Col, typename _Ty>
    inline Matrix<_Row, _Col, _Ty>::Matrix(std::initializer_list<std::initializer_list<_Ty>> list)
        : Matrix(list.size(), list.begin()->size())
    {
        for (size_t i = 0; i < list.size(); ++i)
        {
            for (size_t j = 0; j < list.begin()->size(); ++j)
            {
                this->elements[i * this->col + j] = *((list.begin() + i)->begin() + j);
            }
        }
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline constexpr size_t _MAT::Capacity() const noexcept
    {
        if constexpr (_MAT::IsDynamic())
            return this->elements.capacity();
        else
            return this->elements.size();
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline constexpr Matrix<_Row, _Col, _Ty> &_MAT::Resize(size_t row, size_t col)
    {
        if constexpr (_MAT::IsDynamic())
        {
            this->elements.resize(row * col);
            this->row = row;
            this->col = col;
            return *this;
        }
        else
            return *this;
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline constexpr void _MAT::Reserve(size_t row, size_t col)
    {
        if constexpr (_MAT::IsDynamic())
            this->elements.reserve(row * col);
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline Matrix<_Row, _Col, _Ty> _MAT::Identity()
    {
        if constexpr (!_MAT::IsDynamic())
        {
            if constexpr (_Row != _Col)
                throw "不是方阵!";
            Matrix<_Row, _Col, _Ty> res;
            for (size_t i = 0; i < res.row; ++i)
            {
                for (size_t j = 0; j < res.col; ++j)
                {
                    res(i, j) = (i == j) ? 1 : 0;
                }
            }
            return res;
        }
        else
            return Matrix<_Row, _Col, _Ty>();
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline Matrix<_Row, _Col, _Ty> _MAT::Identity(size_t rank)
    {
        if constexpr (_MAT::IsDynamic())
        {
            Matrix<_Row, _Col, _Ty> res(rank, rank);
            for (size_t i = 0; i < res.row; ++i)
            {
                for (size_t j = 0; j < res.col; ++j)
                {
                    res(i, j) = (i == j) ? 1 : 0;
                }
            }
            return res;
        }
        else
            return _MAT::Identity();
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline Matrix<_Row, _Col, _Ty> _MAT::Random()
    {
        if constexpr (!_MAT::IsDynamic())
        {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<_Ty> dist(0.0, 1.0);
            Matrix<_Row, _Col, _Ty> res;
            std::ranges::for_each(res.elements, [&](_Ty &t)
                                  { t = dist(mt); });
            return res;
        }
        else
            return Matrix<_Row, _Col, _Ty>();
    }

    template <size_t _Row, size_t _Col, typename _Ty>
    inline Matrix<_Row, _Col, _Ty> _MAT::Random(size_t row, size_t col)
    {
        if constexpr (_MAT::IsDynamic())
        {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<_Ty> dist(0.0, 1.0);
            Matrix<_Row, _Col, _Ty> res(row, col);
            std::ranges::for_each(res.elements, [&](_Ty &t)
                                  { t = dist(mt); });
            return res;
        }
        else
            return _MAT::Random();
    }
}

namespace std
{
    template <size_t _Row, size_t _Col, typename _Ty>
    struct formatter<LinerAlgebra::Matrix<_Row, _Col, _Ty>> : formatter<_Ty>
    {
        auto format(const LinerAlgebra::Matrix<_Row, _Col, _Ty> &matrix, format_context &ctx)
        {
            for (size_t i = 0; i < matrix.Row(); ++i)
            {
                for (size_t j = 0; j < matrix.Col(); ++j)
                {
                    formatter<_Ty>::format(matrix(i, j), ctx);
                    format_to(ctx.out(), " ");
                }
                format_to(ctx.out(), "\n");
            }
            return ctx.out();
        }
    };
}
