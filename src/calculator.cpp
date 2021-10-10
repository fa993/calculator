#ifndef CALCULATOR_FA993
#define CALCULATOR_FA993

#include <iostream>
#include "calcfunctionregistry.cpp"

class Calculator
{
private:
    /* data */
public:
    Calculator() {}

    CalcEntity *parse(std::string &input)
    {
        CalcFunction *f = nullptr;
        int x = 0;
        return parseInternal(input, &x, f);
    }

private:
    CalcEntity *consumeEntityFromBuffer(std::string &buffer)
    {
        CalcEntity *ret = new CalcEntity(std::stod(buffer));
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
        }
    }

    CalcEntity *parseInternal(std::string &input, int *offset, CalcFunction *currentFunction)
    {
        CalcEntity *rootEntity;
        std::string buffer;
        CalcFunction *localVar;
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
                
            }
            else if (x == ')')
            {
                break;
            }
            else if (x == ',')
            {
                //pass recursive call inside
            }
            else
            {
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
                        ++(*offset);
                        localVar->pushArg(consumeEntityFromBuffer(buffer));
                        CalcEntity *rt = parseInternal(input, offset, localVar);
                        currentFunction->pushArg(rt);
                        rootEntity = currentFunction;
                        currentFunction = nullptr;
                    }
                    else if (f < 0)
                    {
                        --(*offset);
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