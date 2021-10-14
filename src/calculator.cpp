#ifndef CALCULATOR_FA993
#define CALCULATOR_FA993

#include <iostream>
#include "calcfunctionregistry.cpp"
#include "calcfunction.cpp"
#include "calcvariable.cpp"

enum ParseMode
{
    MODE_SUM,
    MODE_PRODUCT,
    MODE_BRACKET
};

class Calculator
{
private:
    /* data */

    CalcFunctionRegistry *registry;
    std::map<std::string, CalcVariable *> vars;

public:

    std::map<std::string, CalcEntity*> args;

    Calculator()
    {
        registry = new CalcFunctionRegistry;
    }

    CalcEntity *parseExpV2(std::string &input)
    {
        int x = 0;
        return parseBus(input, &x, nullptr, nullptr, MODE_BRACKET);
    }

private:
    static bool isAlpha(unsigned char x)
    {
        return std::isalpha(x);
    }

    static bool isSpace(unsigned char x)
    {
        return std::isspace(x);
    }

    CalcVariable *putIfAbsent(char buffer, std::map<std::string, CalcVariable *> map)
    {
        std::string x1 = std::string(1, buffer);
        return putIfAbsent(x1, map);
    }

    CalcVariable *putIfAbsent(std::string &buffer, std::map<std::string, CalcVariable *> map)
    {
        std::map<std::string, CalcVariable *>::iterator it = vars.find(buffer);
        CalcVariable *ret;
        if (it != vars.cend())
        {
            ret = it->second;
        }
        else
        {
            ret = new CalcVariable(buffer);
            vars[buffer] = ret;
        }
        return ret;
    }

    CalcEntity *consumeEntityFromBuffer(std::string &buffer)
    {
        bool b1 = std::find_if(buffer.cbegin(), buffer.cend(), Calculator::isSpace) != buffer.cend();
        if (b1)
        {
            throw "Stray Whitespace";
        }
        bool b2 = std::find_if(buffer.cbegin(), buffer.cend(), Calculator::isAlpha) != buffer.cend();
        CalcEntity *ret;
        if (b2)
        {
            CalcEntity *r1 = registry->findFunction(buffer);
            if (r1 == nullptr)
            {
                std::map<std::string, CalcVariable *>::iterator it = vars.find(buffer);
                if (it != vars.cend())
                {
                    ret = it->second;
                }
                else
                {
                    if(isAlpha(buffer[0])) {
                        ret = putIfAbsent(buffer[0], vars);
                    } else  {
                        ret = new CalcEntity(buffer[0] - '0');
                    }
                    //xyz
                    //bit dirty but make multiplied element here
                    if (buffer.size() > 1)
                    {
                        CalcFunction *fn = new CalcProductBus();
                        fn->pushArg(ret);
                        bool isCompound = true;
                        for (int f1 = 1; f1 < buffer.size(); f1++)
                        {
                            if(isAlpha(buffer[f1])) {
                                fn->pushArg(putIfAbsent(buffer[f1], vars));
                            } else {
                                isCompound = false;
                                break;
                            }
                            // std::cout<< buffer[f1] << std::endl;
                        }
                        if(isCompound) {
                            ret = fn;
                        } else  {
                            delete fn;
                            ret = putIfAbsent(buffer, vars);
                        }
                    }
                }
            }
            else
            {
                ret = r1;
            }
        }
        else
        {
            ret = new CalcEntity(std::stod(buffer));
        }
        buffer.clear();
        return ret;
    }

    void pushToBus(bool invFlag, CalcFunction *addBus, CalcEntity *entity, CalcFunction *inverse)
    {
        if (invFlag)
        {
            inverse->pushArg(entity);
            addBus->pushArg(inverse);
        }
        else
        {
            addBus->pushArg(entity);
            delete inverse;
        }
    }

    CalcEntity *parseBus(std::string &input, int *offset, CalcEntity *firstArg, bool *fromBracket, ParseMode mode)
    {
        CalcFunction *bus = nullptr;
        CalcEntity *lastEntity = nullptr;
        if (mode == MODE_SUM)
        {
            bus = new CalcSumBus();
        }
        else if (mode == MODE_PRODUCT)
        {
            bus = new CalcProductBus();
        }
        else if (mode == MODE_BRACKET)
        {
            //DO NOTHING
        }
        if (firstArg != nullptr)
        {
            bus->pushArg(firstArg);
        }
        std::string buffer;
        bool invFlag = false;
        for (; (*offset) < input.size(); (*offset)++)
        // for (std::string::const_iterator x = input.cbegin() + (*offset); x != input.end(); ++x, ++(*offset))
        {
            char x = input[*offset];
            if (x == ' ')
            {
                //ignore
                continue;
            }
            else if (x == '+')
            {
                //this is us
                if (mode == MODE_SUM)
                {
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
                    }
                    else
                    {
                        //ignore
                    }
                    invFlag = false;
                }
                else if (mode == MODE_PRODUCT)
                {
                    (*offset)--;
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
                    }
                    break;
                }
                else if (mode == MODE_BRACKET)
                {
                    if (!buffer.empty())
                    {
                        return parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_SUM);
                    }
                    else if (lastEntity != nullptr)
                    {
                        return parseBus(input, offset, lastEntity, fromBracket, MODE_SUM);
                    }
                    else
                    {
                        //act as prefix to number
                        buffer.push_back(x);
                    }
                }
            }
            else if (x == '-')
            {
                if (mode == MODE_SUM)
                {
                    //us but with inverse
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
                    }
                    else
                    {
                        //ignore
                    }
                    invFlag = true;
                }
                else if (mode == MODE_PRODUCT)
                {
                    (*offset)--;
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
                    }
                    break;
                }
                else if (mode == MODE_BRACKET)
                {
                    if (!buffer.empty())
                    {
                        return parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_SUM);
                    }
                    else if (lastEntity != nullptr)
                    {
                        return parseBus(input, offset, lastEntity, fromBracket, MODE_SUM);
                    }
                    else
                    {
                        //act as prefix to number
                        return parseBus(input, offset, nullptr, fromBracket, MODE_SUM);
                    }
                }
            }
            else if (x == '*')
            {
                if (mode == MODE_SUM)
                {
                    (*offset)++;
                    bus->pushArg(parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_PRODUCT));
                }
                else if (mode == MODE_PRODUCT)
                {
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
                    }
                    else
                    {
                        //ignore
                    }
                    invFlag = false;
                }
                else if (mode == MODE_BRACKET)
                {
                    if (!buffer.empty())
                    {
                        CalcEntity *mu = parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_PRODUCT);
                        return parseBus(input, offset, mu, fromBracket, MODE_SUM);
                    }
                    else if (lastEntity != nullptr)
                    {
                        CalcEntity *mu = parseBus(input, offset, lastEntity, fromBracket, MODE_PRODUCT);
                        return parseBus(input, offset, mu, fromBracket, MODE_SUM);
                    }
                    else
                    {
                        throw "Syntax Error";
                    }
                }
            }
            else if (x == '/')
            {
                if (mode == MODE_SUM)
                {
                    //but with twist
                    (*offset)++;
                    CalcFunction *f1 = new CalcMultiplicativeInverse();
                    f1->pushArg(consumeEntityFromBuffer(buffer));
                    bus->pushArg(parseBus(input, offset, f1, fromBracket, MODE_PRODUCT));
                }
                else if (mode == MODE_PRODUCT)
                {
                    //but with twist
                    if (!buffer.empty())
                    {
                        pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
                    }
                    else
                    {
                        //ignore
                    }
                    invFlag = true;
                }
                else if (mode == MODE_BRACKET)
                {
                    if (!buffer.empty())
                    {
                        CalcEntity *mu = parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_PRODUCT);
                        return parseBus(input, offset, mu, fromBracket, MODE_SUM);
                    }
                    else if (lastEntity != nullptr)
                    {
                        CalcEntity *mu = parseBus(input, offset, lastEntity, fromBracket, MODE_PRODUCT);
                        return parseBus(input, offset, mu, fromBracket, MODE_SUM);
                    }
                    else
                    {
                        throw "Syntax Error";
                    }
                }
            }
            else if (x == '(')
            {
                //pass recursive call inside
                (*offset)++;
                CalcFunction *pre = nullptr;
                bool b = true;
                if (!buffer.empty())
                {
                    pre = static_cast<CalcFunction *>(consumeEntityFromBuffer(buffer));
                }
                CalcEntity *rt = nullptr;
                do
                {
                    rt = parseBus(input, offset, nullptr, &b, MODE_BRACKET);
                    if (pre != nullptr)
                    {
                        pre->pushArg(rt);
                    }
                } while (!b);
                if (pre != nullptr)
                {
                    rt = pre;
                }
                if (mode == MODE_SUM)
                {
                    pushToBus(invFlag, bus, rt, new CalcAdditiveInverse());
                }
                else if (mode == MODE_PRODUCT)
                {
                    pushToBus(invFlag, bus, rt, new CalcMultiplicativeInverse());
                }
                else if (mode == MODE_BRACKET)
                {
                    lastEntity = rt;
                }
            }
            else if (x == ')')
            {
                (*fromBracket) = true;
                break;
            }
            else if (x == ',')
            {
                (*offset)++;
                (*fromBracket) = false;
                break;
            }
            else if(x == '=') {
                CalcVariable* x1 = static_cast<CalcVariable*>(consumeEntityFromBuffer(buffer));
                (*offset)++;
                CalcEntity* nextOne = parseBus(input, offset, nullptr, fromBracket, MODE_BRACKET);
                args[x1->getName()] =  nextOne;
                if(mode == MODE_SUM) {
                    pushToBus(invFlag, bus, x1, new CalcAdditiveInverse());
                } else if(mode == MODE_PRODUCT) {
                    pushToBus(invFlag, bus, x1, new CalcMultiplicativeInverse());
                } else if(mode == MODE_BRACKET) {
                    lastEntity = x1;
                }
            }
            else
            {
                buffer.push_back(x);
                // std::cout << buffer << std::endl;
                continue;
            }
        }
        if (mode == MODE_SUM)
        {
            if (!buffer.empty())
            {
                pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
            }
            return bus;
        }
        else if (mode == MODE_PRODUCT)
        {
            if (!buffer.empty())
            {
                pushToBus(invFlag, bus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
            }
            return bus;
        }
        else if (mode == MODE_BRACKET)
        {
            if (!buffer.empty())
            {
                return consumeEntityFromBuffer(buffer);
            }
            else if (lastEntity != nullptr)
            {
                return lastEntity;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
};

std::map<std::string, CalcEntity*> cloneMap(std::map<std::string, CalcEntity*> &map) {
    std::map<std::string, CalcEntity*> duplicate;
    for(std::map<std::string, CalcEntity*>::const_iterator f = map.cbegin(); f != map.cend(); f++) {
        duplicate.insert(std::make_pair<std::string, CalcEntity*>(f->first, f->second->clone()));
    }
    return duplicate;
}

int main(int argc, char const *argv[])
{
    Calculator *r = new Calculator();
    std::string x;
    std::map<std::string, CalcEntity*> noArgs;
    std::map<std::string, CalcEntity*> dup;
    while (true)
    {
        try
        {
            std::cout << ">>";
            getline(std::cin, x);
            if (x == "bye")
            {
                std::cout << "bye!" << std::endl;
                break;
            }
            char last = x.back();
            bool ls = last == ';';
            if (ls)
            {
                x.pop_back();
            }
            CalcEntity *clc = r->parseExpV2(x);
            if (clc != nullptr)
            {
                clc->simplify(noArgs);
                if(!clc->isCompletelySimplified()) {
                    clc = clc->clone();
                    dup = cloneMap(r->args);
                    clc->simplify(dup);
                }
                if (!ls)
                {
                    std::cout << clc->toString() << std::endl;
                }
                else
                {
                    std::cout << "Ok" << std::endl;
                }
                // delete clc;
                //figure out a way to delete map                
            }
        }
        catch (const char *msg)
        {
            std::cout << msg << std::endl;
        }
    }
    return 0;
}

#endif
