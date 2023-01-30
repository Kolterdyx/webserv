//
// Created by kolterdyx on 29/01/23.
//

#ifndef XMLPARSER_XML_HPP
#define XMLPARSER_XML_HPP

#include "XMLElement.hpp"


/**
 * @brief XMLDocument class.
 * @details This class is the main class of the XMLParser library. It is used to parse XMLDocument files, and to create and modify XMLDocument data.
 * @note This class is C++98 compatible.
 */
class XMLDocument {

private:
    /**
     * @brief The root element of the XMLDocument file.
     */
    XMLElement *root;

    static std::vector<std::string> split(const std::string &str, char split_char);

    std::vector<XMLElement*> getChildren(const std::string& selector) const;

    XMLElement* getChild(const std::string& selector) const;



public:

    /**
     * @brief Get the root element of the XMLDocument file.
     * @return The root element of the XMLDocument file.
     */
    const XMLElement *getRoot() const;

    /**
     * @brief Set the root element of the XMLDocument file.
     * @param root The new root element of the XMLDocument file.
     */
    void setRoot(XMLElement *root);

    /**
     * @brief Default constructor.
     * @details This constructor creates an empty XMLDocument object.
     * @param rootName The name of the root element.
     */
    explicit XMLDocument(const std::string &rootName = "root");

    /**
     * @brief Destructor.
     */
    ~XMLDocument();

    /**
     * @brief Create XMLDocument object from XMLDocument string.
     */
    void fromString(const std::string& xml);

    /**
     * @brief Generate XMLDocument string.
     */
    std::string toString() const;

    /**
     * @brief << operator overload.
     */
    friend std::ostream& operator<<(std::ostream& os, const XMLDocument& xml);

    /**
     * @brief Generate pretty XMLDocument string.
     * @param indent The number of spaces to indent each level.
     */
    std::string toPrettyString(int indent) const;

    /**
     * @brief Query the XMLDocument tree for elements with a given name (the last of the element chain).
     * @details If an attribute is specified, only elements with that attribute will be returned. If a value is also
     * specified, only elements with that attribute and value will be returned.
     * @param query The element chain to traverse.
     * @param attribute The attribute name to query.
     * @param value The attribute value to query.
     * @return A vector of all elements with the given name, attribute and value.
     */
    std::vector<XMLElement *> query(const std::string &query) const;

    /**
     * @brief Create XMLDocument object from XMLDocument file.
     * @param filename The path to the XMLDocument file.
     */
    void fromFile(const std::string& filename);

};


#endif //XMLPARSER_XML_HPP
