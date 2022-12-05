#include <iostream>
#include <chrono>
#include "include/Matrix.hpp"

using namespace std;
using namespace std::chrono;
using namespace LinerAlgebra;

int main()
{
    auto dm1 = DMatrix<>::Identity(3);
    auto dm2 = DMatrix<>::Identity(3);
    // DMatrix<> dm3 = dm1 + dm2;
    auto m1 = Matrix<3, 3>::Identity();
    auto m2 = Matrix<3, 3>::Identity();
    try
    {
        Matrix<3, 3> m3 = m1 - m1 + 5.6 * dm1 - 2 * dm1 * 2 + dm2 / 4;
        cout << format("{:12.4f}", m3);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}