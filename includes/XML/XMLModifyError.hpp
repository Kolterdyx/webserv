//
// Created by kolterdyx on 30/01/23.
//

#ifndef XMLPARSER_XMLMODIFYERROR_HPP
#define XMLPARSER_XMLMODIFYERROR_HPP

#include "XMLError.hpp"

class XMLModifyError : public XMLError {
public:
    explicit XMLModifyError(const std::string &message) : XMLError("Modify error: " + message) {}
};


#endif //XMLPARSER_XMLMODIFYERROR_HPP
