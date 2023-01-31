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
     * @brief Query the XMLDocument tree using CSS-based selectors.
     * @details This function is used to query the XMLDocument tree using CSS-based selectors. All queries start with a '/' character, and can contain the following:
     * You can use the following selectors:
     *
     * @code * @endcode Selects all elements in that level.
     * @code @/selector @endcode Recursively selects all elements matching the given selector in all levels.
     * @code element @endcode Selects all elements with the specified name in that level.
     * @code *[@attribute] @endcode Selects all elements with the specified attribute in that level.
     * @code *[@attribute=value] @endcode Selects all elements with the specified attribute and value in that level.
     * @code element[@attribute] @endcode Selects all elements with the specified name and attribute in that level.
     * @code element[@attribute=value] @endcode Selects all elements with the specified name and attribute and value in that level.
     * @code *[selector] @endcode Selects all elements with the specified name that have a child matching the given selector in that level.
     * @code element[selector] @endcode Selects all elements that have a child matching the given selector in that level.
     * @code element:not(selector) @endcode Selects all elements that do not match the given selector in that level.
     * @code selector;selector @endcode Selects all elements matching the given selectors (OR).
     * @code selector/selector @endcode Selects all elements where the first selector is a direct parent of the second selector.
     *
     * @examples
     * @code /@/@ @endcode Recursively selects all elements in all levels.
     * @code /@/element @endcode Selects all elements with the specified name in all levels.
     * @code /elem1/elem2 @endcode Selects all elements of name elem2 that are direct children of elem1 in the root level.
     * @param query The query to execute.
     * @return A vector (XMLElementVector) of all elements returned by the query.
     */
    XMLElementVector query(const std::string &query) const;

    /**
     * @brief Create XMLDocument object from XMLDocument file.
     * @param filename The path to the XMLDocument file.
     */
    void fromFile(const std::string& filename);

};


#endif //XMLPARSER_XML_HPP
