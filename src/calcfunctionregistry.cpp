#ifndef CALCFUNCTIONREGISTRY_FA993
#define CALCFUNCTIONREGISTRY_FA993

#include "calcfunction.cpp"
#include <string>

class CalcProductBus : public CalcFunctionBus
{
private:
    /* data */
public:

    double getInitialValue() {
        return 1;
    }

    double aggregate(double output, double first) {
        return output * first;
    }

    CalcFunction* getNewInstance() {
        return new CalcProductBus();
    }

    std::string getConnecter() {
        return "*";
    }

};

class CalcMultiplicativeInverse : public CalcUnaryFunction 
{
private:
    /* data */
public:
    double operate(double first) {
        return 1 / first;
    }

    CalcFunction* getNewInstance() {
        return new CalcMultiplicativeInverse();
    }

    std::string toString() {
        return "(1 / " + arguments.at(0)->toString() + ")";
    }

};

class CalcSumBus : public CalcFunctionBus
{
private:
    /* data */
public:

    double getInitialValue() {
        return 0;
    }

    double aggregate(double output, double first) {
        return output + first;
    }

    CalcFunction* getNewInstance() {
        return new CalcSumBus();
    }

    std::string getConnecter() {
        return "+";
    }

};

class CalcAdditiveInverse : public CalcUnaryFunction
{
private:
    /* data */
public:

    double operate(double first) {
        return -first;
    }

    CalcFunction* getNewInstance() {
        return new CalcAdditiveInverse();
    }

    std::string toString() {
        return "(-"  + arguments.at(0)->toString() + ")";
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
            return new CalcSumBus();
        } else if(name == "multiply") {
            return new CalcProductBus();
        }
        return nullptr;
    }

};

#endif