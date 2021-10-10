#ifndef CALCFUNCTION_FA993
#define CALCFUNCTION_FA993

#include "calcentity.cpp"
#include <map>
#include <string>
#include <vector>

class CalcFunction : public CalcEntity {

protected:

    std::vector<CalcEntity*> arguments;

public: 

    void pushArg(CalcEntity* arg) {
        arguments.push_back(arg);
    }

    virtual int getArguementListLength() = 0; 

    virtual int getPriority() = 0;

    double getValue() {
        throw "Unimplemented Error";
    }

};

class CalcStandardBinaryFunction : public CalcFunction
{

public:

    void simplify(std::map<std::string, double> &args)
    {
        if(arguments.size() != 2) {
            throw "Incorrect length of arguement list";
        }
        CalcEntity* arg1 = arguments.at(0);
        CalcEntity* arg2 = arguments.at(1);
        arg1->simplify(args);
        arg2->simplify(args);
        bool x = arg1->isCompletelySimplified();
        bool y = arg2->isCompletelySimplified();
        complete = false;
        double v1;
        double v2;
        if (x)
        {
            v1 = arg1->getValue();
            arg1 = new CalcEntity(v1);
        }
        if (y)
        {
            v2 = arg2->getValue();
            arg2 = new CalcEntity(v2);
        }
        if (x && y)
        {
            result = operate(v1, v2);
            complete = true;
            arguments.clear();
        }
    }

    int getArguementListLength() {
        return 2;
    }

    virtual double operate(double first, double second) = 0;

};

class CalcCustomBinaryFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:

    int getPriority() {
        return 0;
    }

};

#endif