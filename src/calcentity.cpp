#ifndef CALCENTITY_FA993
#define CALCENTITY_FA993

#include <map>
#include <string>

class CalcEntity
{
protected:
    /* data */

    bool complete;
    double result;

public:
    CalcEntity() {
        result = 0;
        complete = false;
    }

    CalcEntity(double num)
    {
        result = num;
        complete = true;
    }

    virtual void simplify(std::map<std::string, double> &args)
    {
    }

    bool isCompletelySimplified()
    {
        return complete;
    }

    double getValue()
    {
        return result;
    }
};

#endif