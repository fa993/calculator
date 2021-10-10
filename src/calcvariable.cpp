#ifndef CALCVARIABLE_FA993
#define CALCVARIABLE_FA993

#include "calcentity.cpp"
#include <map>
#include <string>

class CalcVariable : public CalcEntity
{
private:
    /* data */
    std::string variable;

public:
    CalcVariable(std::string name)
    {
        variable = name;
    }

    virtual void simplify(std::map<std::string, double> &args)
    {
        std::map<std::string, double>::iterator it = args.find(variable);
        if (it != args.end())
        {
            //found
            complete = true;
            result = it->second;
        }
        else
        {
            //not found
            complete = false;
        }
    }
};

#endif