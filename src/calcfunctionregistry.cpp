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

    double getPriority() {
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

    double getPriority() {
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

    double getPriority() {
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

    double getPriority() {
        return 1;
    }
};

class CalcProductBus : public CalcFunctionBus
{
private:
    /* data */
public:

    CalcProductBus();

    double getInitialValue() {
        return 1;
    }

    double operate(double first, double second) {
        return first * second;
    }

    double getPriority(){
        return 1;
    }

};

class CalcMultiplicativeInverse : public CalcUnaryFunction 
{
private:
    /* data */
public:
    CalcMultiplicativeInverse();

    double operate(double first) {
        return 1 / first;
    }

    double getPriority(){
        return 1;
    }
};

class CalcSumBus : public CalcFunctionBus
{
private:
    /* data */
public:

    CalcSumBus();

    double getInitialValue() {
        return 0;
    }

    double operate(double first, double second) {
        return first + second;
    }

    double getPriority(){
        return 2;
    }

};

class CalcAdditiveInverse : public CalcUnaryFunction
{
private:
    /* data */
public:
    CalcAdditiveInverse();

    double operate(double first) {
        return -first;
    }

    double getPriority(){
        return 2;
    }
};

class CalcFunctionRegistry
{
private:
    /* data */

public:

    CalcFunctionRegistry() {
        
    }

    CalcFunction* findFunction(std::string &name){
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

    CalcFunction* findBus(double priority) {
        if(priority == 2){
            return new CalcSumBus();
        } else if(priority == 1) {
            return new CalcProductBus();
        }
        return nullptr;
    }

    CalcFunction* findInverse(double priority) {
        if(priority == 2){
            return new CalcAdditiveInverse();
        } else if(priority == 1) {
            return new CalcMultiplicativeInverse();
        }
        return nullptr;
    }

};

#endif