//
// Created by kolterdyx on 30/01/23.
//

#ifndef XMLPARSER_XMLACCESSERROR_HPP
#define XMLPARSER_XMLACCESSERROR_HPP

#include "XMLError.hpp"

class XMLAccessError : public XMLError {
public:
    explicit XMLAccessError(const std::string &message) : XMLError("Access error: " + message) {}
};


#endif //XMLPARSER_XMLACCESSERROR_HPP
