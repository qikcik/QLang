#include <algorithm>
#include <iostream>
#include <map>

#include "LexScanner.hpp"
#include "LexToken.hpp"
#include <memory>
#include <stack>

#include "vx.hpp"

#include "AstParser.hpp"


int main()
{

    LexScanner scanner("(2+-2)*2/4","<embed>",{"+","-","*","/","(",")"});
    while ( scanner.current())
    {
        std::cout << scanner.current() << "\n";
        scanner.next();
    }

    std::cout << "AST: \n";
    scanner.restart();
    std::cout << AstParser(scanner).expr()->stringify();

    return 0;
}
