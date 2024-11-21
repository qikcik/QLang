#pragma once
#include <memory>
#include <string>

struct CodeSource
{
    std::string name;
    std::string content;

    std::string printHint(size_t line, size_t character) const
    {
        std::string linePrefix = std::to_string(line) + "] ";
        size_t currentLine = 1;
        size_t startChar = 0;

        if (line > 1)
            for(size_t i = 0; i < content.size(); i++)
            {
                if(content[i] == '\n')
                    currentLine++;

                if(currentLine == line)
                {
                    startChar = i+1;
                    break;
                }
            }

        std::string result= "in <"+name+">:"+std::to_string(line)+":"+std::to_string(character)+"\n"+linePrefix;
        for(size_t i = startChar; i < content.size(); i++)
        {
            if(content[i] == '\n')
                break;

            result += content[i];
        }
        result += '\n';

        for(int i=0; i!=character+linePrefix.size()-1;i++) result += " ";
        result += " ^ ";
        return result;
    }
};
