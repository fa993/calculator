#ifndef CALCFUNCTIONREGISTRY_FA993
#define CALCFUNCTIONREGISTRY_FA993

#include "calcfunction.cpp"
#include <string>

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

class CalcFunctionRegistry
{
private:
    /* data */

public:

    CalcFunctionRegistry() {
        
    }

    CalcEntity* findFunction(std::string &name){
        if(name == "add") {
            return new CalcSumFunction();
        } else if(name == "subtract"){
            return new CalcDifferenceFunction();
        } else if(name == "multiply") {
            return new CalcProductFunction();
        } else if(name == "divide") {
            return new CalcDivisionFunction();
        }
        return nullptr;
    }

};

#endif