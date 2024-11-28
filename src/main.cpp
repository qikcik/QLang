#include <algorithm>
#include <iostream>
#include <map>

#include "LexScanner.hpp"
#include "LexToken.hpp"
#include <memory>
#include <stack>

#include "vx.hpp"

#include "AstParser.hpp"
#include "AstTreeWalkInterpreter.hpp"
#include "CodeSource.hpp"


int main()
{
    auto rootScope = RuntimeScope(nullptr);

    while(true)
    {
        std::shared_ptr<CodeSource> source = std::make_shared<CodeSource>("repl");
        std::cout << ">";
        std::getline(std::cin, source->content);

        if(source->content == "exit")
            break;

        if(source->content == "multiline")
        {
            source->content = "";
            while(true)
            {
                std::string line;
                std::getline(std::cin, line);
                if(line == "run")
                    break;

                source->content += line+'\n';
            }
        }

        try
        {
            LexScanner scanner(source,{"+","-","*","^","/","%","(",")","==","!=","<",">","<=",">=","!","&&","||","{","}",":=",",","fn"});

            std::cout << "TOKEKNS: \n";
            while ( scanner.current())
            {
                std::cout << scanner.current() << "\n";
                scanner.next();
            }

            std::cout << "\nAST: \n";
            scanner.restart();
            auto root = AstParser(scanner).block();
            std::cout << *root << "\n";

            std::cout << "\nINTERPRET: \n";


            treeWallInterpret(root,rootScope,rootScope,true);

            std::cout << "\n";
        }
        catch(std::exception& e)
        {
            std::cout << "\nERROR OCCURED: more info above \n";
        }
        catch(FuncReturn& e)
        {
            std::cout << "\nTried return from main scope \n";
        }
    }

    return 0;
}
