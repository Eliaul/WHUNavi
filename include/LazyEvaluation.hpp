#pragma once

#include <concepts>
#include "Error.hpp"

namespace LinerAlgebra
{
    namespace lazy
    {
        template <typename _Ty>
        concept arithmetic = std::is_arithmetic_v<_Ty>;

        constexpr size_t Max(size_t x, size_t y)
        {
            return x < y ? y : x;
        }

#pragma region "Binary functor"
        struct AddOperatorType
        {
            constexpr explicit AddOperatorType() = default;
            template <typename LHS, typename RHS>
            auto operator()(const LHS &lhs, const RHS &rhs) const
            {
                return lhs + rhs;
            }
        };
        constexpr AddOperatorType addOpt{};

        struct SubtractOperatorType
        {
            constexpr explicit SubtractOperatorType() = default;
            template <typename LHS, typename RHS>
            auto operator()(const LHS &lhs, const RHS &rhs) const
            {
                return lhs - rhs;
            }
        };
        constexpr SubtractOperatorType subOpt{};

        struct MultipleOperatorType
        {
            constexpr explicit MultipleOperatorType() = default;
            template <typename LHS, typename RHS>
            auto operator()(const LHS &lhs, const RHS &rhs) const
            {
                return lhs * rhs;
            }
        };
        constexpr MultipleOperatorType mulOpt{};

        struct DivideOperatorType
        {
            constexpr explicit DivideOperatorType() = default;
            template <typename LHS, typename RHS>
            auto operator()(const LHS &lhs, const RHS &rhs) const
            {
                return lhs / rhs;
            }
        };
        constexpr DivideOperatorType divOpt{};
#pragma endregion

        /// @brief Base Template Expression
        /// @tparam Derived Operator
        template <typename _Derived>
        class Expr
        {
        protected:
            explicit Expr() {}
            auto operator[](size_t index) const { return GetDerived().At(index); }
            constexpr size_t Row() const { return GetDerived().GetRow(); }
            constexpr size_t Col() const { return GetDerived().GetCol(); }
            auto operator()() const { return GetDerived()(); }

        public:
            const _Derived &GetDerived() const { return static_cast<const _Derived &>(*this); }
            _Derived &GetDerived() { return static_cast<_Derived &>(*this); }
        };

        template <typename _Ty>
        concept expression = std::derived_from<_Ty, Expr<_Ty>>;

        /// @brief Scalar Template Expression
        /// @tparam _Ty Arithmetic Type
        template <arithmetic _Ty>
        struct ExprScalar
        {
        private:
            const _Ty &s;

        public:
            constexpr ExprScalar(const _Ty &s) : s(s) {}
            constexpr const _Ty &operator[](size_t) const { return s; }
            constexpr size_t Row() const { return 0; }
            constexpr size_t Col() const { return 0; }
        };

        template <typename _Ty>
        struct ExprTraits
        {
            using Type = typename const _Ty &;
        };

        template <typename _Ty>
        struct ExprTraits<ExprScalar<_Ty>>
        {
            using Type = typename ExprScalar<_Ty>;
        };

        /// @brief Start Evaluation
        /// @tparam _Ty Matrix Or Scalar
        template <typename _Ty>
        class ExprStart : public Expr<ExprStart<_Ty>>
        {
            ExprTraits<_Ty>::Type value;

        public:
            using BaseType = typename Expr<ExprStart<_Ty>>;
            using BaseType::operator[];
            using BaseType::Col;
            using BaseType::Row;

            explicit ExprStart(const _Ty &value) : value(value) {}
            size_t GetRow() const { return value.Row(); }
            size_t GetCol() const { return value.Col(); }
            auto At(size_t index) const { return value[index]; }
            decltype(auto) operator()() const { return (value); }
        };

        /// @brief Binary Operator Template
        /// @tparam _BiFunc Binary Functor
        /// @tparam _LExpr Left Expression
        /// @tparam _RExpr Right Expression
        template <typename _BiFunc, typename _LExpr, typename _RExpr>
        class BinaryOperator : public Expr<BinaryOperator<_BiFunc, _LExpr, _RExpr>>
        {
            _BiFunc biFunc;
            _LExpr lExpr;
            _RExpr rExpr;

        public:
            using BaseType = typename Expr<BinaryOperator<_BiFunc, _LExpr, _RExpr>>;
            using BaseType::operator[];
            using BaseType::Col;
            using BaseType::Row;

            explicit BinaryOperator(const _BiFunc &func, const _LExpr &lExpr, const _RExpr &rExpr)
                : biFunc(func), lExpr(lExpr), rExpr(rExpr) {}
            auto At(size_t index) const { return biFunc(lExpr[index], rExpr[index]); }
            constexpr size_t GetRow() const { return Max(lExpr.Row(), rExpr.Row()); }
            constexpr size_t GetCol() const { return Max(lExpr.Col(), rExpr.Col()); }
            template <template <size_t, size_t, typename> class _T, size_t _Row, size_t _Col, typename _Ty>
            constexpr bool IsSizeMatch(const _T<_Row, _Col, _Ty> &other)
            {
                return Row() == other.Row() && Col() == other.Col();
            }
            template <typename _Ty>
            operator _Ty()
            {
                _Ty res(GetRow(), GetCol());
                for (size_t index = 0; index < res.Size(); ++index)
                    res[index] = (*this)[index];
                return res;
            }
            /// @brief Addoperator
            /// @tparam _Ty Template Expression
            /// @param rhs Expression
            template <expression _Ty>
            auto operator+(const _Ty &rhs)
            {
                return BinaryOperator<AddOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, _Ty>(addOpt, *this, rhs);
            }
            /// @brief AddOperator
            /// @tparam _Ty MatrixType
            /// @param rhs Matrix
            template <template <size_t, size_t, typename> class _T, size_t _Row, size_t _Col, typename _Ty>
            auto operator+(const _T<_Row, _Col, _Ty> &rhs)
            {
                if (!IsSizeMatch(rhs))
                    throw SizeExcept();
                using ExprT = ExprStart<_T<_Row, _Col, _Ty>>;
                return BinaryOperator<AddOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(addOpt, *this, ExprT(rhs));
            }
            /// @brief AddOperator
            /// @tparam _Ty Arithmetic Type
            /// @param rhs Number
            template <arithmetic _Ty>
            auto operator+(const _Ty &rhs)
            {
                using ExprT = ExprStart<ExprScalar<_Ty>>;
                return BinaryOperator<AddOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(addOpt, *this, ExprT(rhs));
            }
            /// @brief SubtractOperator
            /// @tparam _Ty Template Expression
            /// @param rhs Expression
            template <expression _Ty>
            auto operator-(const _Ty &rhs)
            {
                return BinaryOperator<SubtractOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, _Ty>(subOpt, *this, rhs);
            }
            /// @brief SubtractOperator
            /// @tparam _Ty MatrixType
            /// @param rhs Matrix
            template <template <size_t, size_t, typename> class _T, size_t _Row, size_t _Col, typename _Ty>
            auto operator-(const _T<_Row, _Col, _Ty> &rhs)
            {
                if (!IsSizeMatch(rhs))
                    throw SizeExcept();
                using ExprT = ExprStart<_T<_Row, _Col, _Ty>>;
                return BinaryOperator<SubtractOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(subOpt, *this, ExprT(rhs));
            }
            /// @brief SubtractOperator
            /// @tparam _Ty Arithmetic Type
            /// @param rhs Number
            template <arithmetic _Ty>
            auto operator-(const _Ty &rhs)
            {
                using ExprT = ExprStart<ExprScalar<_Ty>>;
                return BinaryOperator<SubtractOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(subOpt, *this, ExprT(rhs));
            }
            /// @brief MultipleOperator
            /// @tparam _Ty Arithmetic Type
            /// @param rhs Number
            template <arithmetic _Ty>
            auto operator*(const _Ty &rhs)
            {
                using ExprT = ExprStart<ExprScalar<_Ty>>;
                return BinaryOperator<MultipleOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(mulOpt, *this, ExprT(rhs));
            }
            template <arithmetic _Ty>
            auto operator/(const _Ty &rhs)
            {
                using ExprT = ExprStart<ExprSCalar<_Ty>>;
                return BinaryOperator<DivideOperatorType, BinaryOperator<_BiFunc, _LExpr, _RExpr>, ExprT>(divOpt, *this, ExprT(rhs));
            }
        };
    }
}
