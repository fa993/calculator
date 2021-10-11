#ifndef CALCFUNCTION_FA993
#define CALCFUNCTION_FA993

#include "calcentity.cpp"
#include <map>
#include <string>
#include <vector>

class CalcFunction : public CalcEntity
{

protected:
    std::vector<CalcEntity *> arguments;

public:
    void pushArg(CalcEntity *arg)
    {
        arguments.push_back(arg);
    }

    virtual int getArguementListLength() = 0;

    virtual double getPriority() = 0;

    double getValue()
    {
        throw "Unimplemented Error";
    }

    virtual ~CalcFunction() {
        
    }
};

class CalcStandardBinaryFunction : public CalcFunction
{

public:
    void simplify(std::map<std::string, double> &args)
    {
        if (arguments.size() != getArguementListLength())
        {
            throw "Incorrect length of arguement list";
        }
        CalcEntity *arg1 = arguments.at(0);
        CalcEntity *arg2 = arguments.at(1);
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

    int getArguementListLength()
    {
        return 2;
    }

    virtual double operate(double first, double second) = 0;
};

class CalcUnaryFunction : public CalcFunction
{
private:
    /* data */
public:
    void simplify(std::map<std::string, double> &args)
    {
        if (arguments.size() != getArguementListLength())
        {
            throw "Incorrect length of arguement list";
        }
        CalcEntity *x1 = arguments[0];
        x1->simplify(args);
        if (x1->isCompletelySimplified())
        {
            double v1 = x1->getValue();
            result = operate(v1);
            complete = true;
            arguments.clear();
        }
        else
        {
            complete = false;
        }
    }

    int getArguementListLength()
    {
        return 1;
    }

    virtual double operate(double first) = 0;
};

class CalcCustomBinaryFunction : public CalcStandardBinaryFunction
{
private:
    /* data */
public:
    double getPriority()
    {
        return 0;
    }
};

class CalcFunctionBus : public CalcFunction
{
private:
    /* data */
public:

    void simplify(std::map<std::string, double> &args)
    {
        bool all = true;
        double aggregateValue = getInitialValue();
        std::vector<CalcEntity *> parsedArgs;
        for (int i = 0; i < arguments.size(); i++)
        {
            CalcEntity *x1 = arguments[i];
            x1->simplify(args);
            if (x1->isCompletelySimplified())
            {
                double v1 = x1->getValue();
                arguments[i] = new CalcEntity(v1);
                aggregateValue = aggregate(aggregateValue, v1);
            }
            else
            {
                all = false;
                parsedArgs.push_back(x1);
            }
        }
        arguments.clear();
        if (all)
        {
            complete = true;
            result = aggregateValue;
        }
        else
        {
            complete = false;
            parsedArgs.push_back(new CalcEntity(aggregateValue));
            arguments = parsedArgs;
        }
    }

    int getArguementListLength()
    {
        throw "Unsupported Error";
    }

    virtual double getInitialValue() = 0;

    virtual double aggregate(double output, double next) = 0;
};

#endif