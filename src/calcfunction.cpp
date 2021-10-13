#ifndef CALCFUNCTION_FA993
#define CALCFUNCTION_FA993

#include "calcentity.cpp"
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
        if(f1 == INFINITE_ARGUEMENTS || arguments.size() < f1) {
            arguments.push_back(arg);
        } else {
            throw "Too many Arguements Error";
        }
    }

    virtual int getArguementListLength() = 0;

    double getValue()
    {
        throw "Unsupported Error";
    }

    virtual ~CalcFunction() {
        
    }
};

class CalcStandardBinaryFunction : public CalcFunction
{

public:
    void simplify(std::map<std::string, CalcEntity*> &args)
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
    void simplify(std::map<std::string, CalcEntity*> &args)
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
};

class CalcFunctionBus : public CalcFunction
{
private:
    /* data */
public:

    void simplify(std::map<std::string, CalcEntity*> &args)
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
        return INFINITE_ARGUEMENTS;
    }

    CalcEntity* clone() {
        CalcFunction* sf = getNewInstance();
        for(std::vector<CalcEntity*>::const_iterator x = arguments.cbegin(); x != arguments.cend(); ++x) {
            sf->pushArg((*x)->clone());
        }
        return sf;
    }

    virtual double getInitialValue() = 0;

    virtual double aggregate(double output, double next) = 0;
    
    virtual CalcFunction* getNewInstance() = 0;

};

#endif