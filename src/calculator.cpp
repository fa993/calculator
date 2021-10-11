#ifndef CALCULATOR_FA993
#define CALCULATOR_FA993

#include <iostream>
#include "calcfunctionregistry.cpp"
#include "calcfunction.cpp"
#include "calcvariable.cpp"

class Calculator
{
private:
    /* data */

    CalcFunctionRegistry *registry;
    std::map<std::string, CalcVariable *> vars;

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

    CalcEntity* parseV2(std::string &input) {
        CalcEntity *ans;
        int x = 0;
        CalcEntity *rt;
        while (x < input.size())
        {
            ans = parseInternalV2(input, &x, rt, nullptr, nullptr, nullptr);
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
                    ret = putIfAbsent(buffer[0], vars);
                    //xyz
                    //bit dirty but make multiplied element here
                    if (buffer.size() > 1)
                    {
                        CalcFunction *fn = new CalcProductBus();
                        fn->pushArg(ret);
                        for (int f1 = 1; f1 < buffer.size(); f1++)
                        {
                            fn->pushArg(putIfAbsent(buffer[f1], vars));
                        }
                        ret = fn;
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
            resolvePreviousBuffer(rootEntity, buffer);
        }
    }

    void resolvePreviousBufferV2(CalcEntity *&rootEntity, CalcFunction *&currentFunction, std::string &buffer)
    {
        if (currentFunction != nullptr)
        {
            //resolve previous buffer

            currentFunction->pushArg(consumeEntityFromBuffer(buffer));
            rootEntity = currentFunction;

            currentFunction = nullptr;
        }
        else
        {
            resolvePreviousBuffer(rootEntity, buffer);
        }
    }

    void resolvePreviousBuffer(CalcEntity *&rootEntity, std::string &buffer)
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

    void checkForInverse(CalcFunction *&wrapper, bool inverse, double proposedPriority)
    {
        if (inverse)
        {
            wrapper = registry->findInverse(proposedPriority);
        }
    }

    CalcEntity *parseInternalV2(std::string &input, int *offset, CalcEntity* rootEntity, bool inverse, CalcFunction *paritallyFilledBus, bool *fromBracket)
    {
        //new strategy... use function bus instead of reursive objects
        std::string buffer;
        double proposedPriority = -1;
        CalcFunction* wrapper = nullptr;
        if(paritallyFilledBus != nullptr){
            checkForInverse(wrapper, inverse, paritallyFilledBus->getPriority());
        }
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
                proposedPriority = 2;
                inverse = false;
            }
            else if (x == '-')
            {
                proposedPriority = 2;
                inverse = true;
            }
            else if (x == '*')
            {
                proposedPriority = 1;
                inverse = false;
            }
            else if (x == '/')
            {
                proposedPriority = 1;
                inverse = true;
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
                    resolvePreviousBufferV2(rootEntity, wrapper, buffer);
                    checkForInverse(wrapper, inverse, paritallyFilledBus->getPriority());
                    pre = static_cast<CalcFunction *>(rootEntity);
                    if (paritallyFilledBus != nullptr)
                    {   
                        paritallyFilledBus->pushArg(pre);
                    }
                    else
                    {
                        rootEntity = pre;
                    }
                }
                while (b)
                {
                    ++(*offset);
                    rt = parseInternalV2(input, offset, nullptr, nullptr, nullptr, &b);
                    if (x1)
                    {
                        pre->pushArg(rt);
                    }
                }
                if (!x1)
                {
                    if (paritallyFilledBus != nullptr)
                    {
                        paritallyFilledBus->pushArg(rt);
                        rootEntity = paritallyFilledBus;
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
                std::cout << x << std::endl;
                buffer.push_back(x);
                continue;
            }
            if (proposedPriority != -1)
            {
                if (paritallyFilledBus != nullptr)
                {
                    int f2 = proposedPriority - paritallyFilledBus->getPriority();
                    if (f2 == 0)
                    {
                        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
                        checkForInverse(wrapper, inverse, proposedPriority);
                        paritallyFilledBus->pushArg(rootEntity);
                    }
                    else if (f2 > 0)
                    {
                        (*offset)--;
                        break;
                    }
                    else if (f2 < 0)
                    {
                        //recurse
                        //3 + 2 * 4
                        //3 - 2 / 5
                        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
                        //checkForInverse(wrapper, inverse, proposedPriority);
                        CalcFunction *newBus = registry->findBus(proposedPriority);
                        newBus->pushArg(rootEntity);
                        (*offset)++;
                        rootEntity = parseInternalV2(input, offset, nullptr, inverse, newBus, fromBracket);
                        paritallyFilledBus->pushArg(rootEntity);
                    }
                }
                else
                {
                    paritallyFilledBus = registry->findBus(proposedPriority);
                    if(!buffer.empty() || rootEntity != nullptr){
                        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
                        paritallyFilledBus->pushArg(rootEntity);
                    }
                    checkForInverse(wrapper, inverse, proposedPriority);
                }
                proposedPriority = -1;
            }
        }
        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
        if(paritallyFilledBus != nullptr) {
            paritallyFilledBus->pushArg(rootEntity);
            rootEntity = paritallyFilledBus;
        }
        return rootEntity;
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
                    if (x1)
                    {
                        pre->pushArg(rt);
                    }
                }
                if (!x1)
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
            CalcEntity *clc = r->parseV2(x);
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