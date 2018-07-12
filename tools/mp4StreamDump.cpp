#include <iostream>
#include <ISOBMFF.hpp>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <boost/iostreams/stream.hpp>

int main() {

    ISOBMFF::Parser parser;

    parser.Parse("/tmp/output.m4s");

    std::cout << *parser.GetFile() << std::endl;
    return 0;
}