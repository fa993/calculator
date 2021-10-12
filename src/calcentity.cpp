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

private:
    CalcEntity(bool comp, double res) {
        complete = comp;
        result = res;
    }

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

    virtual void simplify(std::map<std::string, CalcEntity*> &args)
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

    virtual CalcEntity* clone() {
        return new CalcEntity(complete, result);
    }
};

#endif