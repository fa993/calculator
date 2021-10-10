#ifndef CALCFUNCTIONREGISTRY_FA993
#define CALCFUNCTIONREGISTRY_FA993

#include "calcfunction.cpp"

class CalcSumFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:

    double operate(double first, double second)
    {
        return first + second;
    }

    int getPriority() {
        return 2;
    }
};

class CalcDifferenceFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:

    double operate(double first, double second)
    {
        return first - second;
    }

    int getPriority() {
        return 2;
    }
};

class CalcProductFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:

    double operate(double first, double second)
    {
        return first * second;
    }

    int getPriority() {
        return 1;
    }
};

class CalcDivisionFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:

    double operate(double first, double second)
    {
        return first / second;
    }

    int getPriority() {
        return 1;
    }
};

#endif