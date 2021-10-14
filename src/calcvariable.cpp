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
        complete = false;
        variable = name;
    }

    virtual void simplify(std::map<std::string, CalcEntity*> &args)
    {
        std::map<std::string, CalcEntity*>::iterator it = args.find(variable);
        if (it != args.end())
        {
            //found
            complete = true;
            CalcEntity* val = it->second;
            if(!val->isCompletelySimplified()) {
                val->simplify(args);
                if(val->isCompletelySimplified()) {
                    result = val->getValue();
                    args[variable] = new CalcEntity(result);
                } else {
                    complete = false;
                }
            } else {
                result = val->getValue();
            }
        }
        else
        {
            //not found
            complete = false;
        }
    }

    std::string getName() {
        return variable;
    }

    CalcEntity* clone() {
        return new CalcVariable(variable);
    }

    std::string toString() {
        if(complete) {
            return std::to_string(result);
        } else {
            return variable;
        }
    }
};

#endif