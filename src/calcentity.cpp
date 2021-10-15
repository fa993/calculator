#ifndef CALCENTITY_FA993
#define CALCENTITY_FA993

#include <map>
#include <string>

class CalcEntity
{

public:

    virtual CalcEntity* simplify(std::map<std::string, CalcEntity*> &args) = 0;

    virtual bool isCompletelySimplified() = 0;

    virtual double _value() = 0;

    virtual std::string toString() = 0;

    double getValue() {
        if(isCompletelySimplified()) {
            return _value();
        } else {
            throw "Unsimplified Error";
        }
    }

    virtual ~CalcEntity() {
        //TODO
    }

};

#endif