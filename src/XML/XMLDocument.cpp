
#include <sstream>
#include <fstream>
#include "XML/XMLDocument.hpp"
#include "XML/XMLAccessError.hpp"

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

void XMLDocument::fromFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    std::string xml;
    std::string line;
    while (std::getline(file, line)) {
        xml += line + "\n";
    }
    fromString(xml);
}


XMLElementVector XMLDocument::query(const std::string &query) const {
    XMLElementVector result;

    std::vector<std::string> queries = split(query, ';');

    // queries is a vector of CSS selectors
    // Parse each selector and add the elements that match to the result vector
    XMLElement *tmpRoot = new XMLElement("tmpRoot");
    tmpRoot->addChild(root);
    for (size_t i = 0; i < queries.size(); i++) {
        std::string selector = queries[i].substr(1, queries[i].size() - 1);
        XMLElementVector elements = tmpRoot->query(selector);
        result.insert(result.end(), elements.begin(), elements.end());
    }
    tmpRoot->removeChild(root);
    delete tmpRoot;
    return result;
}
