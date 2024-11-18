#pragma once
#include <optional>

#include "LexToken.hpp"

class LexScanner
{
public:
    explicit LexScanner(const std::string& inSource, const std::string& inSourceName,const std::vector<std::string>& inSeparators);

    std::optional<LexToken::Any> next();
    std::optional<LexToken::Any> current() {return currentToken;};
    void restart();

    template<typename T>
    std::optional<T> current()
    {
        if(currentToken)
        {
            if (const auto asT = std::get_if<T>(&*currentToken))
                return *asT;
        }
        return {};
    }

    template<typename T>
    std::optional<T> currentMath(const typename T::TContentType& expected)
    {
        if(auto v = current<T>())
        {
            if(v->content == expected)
                return v;
        }
        return {};
    }

protected:
    bool tryTokenizeNumber();
    bool tryTokenizeString();
    bool tryTokenizeSeparator();
    bool tryTokenizeLabel();

    LexToken::Source makeSource(size_t startingCharacter) const;

    const std::string& source;
    size_t currentLine {1};

    size_t positionIdx {};
    std::optional<LexToken::Any> currentToken {};
    std::string sourceName {};
    std::vector<std::string> separators {};
};