//
// Created by kolterdyx on 30/01/23.
//

#ifndef XMLPARSER_XMLERROR_HPP
#define XMLPARSER_XMLERROR_HPP

#include <stdexcept>

class XMLError : public std::runtime_error {

public:
    explicit XMLError(const std::string &message) : std::runtime_error("XMLDocument: " + message) {}

};


#endif //XMLPARSER_XMLERROR_HPP
