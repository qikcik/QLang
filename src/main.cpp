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
    std::string source = "2^3^2";
    LexScanner scanner(source,"<embed>",{"+","-","*","^","/","(",")","==","!=","<",">","<=",">=","!"});

    std::cout << "\n\nTOKEKNS: \n";
    while ( scanner.current())
    {
        std::cout << scanner.current() << "\n";
        scanner.next();
    }

    std::cout << "\n\nAST: \n";
    scanner.restart();
    auto root = AstParser(scanner).expr();
    std::cout << root->stringify();

    std::cout << "\n\nINTERPRET: \n";

    std::cout << treeWallInterpret(root.get());

    return 0;
}
