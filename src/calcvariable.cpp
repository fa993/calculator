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

    CalcEntity* simplify(std::map<std::string, CalcEntity*> &args)
    {
        std::map<std::string, CalcEntity*>::iterator it = args.find(variable);
        if (it != args.end())
        {
            //found
            CalcEntity* val = it->second;
            if(!val->isCompletelySimplified()) {
                CalcEntity* ret = val->simplify(args);
                args[variable] = ret;
                return ret;
            } else {
                return val;
            }
        }
        return new CalcVariable(variable);
    }

    bool isCompletelySimplified() {
        return false;
    }

    double _value()
    {
        throw "Unsupported Error";
    }

    std::string getName() {
        return variable;
    }

    std::string toString() {
        return variable;
    }
};

#endif