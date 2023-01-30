
#include <sstream>
#include <fstream>
#include "XMLDocument.hpp"
#include "XMLAccessError.hpp"

const XMLElement * XMLDocument::getRoot() const {
    return root;
}

void XMLDocument::setRoot(XMLElement *root) {
    delete XMLDocument::root;
    XMLDocument::root = root;
}

XMLDocument::XMLDocument(const std::string &rootName) {
    root = new XMLElement(rootName);
}


std::string XMLDocument::toString() const {
    return root->toPrettyString(4);
}

std::ostream &operator<<(std::ostream &os, const XMLDocument &xml) {
    os << xml.toString();
    return os;
}

void XMLDocument::fromString(const std::string &xml) {
    XMLElement *root;
    root = XMLElement::fromString(xml);
    setRoot(root);
}

std::string XMLDocument::toPrettyString(int indent) const {
    return root->toPrettyString(indent);
}

XMLDocument::~XMLDocument() {
    delete root;
}

std::vector<std::string> XMLDocument::split(const std::string &str, char split_char) {
    // This function splits the specified string into a vector of strings, using the specified character as a separator.
    // For example, split("foo.bar", '.') will return a vector containing "foo" and "bar".
    // The separator character will be ignored if it is between double or single quotes.

    std::vector<std::string> result;
    std::string current;
    bool in_quotes = false;
    char quote_char = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '\'' || c == '"') {
            if (in_quotes) {
                if (c == quote_char) {
                    in_quotes = false;
                    current += c;
                } else {
                    current += c;
                }
            } else {
                in_quotes = true;
                quote_char = c;
                current += c;
            }
        } else if (c == split_char) {
            if (in_quotes) {
                current += c;
            } else {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}

XMLElement *XMLDocument::getChild(const std::string &selector) const {
    std::vector<XMLElement *> elements = query(selector);
    if (elements.empty()) {
        return NULL;
    }
    return elements[0];
}

void XMLDocument::fromFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    std::string xml;
    std::string line;
    while (std::getline(file, line)) {
        xml += line + "\n";
    }
    fromString(xml);
}


std::vector<XMLElement *> XMLDocument::query(const std::string &query) const {
    std::vector<XMLElement *> result;

    std::vector<std::string> queries = split(query, ' ');

    // queries is a vector of CSS selectors
    // Parse each selector and add the elements that match to the result vector
    XMLElement *tmpRoot = new XMLElement("tmpRoot");
    tmpRoot->addChild(root);
    for (size_t i = 0; i < queries.size(); i++) {
        std::string selector = queries[i].substr(1, queries[i].size() - 1);
        std::vector<XMLElement *> elements = tmpRoot->query(selector);
        result.insert(result.end(), elements.begin(), elements.end());
    }

    return result;
}


//{
//
//    // Check if an element name is specified
//    std::string elementName;
//    size_t pos = selector.find_first_of("[:");
//    elementName = selector.substr(0, pos);
//
//    // Check if a not selector is specified
//    bool notSelector = false;
//    std::string notSelectorString;
//    if (selector.find(":not(") != std::string::npos) {
//        notSelector = true;
//        // Get the string between the parentheses
//        // Must be capable of handling nested parentheses (e.g. :not(other test:not([attribute=value])))
//        size_t start = selector.find(":not(") + 5;
//        size_t end = start;
//        int parentheses = 1;
//        while (parentheses > 0) {
//            if (selector[end] == '(') {
//                parentheses++;
//            } else if (selector[end] == ')') {
//                parentheses--;
//            }
//            end++;
//        }
//        notSelectorString = selector.substr(start, end - start - 1);
//    }
//
//    // Check if an attributes are specified
//    std::string attribute;
//    bool hasAttribute = false;
//    std::string value;
//    bool hasValue = false;
//    pos = selector.find_first_of("[");
//    if (pos != std::string::npos) {
//        hasAttribute = true;
//        size_t end = selector.find_first_of("]");
//        if (end == std::string::npos) {
//            throw XMLAccessError("Invalid selector: " + selector);
//        }
//        std::string attributeString = selector.substr(pos + 1, end - pos - 1);
//        size_t equals = attributeString.find_first_of("=");
//        if (equals != std::string::npos) {
//            attribute = attributeString.substr(0, equals);
//            value = attributeString.substr(equals + 1);
//            hasValue = true;
//        } else {
//            attribute = attributeString;
//        }
//    }
//
//    // Get all elements that match the element name
//    std::vector<XMLElement *> elements;
//    if (elementName.empty()) {
//        elements = query("*");
//    } else {
//        elements = query(elementName);
//    }
//
//    // Filter the elements based on the attribute and value
//    // If a not selector is specified, filter out the elements that have the attribute. If a value is specified, filter
//    // out the elements that have the attribute with the specified value.
//
//    std::vector<XMLElement *> tmp;
//    for (size_t i = 0; i < elements.size(); i++) {
//        if (notSelector) {
//            std::vector<XMLElement *> notElements = query(notSelectorString);
//            bool found = false;
//            for (size_t j = 0; j < notElements.size(); j++) {
//                if (elements[i] == notElements[j]) {
//                    found = true;
//                    break;
//                }
//            }
//            if (!found) {
//                tmp.push_back(elements[i]);
//            }
//        } else {
//            tmp.push_back(elements[i]);
//        }
//    }
//    elements = tmp;
//    std::vector<XMLElement *> result;
//    for (size_t i = 0; i < elements.size(); i++) {
//        if (hasAttribute) {
//            if (hasValue) {
//                if (elements[i]->hasAttribute(attribute) && elements[i]->getAttribute(attribute) == value) {
//                    result.push_back(elements[i]);
//                }
//            } else {
//                if (elements[i]->hasAttribute(attribute)) {
//                    result.push_back(elements[i]);
//                }
//            }
//        } else {
//            result.push_back(elements[i]);
//        }
//    }
//}