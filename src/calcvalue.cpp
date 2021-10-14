#ifndef CALCVALUE_FA993
#define CALCVALUE_FA993

#include "calcentity.cpp"

class CalcValue : public CalcEntity
{
private:
    /* data */
    double result;
public:
    CalcValue(double res) {
        result = res;
    }

    CalcEntity* simplify(std::map<std::string, CalcEntity*> &args) {
        return new CalcValue(result);
    }

    bool isCompletelySimplified() {
        return true;
    }

    double _value() {
        return result;
    }

    std::string toString() {
        return std::to_string(getValue());
    }
};


#endif