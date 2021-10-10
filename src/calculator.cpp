#ifndef CALCULATOR_FA993
#define CALCULATOR_FA993

#include <iostream>
#include "calcfunctionregistry.cpp"
#include "calcvariable.cpp"

class Calculator
{
private:
    /* data */

    CalcFunctionRegistry *registry;

public:
    Calculator()
    {
        registry = new CalcFunctionRegistry;
    }

    CalcEntity *parse(std::string &input)
    {
        CalcEntity *ans;
        int x = 0;
        CalcFunction *f = nullptr;
        CalcEntity *rt;
        while (x < input.size())
        {
            ans = parseInternal(input, &x, rt, f, nullptr);
            rt = ans;
        }
        return ans;
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
                ret = new CalcVariable(buffer);
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

    // void resolvePreviousBuffer(CalcEntity *&rootEntity, CalcFunction *&currentFunction, std::string &buffer)
    // {
    //     if (currentFunction != nullptr)
    //     { //resolve previous buffer
    //         if (currentFunction->canPushArgs())
    //         {
    //             currentFunction->pushArg(consumeEntityFromBuffer(buffer));
    //             rootEntity = currentFunction;
    //         }
    //         else
    //         {
    //             throw "Syntax Error";
    //         }
    //     }
    //     else
    //     {
    //         if (!buffer.empty())
    //         {
    //             rootEntity = consumeEntityFromBuffer(buffer);
    //         }
    //         else
    //         {
    //             throw "Syntax Error";
    //         }
    //     }
    // }

    void resolvePreviousBuffer(CalcEntity *&rootEntity, CalcFunction *&currentFunction, std::string &buffer)
    {
        if (currentFunction != nullptr)
        {
            //resolve previous buffer
            currentFunction->pushArg(consumeEntityFromBuffer(buffer));
            rootEntity = currentFunction;
        }
        else
        {
            if (!buffer.empty())
            {
                rootEntity = consumeEntityFromBuffer(buffer);
            }
            else if (rootEntity == nullptr)
            {
                throw "Syntax Error";
            }
        }
    }

    CalcEntity *parseInternal(std::string &input, int *offset, CalcEntity *rootEntity, CalcFunction *currentFunction, bool *fromBracket)
    {
        std::string buffer;
        CalcFunction *localVar = nullptr;
        for (; (*offset) < input.size(); (*offset)++)
        // for (std::string::const_iterator x = input.cbegin() + (*offset); x != input.end(); ++x, ++(*offset))
        {
            char x = input[*offset];
            //new strategy... recurse on precedence change;
            //3 + 2 * 7
            //3 + 2 * 7 - 6
            if (x == ' ')
            {
                //ignore
                continue;
            }
            else if (x == '+')
            {
                localVar = new CalcSumFunction();
            }
            else if (x == '-')
            {
                localVar = new CalcDifferenceFunction();
            }
            else if (x == '*')
            {
                localVar = new CalcProductFunction();
            }
            else if (x == '/')
            {
                localVar = new CalcDivisionFunction();
            }
            else if (x == '(')
            {
                //pass recursive call inside
                bool b = true;
                CalcEntity *rt;
                CalcFunction *pre;
                bool x1 = !buffer.empty();
                if (x1)
                {
                    pre = static_cast<CalcFunction *>(consumeEntityFromBuffer(buffer));
                    if (currentFunction != nullptr)
                    {
                        currentFunction->pushArg(pre);
                        rootEntity = currentFunction;
                        currentFunction = nullptr;
                    }
                    else
                    {
                        rootEntity = pre;
                    }
                }
                while (b)
                {
                    ++(*offset);
                    rt = parseInternal(input, offset, nullptr, nullptr, &b);
                    if(x1) {
                        pre->pushArg(rt);
                    }
                }
                if(!x1)
                {
                    if (currentFunction != nullptr)
                    {
                        currentFunction->pushArg(rt);
                        rootEntity = currentFunction;
                        currentFunction = nullptr;
                    }
                    else
                    {
                        rootEntity = rt;
                    }
                }
            }
            else if (x == ')')
            {
                (*fromBracket) = false;
                break;
            }
            else if (x == ',')
            {
                //add(4, 5)
                //pass recursive call inside
                break;
            }
            else
            {
                // std::cout << x << std::endl;
                buffer.push_back(x);
                continue;
            }
            if (localVar != nullptr)
            {
                if (currentFunction == nullptr)
                {
                    resolvePreviousBuffer(rootEntity, currentFunction, buffer);
                    currentFunction = localVar;
                    currentFunction->pushArg(rootEntity);
                }
                else
                {
                    int f = currentFunction->getPriority() - localVar->getPriority();
                    if (f == 0)
                    {
                        resolvePreviousBuffer(rootEntity, currentFunction, buffer);
                        currentFunction = localVar;
                        currentFunction->pushArg(rootEntity);
                    }
                    else if (f > 0)
                    {
                        //recurse
                        // parse()
                        //TODO
                        localVar->pushArg(consumeEntityFromBuffer(buffer));
                        ++(*offset);
                        CalcEntity *rt = parseInternal(input, offset, nullptr, localVar, fromBracket);
                        currentFunction->pushArg(rt);
                        rootEntity = currentFunction;
                        currentFunction = nullptr;
                    }
                    else if (f < 0)
                    {
                        (*offset)--;
                        break;
                    }
                }
                localVar = nullptr;
            }
        }
        resolvePreviousBuffer(rootEntity, currentFunction, buffer);
        return rootEntity;
    }
};

int main(int argc, char const *argv[])
{
    Calculator *r = new Calculator();
    std::string x;
    std::map<std::string, double> args;
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
            CalcEntity *clc = r->parse(x);
            clc->simplify(args);
            std::cout << clc->getValue() << std::endl;
        }
        catch (const char *msg)
        {
            std::cout << msg << std::endl;
        }
    }
    return 0;
}

#endif

// if (!buffer.empty())
// {
//     if (currentFunction != nullptr)
//     {
//         if (currentFunction->canPushArgs())
//         {
//             currentFunction->pushArg(consumeEntityFromBuffer(buffer));
//             if (!currentFunction->canPushArgs())
//             {
//                 rootEntity = currentFunction;
//                 currentFunction = nullptr;
//             }
//         }
//     }
//     else
//     {
//         rootEntity = consumeEntityFromBuffer(buffer);
//     }
// }