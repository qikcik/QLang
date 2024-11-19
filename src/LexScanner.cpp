#include "LexScanner.hpp"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stack>
#include <ostream>



LexScanner::LexScanner(std::shared_ptr<CodeSource> inSource, const std::vector<std::string>& inSeparators)
:   source(std::move(inSource)),
    separators(inSeparators)
{
    next();
}

std::optional<LexToken::Any> LexScanner::next()
{
    auto beginIdx = positionIdx;
    while(source->content[positionIdx] != '\0')
    {
        auto charIt = source->content[positionIdx];

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
            std::cout << "\nCRITICAL SCANNER ERROR " << source->printHint(currentLine,positionIdx) << "unexpected character" << std::endl;
            throw std::runtime_error( "unidentified token starting with:" + std::to_string(static_cast<char>(source->content[positionIdx])) + " at: "+ std::to_string(positionIdx));
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
    if(!isdigit(source->content[positionIdx])) return false;

    std::string summed;
    const size_t startPositionIdx = positionIdx;
    bool containsDot = false;
    while(positionIdx != source->content.size())
    {
        auto charIt = source->content[positionIdx];

        if(isdigit(charIt))
        {
            summed += charIt;
            positionIdx++;
        }
        else if(charIt=='.' and !containsDot)
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
    if(source->content[positionIdx] != '"') return false;

    auto beginIdx = positionIdx;
    positionIdx++;

    while(source->content[positionIdx] != '\0')
    {
        auto prevCharIt = source->content[positionIdx-1];
        auto charIt = source->content[positionIdx];

        if(charIt == '"' && prevCharIt != '\\') // allow escape code
            break;

        positionIdx++;
    }
    positionIdx++; // escape string
    currentToken = LexToken::String{makeSource(beginIdx),source->content.substr(beginIdx,positionIdx-beginIdx)};
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
            if(source->content[beginIdx+operatCharIdx] != operatIt[operatCharIdx])
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
    if(!((source->content[positionIdx] >= 'a' && source->content[positionIdx] <= 'z') || (source->content[positionIdx] >= 'A' && source->content[positionIdx] <= 'Z')))
        return false;

    auto beginIdx = positionIdx;
    while(source->content[positionIdx] != '\0')
    {
        auto charIt = source->content[positionIdx];

        if ((charIt >= '0' && charIt <= '9') || (charIt >= 'A' && charIt <= 'Z') ||
            (charIt >= 'a' && charIt <= 'z') || charIt == '_')
        {
            positionIdx++;
        } else break;
    }

    currentToken = LexToken::Label{makeSource(beginIdx),source->content.substr(beginIdx,positionIdx-beginIdx)};
    return true;
}

LexToken::Source LexScanner::makeSource(size_t startingCharacter) const
{
    return LexToken::Source {
        .fromSource = source,
        .atLine =  currentLine,
        .startingCharacter = startingCharacter,
    };
}

