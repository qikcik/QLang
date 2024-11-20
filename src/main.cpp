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
    while(true)
    {
        std::shared_ptr<CodeSource> source = std::make_shared<CodeSource>("repl");
        std::cout << ">";
        std::getline(std::cin, source->content);

        if(source->content == "exit")
            break;

        try
        {
            LexScanner scanner(source,{"+","-","*","^","/","(",")","==","!=","<",">","<=",">=","!","&&","||","{","}"});

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

            treeWallInterpret(*root);
            std::cout << "\n";
        }
        catch(std::exception& e)
        {
            std::cout << "\nERROR OCCURED: more info above \n";
        }
    }

    return 0;
}
