#ifndef CALCULATOR_FA993
#define CALCULATOR_FA993

#include <iostream>
#include "calcfunctionregistry.cpp"
#include "calcfunction.cpp"
#include "calcvariable.cpp"

enum MatchResult
{
    OPERATION_CONTINUE,
    OPERATION_BREAK,
    OPERATION_NOT_MATCH
};

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
    std::map<CalcVariable *, CalcEntity *> values;

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

    CalcEntity *parseV2(std::string &input)
    {
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

    // CalcEntity *parseExp(std::string &input)
    // {
    //     int x = 0;
    //     return parseBracketBus(input, &x, nullptr);
    // }

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
            if (!buffer.empty())
            {
                currentFunction->pushArg(consumeEntityFromBuffer(buffer));
                rootEntity = currentFunction;
            }
            else
            {
                rootEntity = currentFunction;
            }
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

    MatchResult defaultOperations(char x, bool *fromBracket, std::string &input, std::string &buffer, int *offset)
    {
        if (x == ' ')
        {
            //ignore
            return OPERATION_CONTINUE;
        }
        else if (x == ')')
        {
            (*fromBracket) = true;
            return OPERATION_BREAK;
        }
        else if (x == ',')
        {
            (*offset)++;
            (*fromBracket) = false;
            return OPERATION_BREAK;
        }
        // else if(x == '=') {

        //     a = consumeEntityFromBuffer(buffer);
        //     values[] = parseBracketBus(input, offset, fromBracket);
        //     return OPERATION_CONTINUE;
        // }
        else if (x == '+' || x == '-' || x == '/' || x == '*')
        {
            return OPERATION_NOT_MATCH;
        }
        else
        {
            // std::cout << x << std::endl;
            buffer.push_back(x);
            return OPERATION_CONTINUE;
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
                        return parseBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket, MODE_SUM);
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
                        buffer.push_back(x);
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
            // else if(x == '=') {

            //     a = consumeEntityFromBuffer(buffer);
            //     values[] = parseBracketBus(input, offset, fromBracket);
            //     return OPERATION_CONTINUE;
            // }
            else
            {
                buffer.push_back(x);
                std::cout << buffer << std::endl;
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

    // CalcEntity *parseAddBus(std::string &input, int *offset, CalcEntity *firstArg, bool *fromBracket)
    // {
    //     CalcFunction *addBus = new CalcSumBus();
    //     if (firstArg != nullptr)
    //     {
    //         addBus->pushArg(firstArg);
    //     }
    //     std::string buffer;
    //     bool invFlag = false;
    //     for (; (*offset) < input.size(); (*offset)++)
    //     // for (std::string::const_iterator x = input.cbegin() + (*offset); x != input.end(); ++x, ++(*offset))
    //     {
    //         char x = input[*offset];
    //         MatchResult rs = defaultOperations(x, fromBracket, input, buffer, offset);
    //         if (rs == OPERATION_BREAK)
    //         {
    //             break;
    //         }
    //         else if (rs == OPERATION_CONTINUE)
    //         {
    //             continue;
    //         }
    //         if (x == '+')
    //         {
    //             //this is us
    //             if (!buffer.empty())
    //             {
    //                 pushToBus(invFlag, addBus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
    //             }
    //             else
    //             {
    //                 //ignore
    //             }
    //             invFlag = false;
    //         }
    //         else if (x == '-')
    //         {
    //             //us but with inverse
    //             if (!buffer.empty())
    //             {
    //                 pushToBus(invFlag, addBus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
    //             }
    //             else
    //             {
    //                 //ignore
    //             }
    //             invFlag = true;
    //         }
    //         else if (x == '*')
    //         {
    //             (*offset)++;
    //             addBus->pushArg(parseMultiplyBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket));
    //         }
    //         else if (x == '/')
    //         {
    //             //but with twist
    //             (*offset)++;
    //             CalcFunction *f1 = new CalcMultiplicativeInverse();
    //             f1->pushArg(consumeEntityFromBuffer(buffer));
    //             addBus->pushArg(parseMultiplyBus(input, offset, f1, fromBracket));
    //         }
    //         else if (x == '(')
    //         {
    //             //pass recursive call inside
    //             (*offset)++;
    //             CalcFunction *pre = nullptr;
    //             bool b = true;
    //             if (!buffer.empty())
    //             {
    //                 pre = static_cast<CalcFunction *>(consumeEntityFromBuffer(buffer));
    //             }
    //             CalcEntity *rt = nullptr;
    //             do
    //             {
    //                 rt = parseBracketBus(input, offset, &b);
    //                 if (pre != nullptr)
    //                 {
    //                     pre->pushArg(rt);
    //                 }
    //             } while (!b);
    //             if (pre != nullptr)
    //             {
    //                 rt = pre;
    //             }
    //             pushToBus(invFlag, addBus, rt, new CalcAdditiveInverse());
    //         }
    //     }
    //     if (!buffer.empty())
    //     {
    //         pushToBus(invFlag, addBus, consumeEntityFromBuffer(buffer), new CalcAdditiveInverse());
    //     }
    //     return addBus;
    // }

    // CalcEntity *parseMultiplyBus(std::string &input, int *offset, CalcEntity *firstArg, bool *fromBracket)
    // {
    //     CalcFunction *multiplyBus = new CalcProductBus();
    //     if (firstArg != nullptr)
    //     {
    //         multiplyBus->pushArg(firstArg);
    //     }
    //     std::string buffer;
    //     bool invFlag = false;
    //     for (; (*offset) < input.size(); (*offset)++)
    //     // for (std::string::const_iterator x = input.cbegin() + (*offset); x != input.end(); ++x, ++(*offset))
    //     {
    //         char x = input[*offset];
    //         MatchResult rs = defaultOperations(x, fromBracket, input, buffer, offset);
    //         if (rs == OPERATION_BREAK)
    //         {
    //             break;
    //         }
    //         else if (rs == OPERATION_CONTINUE)
    //         {
    //             continue;
    //         }
    //         else if (x == '+' || x == '-')
    //         {
    //             (*offset)--;
    //             if (!buffer.empty())
    //             {
    //                 pushToBus(invFlag, multiplyBus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
    //             }
    //             break;
    //         }
    //         else if (x == '*')
    //         {
    //             if (!buffer.empty())
    //             {
    //                 pushToBus(invFlag, multiplyBus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
    //             }
    //             else
    //             {
    //                 //ignore
    //             }
    //             invFlag = false;
    //         }
    //         else if (x == '/')
    //         {
    //             //but with twist
    //             if (!buffer.empty())
    //             {
    //                 pushToBus(invFlag, multiplyBus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
    //             }
    //             else
    //             {
    //                 //ignore
    //             }
    //             invFlag = true;
    //         }
    //         else if (x == '(')
    //         {
    //             //pass recursive call inside
    //             (*offset)++;
    //             CalcFunction *pre = nullptr;
    //             bool b = true;
    //             if (!buffer.empty())
    //             {
    //                 pre = static_cast<CalcFunction *>(consumeEntityFromBuffer(buffer));
    //             }
    //             CalcEntity *rt = nullptr;
    //             do
    //             {
    //                 rt = parseBracketBus(input, offset, &b);
    //                 if (pre != nullptr)
    //                 {
    //                     pre->pushArg(rt);
    //                 }
    //             } while (!b);
    //             if (pre != nullptr)
    //             {
    //                 rt = pre;
    //             }
    //             pushToBus(invFlag, multiplyBus, rt, new CalcMultiplicativeInverse());
    //         }
    //     }
    //     if (!buffer.empty())
    //     {
    //         pushToBus(invFlag, multiplyBus, consumeEntityFromBuffer(buffer), new CalcMultiplicativeInverse());
    //     }
    //     return multiplyBus;
    // }

    // //3 + 4 * 9
    // //5 * 9
    // CalcEntity *parseBracketBus(std::string &input, int *offset, bool *fromBracket)
    // {
    //     std::string buffer;
    //     bool invFlag = false;
    //     for (; (*offset) < input.size(); (*offset)++)
    //     // for (std::string::const_iterator x = input.cbegin() + (*offset); x != input.end(); ++x, ++(*offset))
    //     {
    //         char x = input[*offset];
    //         MatchResult rs = defaultOperations(x, fromBracket, input, buffer, offset);
    //         if (rs == OPERATION_BREAK)
    //         {
    //             break;
    //         }
    //         else if (rs == OPERATION_CONTINUE)
    //         {
    //             continue;
    //         }
    //         if (x == '+' || x == '-')
    //         {
    //             if (!buffer.empty())
    //             {
    //                 return parseAddBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket);
    //             }
    //             else
    //             {
    //                 //act as prefix to number
    //                 buffer.push_back(x);
    //             }
    //         }
    //         else if (x == '*' || x == '/')
    //         {
    //             if (!buffer.empty())
    //             {
    //                 CalcEntity *mu = parseMultiplyBus(input, offset, consumeEntityFromBuffer(buffer), fromBracket);
    //                 return parseAddBus(input, offset, mu, fromBracket);
    //             }
    //             else
    //             {
    //                 throw "Syntax Error";
    //             }
    //         }
    //         else if (x == '(')
    //         {
    //             (*offset)++;
    //             CalcFunction *pre = nullptr;
    //             bool b = true;
    //             if (!buffer.empty())
    //             {
    //                 pre = static_cast<CalcFunction *>(consumeEntityFromBuffer(buffer));
    //             }
    //             CalcEntity *rt = nullptr;
    //             do
    //             {
    //                 rt = parseBracketBus(input, offset, &b);
    //                 if (pre != nullptr)
    //                 {
    //                     pre->pushArg(rt);
    //                 }
    //             } while (!b);
    //             if (pre != nullptr)
    //             {
    //                 return pre;
    //             }
    //             else
    //             {
    //                 return rt;
    //             }
    //         }
    //     }
    //     if (buffer.empty())
    //     {
    //         return nullptr;
    //     }
    //     else
    //     {
    //         return consumeEntityFromBuffer(buffer);
    //     }
    // }

    CalcEntity *parseInternalV2(std::string &input, int *offset, CalcEntity *rootEntity, bool inverse, CalcFunction *paritallyFilledBus, bool *fromBracket)
    {
        //new strategy... use function bus instead of reursive objects
        std::string buffer;
        double proposedPriority = -1;
        CalcFunction *wrapper = nullptr;
        if (paritallyFilledBus != nullptr)
        {
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
                    if (paritallyFilledBus != nullptr)
                    {
                        checkForInverse(wrapper, inverse, paritallyFilledBus->getPriority());
                    }
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
                    else if (paritallyFilledBus != nullptr)
                    {
                        paritallyFilledBus->pushArg(rt);
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
                    if (!buffer.empty() || rootEntity != nullptr)
                    {
                        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
                        paritallyFilledBus->pushArg(rootEntity);
                    }
                    checkForInverse(wrapper, inverse, proposedPriority);
                }
                proposedPriority = -1;
            }
        }
        resolvePreviousBufferV2(rootEntity, wrapper, buffer);
        if (paritallyFilledBus != nullptr && rootEntity != nullptr)
        {
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
            char last = x.back();
            bool ls = last == ';';
            if (ls)
            {
                x.pop_back();
            }
            CalcEntity *clc = r->parseExpV2(x);
            if (clc != nullptr)
            {
                clc->simplify(args);
                if (!ls)
                {
                    std::cout << clc->getValue() << std::endl;
                }
                else
                {
                    std::cout << "Ok" << std::endl;
                }
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