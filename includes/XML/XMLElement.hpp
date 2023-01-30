//
// Created by kolterdyx on 29/01/23.
//

#ifndef XMLPARSER_XMLELEMENT_HPP
#define XMLPARSER_XMLELEMENT_HPP


#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>


namespace std {
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> vector) {
        for (size_t i = 0; i < vector.size(); i++) {
            os << i << ": " << vector[i] << "\n";
        }
        return os;
    }

    template<typename K, typename V>
    std::ostream &operator<<(std::ostream &os, const std::map<K, V> map) {
        for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); it++) {
            os << it->first << " : " << it->second << "\n";
        }
        return os;
    }
}

/**
 * @brief XMLElement class.
 * @details This class is used to represent an XMLDocument element. This class is used by the XMLDocument class to represent the XMLDocument file.
 * This class is responsible for storing the name of the element, the attributes of the element, and the children of the element.
 * An XMLDocument textContent can have text textContent instead of children. In this case, the textContent is stored in the textContent variable, and the
 * _hasContent variable is set to true.
 * @note This class is C++98 compatible.
 */
class XMLElement {

private:

    std::string name;

    std::map<std::string, std::string> attributes;

    std::vector<XMLElement*> children;

    std::string textContent;

    bool _hasContent;

    bool _isComment;

    static std::vector<std::string> splitXML(std::string xmlString);

    static size_t find_first_of_unquoted(std::string haystack, std::string needles, size_t start, size_t end);

    static const std::string &replaceEscapeSequences(std::string textContent);

    static std::string trim(std::string str, std::string chars);

    std::string toPrettyString(int indent, int level) const;

public:

    /**
     * @brief Default constructor.
     * @details This constructor creates an empty element.
     */
    XMLElement();

    /**
     * @brief Constructor.
     * @details This constructor creates an element with the given name.
     * @param name The name of the element.
     */
    explicit XMLElement(const std::string& name);

    /**
     * @brief Constructor.
     * @details This constructor creates an element with the given name and attributes.
     * @param name The name of the element.
     * @param attributes The attributes of the element.
     */
    XMLElement(const std::string& name, const std::map<std::string, std::string>& attributes);

    /**
     * @brief Destructor.
     */
    ~XMLElement();

    /**
     * @brief Get the name of the element.
     * @return The name of the element.
     */
    std::string getName() const;

    /**
     * @brief Set the name of the element.
     * @param name The name of the element.
     */
    void setName(const std::string& name);

    /**
     * @brief Get the attributes of the element.
     * @return The attributes of the element.
     */
    std::map<std::string, std::string> getAttributes() const;

    /**
     * @brief Query the element with the given selector.
     * @return The children of the element that match the selector.
     */
    std::vector<XMLElement*> query(const std::string& selector) const;

    /**
     * @brief Set the attributes of the element.
     * @param attributes The attributes of the element.
     */
    void setAttributes(const std::map<std::string, std::string>& attributes);

    /**
     * @brief Set or add an attribute to the element.
     * @param name The name of the attribute.
     * @param value The value of the attribute.
     */
    void setAttribute(const std::string& name, const std::string& value);

    /**
     * @brief Remove an attribute from the element.
     * @param name The name of the attribute.
     */
    void removeAttribute(const std::string& name);

    /**
     * @brief Get the value of an attribute.
     * @param name The name of the attribute.
     */
    std::string getAttribute(const std::string& name) const;

    bool hasAttribute(const std::string& name) const;

    bool hasChild(const std::string& name) const;

    /**
     * @brief Get the children of the element.
     * @return The children of the element.
     */
    std::vector<XMLElement *> getChildren() const;

    /**
     * @brief Get the textContent of the element.
     * @return The textContent of the element.
     */
    std::string getContent() const;

    /**
     * @brief Set the textContent of the element.
     * @param content The textContent of the element.
     */
    void setContent(const std::string& content, bool force = false);

    /**
     * @brief Generate a string representation of the element.
     * @return A string representation of the element.
     */
    std::string toString() const;

    /**
     * @brief << operator overload.
     */
    friend std::ostream& operator<<(std::ostream& os, const XMLElement& element);
    friend std::ostream& operator<<(std::ostream& os, const XMLElement *element);

    /**
     * @brief Given a properly formatted XMLDocument string, this function will parse it and return the created XMLElement.
     */
    static XMLElement *fromString(const std::string& xml);

    /**
     * @brief Add a child to the element.
     * @param element The child to add.
     */
    void addChild(XMLElement *element);

    /**
     * @brief Create an empty child element with the given name.
     */
    XMLElement *createChild(const std::string& name);

    /**
     * @brief Generate a pretty string representation of the element.
     * @param indent Number of spaces to indent each level.
     * @return
     */
    std::string toPrettyString(int indent) const;

    /**
     * @brief Get the number of children of the element.
     * @return The number of children of the element.
     */
    size_t getChildrenCount() const;

    /**
     * @brief Remove a given child from the element.
     */
    void removeChild(XMLElement *element);

    /**
     * @brief Clear the children of the element.
     */
    void clearChildren();

    /**
     * @brief Clear the attributes of the element.
     */
    void clearAttributes();

    /**
     * @brief Clear the text content of the element.
     */
    void clearTextContent();

    /**
     * @brief Check if the element has text content.
     * @return True if the element has text content, false otherwise.
     */
    bool hasContent() const;

    /**
     * @brief Check if the element is a comment.
     * @return True if the element is a comment, false otherwise.
     */
    bool isComment() const;

    std::vector<std::string> split(const std::string &str, char split_char) const;

    bool matchesSelector(std::string selector) const;
};


#endif //XMLPARSER_XMLELEMENT_HPP