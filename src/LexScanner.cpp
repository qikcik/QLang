#include "LexScanner.hpp"

#include <algorithm>
#include <assert.h>
#include <stack>
#include <ostream>

LexScanner::LexScanner(const std::string& inSource, const std::string& inSourceName,const std::vector<std::string>& inSeparators)
:   source(inSource),
    sourceName(inSourceName),
    separators(inSeparators)
{
    next();
}


std::optional<LexToken::Any> LexScanner::next()
{
    auto beginIdx = positionIdx;
    while(source[positionIdx] != '\0')
    {
        auto charIt = source[positionIdx];

        if(charIt == '\n')
        {
            positionIdx++;
            currentLine++;
            continue;
        }

        if(isspace(charIt)) // ignore whitespace
        {
            positionIdx++;
            continue;
        }
        else if(tryTokenizeSeparator())
        {
            return currentToken;
        }
        else if(tryTokenizeString())
        {
            return currentToken;
        }
        else if(tryTokenizeNumber())
        {
            return currentToken;
        }
        else if(tryTokenizeLabel())
        {
            return currentToken;
        }
        else
        {
            throw std::runtime_error( "unidentified token starting with:" + std::to_string(source[positionIdx]));
            break;
        }
    }
    currentToken = std::nullopt;
    return currentToken;
}

void LexScanner::restart()
{
    positionIdx = 0;
    next();
}

bool LexScanner::tryTokenizeNumber()
{
    if(!isdigit(source[positionIdx])) return false;

    std::string summed;
    const size_t startPositionIdx = positionIdx;
    bool containsDot = false;
    while(positionIdx != source.size())
    {
        auto charIt = source[positionIdx];

        if(isdigit(charIt))
        {
            summed += charIt;
            positionIdx++;
        }
        if(charIt=='.' and !containsDot)
        {
            containsDot = true;
            summed += charIt;
            positionIdx++;
        }
        else
        {
            break;
        }
    }

    if(containsDot)
    {
        currentToken = LexToken::Float{makeSource(startPositionIdx),std::stof(summed)};
        return true;
    }


    currentToken = LexToken::Integer{makeSource(startPositionIdx),std::stoi(summed)};
    return true;
}

bool LexScanner::tryTokenizeString()
{
    if(source[positionIdx] != '"') return false;

    auto beginIdx = positionIdx;
    positionIdx++;

    while(source[positionIdx] != '\0')
    {
        auto prevCharIt = source[positionIdx-1];
        auto charIt = source[positionIdx];

        if(charIt == '"' && prevCharIt != '\\') // allow escape code
            break;

        positionIdx++;
    }
    positionIdx++; // escape string
    currentToken = LexToken::String{makeSource(beginIdx),source.substr(beginIdx,positionIdx-beginIdx)};
    return true;
}

bool LexScanner::tryTokenizeSeparator()
{
    auto beginIdx = positionIdx;

    std::string matched {};
    for(auto& operatIt : separators)
    {
        bool match {true};
        for(int operatCharIdx = 0; operatCharIdx != operatIt.size();operatCharIdx++)
        {
            if(source[beginIdx+operatCharIdx] != operatIt[operatCharIdx])
            {
                match = false;
                break;
            }
        }
        if(!match) continue;
        if(matched.size() < operatIt.size()) matched = operatIt; // keep longer match
    }

    if(matched.empty()) return false;

    positionIdx += matched.size();
    currentToken = LexToken::Separator{makeSource(beginIdx),matched};
    return true;
}

bool LexScanner::tryTokenizeLabel()
{
    if(!((source[positionIdx] >= 'a' && source[positionIdx] <= 'z') || (source[positionIdx] >= 'A' && source[positionIdx] <= 'Z')))
        return false;

    auto beginIdx = positionIdx;
    while(source[positionIdx] != '\0')
    {
        auto charIt = source[positionIdx];

        if ((charIt >= '0' && charIt <= '9') || (charIt >= 'A' && charIt <= 'Z') ||
            (charIt >= 'a' && charIt <= 'z') || charIt == '_')
        {
            positionIdx++;
        } else break;
    }

    currentToken = LexToken::Label{makeSource(beginIdx),source.substr(beginIdx,positionIdx-beginIdx)};
    return true;
}

LexToken::Source LexScanner::makeSource(size_t startingCharacter) const
{
    return LexToken::Source {
        .fromSource = sourceName,
        .atLine =  currentLine,
        .startingCharacter = startingCharacter,
    };
}

