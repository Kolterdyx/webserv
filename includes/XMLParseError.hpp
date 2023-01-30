//
// Created by kolterdyx on 30/01/23.
//

#ifndef XMLPARSER_SMLPARSEERROR_HPP
#define XMLPARSER_SMLPARSEERROR_HPP

#include "XMLError.hpp"

class XMLParseError : public XMLError {
public:
    explicit XMLParseError(const std::string &message) : XMLError("Parse error: " + message) {}
};


#endif //XMLPARSER_SMLPARSEERROR_HPP
