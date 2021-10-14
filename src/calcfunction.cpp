#ifndef CALCFUNCTION_FA993
#define CALCFUNCTION_FA993

#include "calcentity.cpp"
#include "calcvalue.cpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

static const int INFINITE_ARGUEMENTS = -1;

class CalcFunction : public CalcEntity
{

protected:
    std::vector<CalcEntity *> arguments;

public:
    void pushArg(CalcEntity *arg)
    {
        int f1 = getArguementListLength();
        if (f1 == INFINITE_ARGUEMENTS || arguments.size() < f1)
        {
            arguments.push_back(arg);
        }
        else
        {
            throw "Too many Arguements Error";
        }
    }

    virtual int getArguementListLength() = 0;

    bool isCompletelySimplified()
    {
        return false;
    }

    double _value()
    {
        throw "Unsupported Error";
    }

    virtual ~CalcFunction()
    {
        //TODO
    }
};

// class CalcStandardBinaryFunction : public CalcFunction
// {

// public:
//     void simplify(std::map<std::string, CalcEntity *> &args)
//     {
//         if (arguments.size() != getArguementListLength())
//         {
//             throw "Incorrect length of arguement list";
//         }
//         CalcEntity *arg1 = arguments.at(0);
//         CalcEntity *arg2 = arguments.at(1);
//         arg1->simplify(args);
//         arg2->simplify(args);
//         bool x = arg1->isCompletelySimplified();
//         bool y = arg2->isCompletelySimplified();
//         complete = false;
//         double v1;
//         double v2;
//         if (x)
//         {
//             v1 = arg1->getValue();
//             arg1 = new CalcEntity(v1);
//         }
//         if (y)
//         {
//             v2 = arg2->getValue();
//             arg2 = new CalcEntity(v2);
//         }
//         if (x && y)
//         {
//             result = operate(v1, v2);
//             complete = true;
//             arguments.clear();
//         }
//     }

//     int getArguementListLength()
//     {
//         return 2;
//     }

//     virtual double operate(double first, double second) = 0;
// };

class CalcUnaryFunction : public CalcFunction
{
private:
    /* data */
public:
    CalcEntity *simplify(std::map<std::string, CalcEntity *> &args)
    {
        if (arguments.size() != getArguementListLength())
        {
            throw "Incorrect length of arguement list";
        }
        CalcEntity *x2 = arguments[0]->simplify(args);
        if (x2->isCompletelySimplified())
        {
            return new CalcValue(operate(x2->getValue()));
        } else {
            CalcFunction* ret = getNewInstance();
            ret->pushArg(x2);
            return ret;
        }
    }

    virtual CalcFunction *getNewInstance() = 0;


    int getArguementListLength()
    {
        return 1;
    }

    virtual double operate(double first) = 0;
};

// class CalcCustomBinaryFunction : public CalcStandardBinaryFunction
// {
// private:
//     /* data */
// public:
// };

class CalcFunctionBus : public CalcFunction
{
private:
    /* data */
public:
    CalcEntity *simplify(std::map<std::string, CalcEntity *> &args)
    {
        bool all = true;
        double aggregateValue = getInitialValue();
        CalcFunction *ret = getNewInstance();
        for (int i = 0; i < arguments.size(); i++)
        {
            CalcEntity *x2 = arguments[i]->simplify(args);
            if (x2->isCompletelySimplified())
            {
                aggregateValue = aggregate(aggregateValue, x2->getValue());
            }
            else
            {
                all = false;
                ret->pushArg(x2);
            }
        }
        if (all)
        {
            delete ret;
            return new CalcValue(aggregateValue);
        }
        else
        {
            if (aggregateValue != getInitialValue())
            {
                ret->pushArg(new CalcValue(aggregateValue));
            }
            return ret;
        }
    }

    int getArguementListLength()
    {
        return INFINITE_ARGUEMENTS;
    }

    virtual double getInitialValue() = 0;

    virtual double aggregate(double output, double next) = 0;

    virtual CalcFunction *getNewInstance() = 0;

    virtual std::string getConnecter() = 0;

    std::string toString()
    {
        std::string ret;
        if (arguments.size() > 0)
        {
            ret.append(arguments.at(0)->toString());
        }
        for (std::vector<CalcEntity *>::const_iterator x = arguments.cbegin() + 1; x != arguments.cend(); ++x)
        {
            ret.append(" " + getConnecter() + " " + (*x)->toString());
        }
        return ret;
    }
}
;

#endif