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


int main()
{
    while(true)
    {
        std::string source{};
        std::cout << ">";
        std::getline(std::cin, source);

        if(source == "exit")
            break;

        LexScanner scanner(source,"<embed>",{"+","-","*","^","/","(",")","==","!=","<",">","<=",">=","!","&&","||"});

        std::cout << "TOKEKNS: \n";
        while ( scanner.current())
        {
            std::cout << scanner.current() << "\n";
            scanner.next();
        }

        std::cout << "\nAST: \n";
        scanner.restart();
        auto root = AstParser(scanner).expr();
        std::cout << root->stringify() << "\n";

        std::cout << "\nINTERPRET: \n";

        std::cout << treeWallInterpret(root.get()) << "\n";
    }

    return 0;
}
