//
// Created by kolterdyx on 29/01/23.
//

#include <sstream>
#include "XML/XMLElement.hpp"
#include "XML/XMLParseError.hpp"
#include "XML/XMLAccessError.hpp"
#include "XML/XMLModifyError.hpp"

XMLElement::XMLElement(const std::string &name) {
    this->name = name;
    _hasContent = false;
    _isComment = false;
    parent = NULL;
    this->visible = true;
}

XMLElement::XMLElement() {
    this->name = "element";
    _hasContent = false;
    _isComment = false;
    parent = NULL;
    this->visible = true;
}

XMLElement::XMLElement(const std::string& name, const std::map<std::string, std::string>& attributes) {
    this->name = name;
    this->attributes = attributes;
    _hasContent = false;
    _isComment = false;
    parent = NULL;
    this->visible = true;
}

std::string XMLElement::getName() const {
    return name;
}

void XMLElement::setName(const std::string &name) {
    if (isComment())
        return;
    this->name = name;
}

std::map<std::string, std::string> XMLElement::getAttributes() const {
    if (isComment())
        return std::map<std::string, std::string>(); // Return empty map.
    return attributes;
}

void XMLElement::setAttributes(const std::map<std::string, std::string>& attributes) {
    if (isComment())
        return;
    this->attributes = attributes;
}

std::vector<XMLElement *> XMLElement::getChildren() const {
    if (isComment())
        return std::vector<XMLElement *>(); // Return empty vector.
    if (!hasContent())
        return children;
    else {
        throw XMLAccessError("Element has text text content.");
    }
}

void XMLElement::setAttribute(const std::string &name, const std::string &value) {
    if (isComment())
        return;
    attributes[name] = value;
}

void XMLElement::removeAttribute(const std::string &name) {
    if (isComment())
        return;
    attributes.erase(name);
}

std::string XMLElement::getAttribute(const std::string &name) const {
    if (isComment())
        return "";
    return attributes.at(name);
}

std::string XMLElement::getContent() const {
    if (hasContent() || isComment())
        return textContent;
    else {
        throw XMLAccessError("XMLDocument: Element has XMLDocument children.");
    }
}

void XMLElement::setContent(const std::string &content, bool force) {
    if (isComment())
        return;
    if (force) {
        this->textContent = content;
        _hasContent = true;
        children.clear();
    } else {
        if (children.empty()) {
            this->textContent = content;
            _hasContent = true;
        } else {
            throw XMLModifyError("XMLDocument: Element has XMLDocument children. Use setContent(textContent, true) to force (THIS WILL ERASE ALL CHILDREN).");
        }
    }
    if (content.empty())
        _hasContent = false;
}

XMLElement *XMLElement::fromString(const std::string &xml) {
    XMLElement *element = new XMLElement();

    if (xml.empty())
        throw XMLParseError("Empty string.");
    if (xml[0] != '<')
        throw XMLParseError("String does not start with '<'.");
    if (trim(xml, " \n")[trim(xml, " \n").length() - 1] != '>')
        throw XMLParseError("String does not end with '>'.");
    if (xml.find("<!--") == 0)
    {
        element->setContent(trim(xml.substr(4, xml.length() - 7), " \t"), true);
        element->_isComment = true;
        element->name = "__comment__";
        return element;
    }

    // First, we need to find the name of the element. This is the first word after the '<'.
    // The XMLDocument tag may or may not have attributes, so we need to find either the first space or the first '>'.
    // The XMLDocument tag may also be self-closing, so we need to check for that as well.
    size_t nameEnd = xml.find_first_of(" />");
    if (nameEnd == std::string::npos)
        throw XMLParseError("Invalid tag.");
    if (xml[nameEnd] == '/' && xml[nameEnd + 1] != '>')
        throw XMLParseError("Invalid tag.");
    std::string name = xml.substr(1, nameEnd - 1);
    if (name.empty() || name.find_first_of(" /,'\"") != std::string::npos)
        throw XMLParseError("Invalid tag.");
    element->setName(name);

    // Now we determine if this is a self-closing tag.
    size_t tagEnd = find_first_of_unquoted(xml, ">", 0, xml.length());
    size_t selfClosingChar = find_first_of_unquoted(xml, "/", 0, tagEnd);
    bool selfClosing = false;
    if (selfClosingChar != std::string::npos && selfClosingChar + 1 == tagEnd) {
        // This is a self-closing tag.
        selfClosing = true;
        tagEnd = selfClosingChar;
    }

    // Now we need to find the attributes.
    // Attributes are in the form of "name="value"" or "name='value'", and are separated by spaces.
    // We need to find the first space after the name, and then the first space after that.
    // The first space after the name is the start of the first attribute.
    // The first space after that is the end of the first attribute.
    // The second space after that is the start of the second attribute.
    // The second space after that is the end of the second attribute.
    // And so on.
    // Since we don't know how many attributes there are, we need to loop until we find the end of the tag.

    size_t attributeStart = find_first_of_unquoted(xml, " ", 0, std::string::npos);
    if (attributeStart != std::string::npos && attributeStart < tagEnd) {
        // There are attributes.
        size_t attributeEnd = find_first_of_unquoted(xml, " />", attributeStart + 1, std::string::npos);
        while (attributeEnd != std::string::npos && attributeEnd <= tagEnd) {
            // We have found an attribute.
            std::string attribute = xml.substr(attributeStart + 1, attributeEnd - attributeStart - 1);
            size_t equals = attribute.find_first_of('=');
            if (equals == std::string::npos)
                throw XMLParseError("Invalid attribute.");
            if (equals == 0)
                throw XMLParseError("Invalid attribute.");
            if (equals == attribute.length() - 1)
                throw XMLParseError("Invalid attribute.");
            if (!((attribute[equals + 1] == '"' || attribute[equals + 1] == '\'') && attribute[attribute.length() - 1] == attribute[equals + 1]))
                throw XMLParseError("Invalid attribute.");
            std::string attributeName = attribute.substr(0, equals);
            std::string attributeValue = attribute.substr(equals + 2, attribute.length() - equals - 3);
            element->setAttribute(attributeName, replaceEscapeSequences(attributeValue));
            attributeStart = attributeEnd;
            attributeEnd = find_first_of_unquoted(xml, " />", attributeStart + 1, std::string::npos);
        }
    }

    // Now we need to find the textContent.
    // If this is a self-closing tag, there is no textContent.
    // If this is not a self-closing tag, the textContent is everything between the end of the tag and the start of the closing tag.
    if (!selfClosing) {
        size_t contentStart = xml.find_first_of('>', tagEnd) + 1;
        size_t contentEnd = xml.find_last_of('<');
        if (contentStart == std::string::npos || contentEnd == std::string::npos)
            throw XMLParseError("Invalid tag.");
        std::string content = xml.substr(contentStart, contentEnd - contentStart);
        // Now we need to check if this textContent is XMLDocument or not.
        // If it is XMLDocument, we can recursively call fromString() on it.
        // If it is not XMLDocument, we can just set the textContent.
        element->_hasContent = false;
        std::vector<std::string> xmlChildren = splitXML(content);
        for (std::vector<std::string>::const_iterator it = xmlChildren.begin(); it != xmlChildren.end(); ++it) {
            std::string childStr = *it;
            if (childStr.empty())
                continue;
            if (childStr[0] != '<') {
                element->setContent(replaceEscapeSequences(content));
                break;
            }
            element->addChild(XMLElement::fromString(*it));
        }
    }

    return element;
}

void XMLElement::addChild(XMLElement *element) {
    if (!element)
        throw XMLModifyError("Element is NULL.");
    if (hasContent()) {
        throw XMLModifyError("Element has text content.");
    }
    children.push_back(element);
    element->parent = this;
    element->visible = true;
}

std::vector<std::string> XMLElement::splitXML(std::string xmlString) {
    // This function splits an XMLDocument string that contains multiple XMLDocument elements next to each other at a depth of 0.
    // For example, if you have the string "<a></a><b><c></c></b><d/>", this function will return a vector containing
    // "<a></a>", "<b><c></c></b>" and "<d/>".
    // This function has to be able to handle self-closing tags, tags that contain other tags, and comments.
    // This function is used by fromString() to split the textContent of an element into multiple elements.

    // We do this by finding the first opening tag, and then finding the closing tag that matches it. We then repeat this process. If we find a self-closing tag, we also need to save it.
    // We need to keep track of the depth of the tags. If we find a closing tag, we need to decrement the depth. If we find an opening tag, we need to increment the depth.
    // If the depth is 0, we have found a tag that is at the same depth as the first tag we found.
    // If the depth is 1, we have found a tag that is one level deeper than the first tag we found.
    // We need to keep track of the depth of the tags, so that we know when we have found the closing tag that matches the first opening tag.

    std::vector<std::string> xmlElements;
    size_t depth = 0;
    size_t start = find_first_of_unquoted(xmlString, "<", 0, std::string::npos);
    size_t startOpen = 0;
    size_t end = 0;
    size_t selfClosing = 0;
    if (!trim(xmlString.substr(0, start), " \t\n").empty() && start != std::string::npos)
        throw XMLParseError("Invalid text content.");
    while (start != std::string::npos) {
        if (xmlString.substr(start + 1, 3) == "!--" && (start == 0 || xmlString[start - 1] != '!')) {
            // This is a comment.
            end = xmlString.find("-->", start);
            if (end == std::string::npos)
                throw XMLParseError("Invalid comment.");
            std::string comment = xmlString.substr(start, end - start + 3);
            if (depth == 0)
                xmlElements.push_back(comment);
            ++end;
            start = find_first_of_unquoted(xmlString, "<", end, std::string::npos);
            continue;
        }
        end = find_first_of_unquoted(xmlString, ">", start, std::string::npos);
        if (end == std::string::npos)
            throw XMLParseError("Invalid tag.");
        selfClosing = find_first_of_unquoted(xmlString, "/>", start, end);
        if (selfClosing != std::string::npos && selfClosing + 1 == end) {
            // This is a self-closing tag.
            if (depth == 0)
                xmlElements.push_back(xmlString.substr(start, end - start + 1));
            ++end;
            start = find_first_of_unquoted(xmlString, "<", end, std::string::npos);
            continue;
        }
        if (xmlString[start + 1] == '/') {
            // This is a closing tag.
            --depth;
            if (depth == 0) {
                // We have found the closing tag that matches the first opening tag.
                xmlElements.push_back(xmlString.substr(startOpen, end - startOpen + 1));
                ++end;
            }
        } else {
            // This is an opening tag.
            if (depth == 0)
                startOpen = start;
            ++depth;
        }
        start = find_first_of_unquoted(xmlString, "<", end, std::string::npos);
    }
    if (depth != 0)
        throw XMLParseError("Invalid tag.");
    else if (xmlElements.empty())
        xmlElements.push_back(xmlString);
    return xmlElements;
}

size_t XMLElement::find_first_of_unquoted(std::string haystack, std::string needles, size_t start, size_t end) {
    // This function is like std::string::find_first_of(), but it ignores characters that are inside quotes.
    // For example, if you have the string "a\"b\"c", this function will return 3 when you search for the first quote.
    // This function is used by splitXML() to find the end of a self-closing tag.

    bool inQuotes = false;
    for (size_t i = start; i < end && i < haystack.length(); ++i) {
        if (haystack[i] == '"') {
            inQuotes = !inQuotes;
        } else if (!inQuotes) {
            for (size_t j = 0; j < needles.length(); ++j) {
                if (haystack[i] == needles[j]) {
                    return i;
                }
            }
        }
    }
    return std::string::npos;
}

const std::string &XMLElement::replaceEscapeSequences(std::string textContent) {
    // This function parses the text textContent of an element. It replaces all XMLDocument escape sequences with their corresponding characters.
    // For example, if you have the string "&lt;a&gt;", this function will return "<a>".
    // This function is used by fromString() to parse the textContent of an element.

    static std::string parsedTextContent;
    parsedTextContent = textContent;
    size_t ampersand = parsedTextContent.find('&');
    while (ampersand != std::string::npos) {
        size_t semicolon = parsedTextContent.find(';', ampersand);
        if (semicolon == std::string::npos)
            throw XMLParseError("Invalid escape sequence.");
        std::string escapeSequence = parsedTextContent.substr(ampersand, semicolon - ampersand + 1);
        if (escapeSequence == "&lt;")
            parsedTextContent.replace(ampersand, 4, "<");
        else if (escapeSequence == "&gt;")
            parsedTextContent.replace(ampersand, 4, ">");
        else if (escapeSequence == "&amp;")
            parsedTextContent.replace(ampersand, 5, "&");
        else if (escapeSequence == "&apos;")
            parsedTextContent.replace(ampersand, 6, "'");
        else if (escapeSequence == "&quot;")
            parsedTextContent.replace(ampersand, 6, "\"");
        else
            throw XMLParseError("Invalid escape sequence.");
        ampersand = parsedTextContent.find('&', ampersand + 1);
    }
    return parsedTextContent;
}

std::string XMLElement::trim(std::string str, std::string chars) {
    // This function removes all the characters in the string "chars" from the beginning and end of the string "str".

    size_t start = str.find_first_not_of(chars);
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(chars);
    return str.substr(start, end - start + 1);
}

XMLElement *XMLElement::createChild(const std::string &name) {
    // This function creates a new child element with the specified name and adds it to the children of this element.
    // It returns a pointer to the new child element.

    if (name.empty())
        throw XMLModifyError("Cannot create child element with empty name.");

    if (hasContent())
        throw XMLModifyError("Cannot add child to element with content. Use setContent() first to remove the content.");
    XMLElement *child = new XMLElement(name);
    children.push_back(child);
    child->parent = this;
    return child;
}

XMLElement::~XMLElement() {
    clearChildren();
}

size_t XMLElement::getChildrenCount() const {
    if (hasContent() || isComment())
        return 0;
    int i = 0;
    for (std::vector<XMLElement*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        XMLElement *child = *it;
        if (!child->isComment() && child->visible)
            ++i;
    }
    return i;
}

void XMLElement::removeChild(XMLElement *element) {
    // This function removes the specified child element from the children of this element.
    if (hasContent() || isComment()) {
        return;
    }

    for (std::vector<XMLElement*>::iterator it = children.begin(); it != children.end(); ++it) {
        XMLElement *child = *it;
        if (child == element) {
            children.erase(it);
            child->parent = NULL;
            return;
        }
    }
    throw XMLModifyError("Element is not a child of this element.");
}

void XMLElement::clearChildren() {
    // This function removes all the children of this element.
    if (hasContent() || isComment())
        return;

    for (std::vector<XMLElement*>::iterator it = children.begin(); it != children.end(); ++it) {
        XMLElement *child = *it;
        child->parent = NULL;
        delete child;
    }
    children.clear();
}

void XMLElement::clearAttributes() {
    // This function removes all the attributes of this element.
    if (isComment())
        return;
    attributes.clear();
}

void XMLElement::clearTextContent() {
    // This function removes the text content of this element.
    if (isComment())
        return;
    textContent.clear();
    _hasContent = false;
}

std::string XMLElement::toString() const {
    return toPrettyString(4);
}

std::string XMLElement::toPrettyString(int indent) const {
    return toPrettyString(indent, 0);
}

std::string XMLElement::toPrettyString(int indent, int level) const {
    if (!visible)
        return "<deleted element/>";
    // This function returns a string representation of the element, with proper indentation and newlines. C++98

    std::string str;
    for (int i = 0; i < indent * level; ++i)
        str += " ";

    if (isComment()) {
        str += "<!-- " + textContent + " -->";
        return str;
    }

    str += "<" + name;

    for (std::map<std::string, std::string>::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        str += " " + it->first + "=\"" + it->second + "\"";
    }

    if (children.empty() && textContent.empty()) {
        str += "/>";
        return str;
    } else {
        str += ">";
        if (_hasContent) {
            str += textContent;
        } else {
            str += "\n";
            for (std::vector<XMLElement*>::const_iterator it = children.begin(); it != children.end(); ++it) {
                const XMLElement *child = *it;
                str += child->toPrettyString(indent, level + 1) + "\n";
            }
            for (int i = 0; i < indent * level; ++i)
                str += " ";
        }
        str += "</" + name + ">";
    }
    return str;
}

std::ostream &operator<<(std::ostream &os, const XMLElement *element) {
    os << (*element);
    return os;
}

std::ostream &operator<<(std::ostream &os, const XMLElement &element) {
    os << element.toPrettyString(4);
    return os;
}

bool XMLElement::hasContent() const {
    return _hasContent;
}

bool XMLElement::isComment() const {
    return _isComment;
}

bool XMLElement::hasAttribute(const std::string &name) const {
    if (isComment())
        return false;
    return attributes.find(name) != attributes.end();
}

bool XMLElement::hasChild(const std::string &name) const {
    if (hasContent() || isComment())
        return false;

    for (std::vector<XMLElement*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        const XMLElement *child = *it;
        if (child->getName() == name)
            return true;
    }
    return false;
}

std::vector<XMLElement *> XMLElement::query(const std::string &selector) const {
    // This function returns a vector of all the children of this element that match the specified selector.

    if (hasContent() || isComment())
        return std::vector<XMLElement*>();

    std::vector<XMLElement*> result;
    std::vector<std::string> selectors = split(selector, '/');
    std::string firstSelector = selectors[0];
    std::string restSelector;
    if (selectors.size() > 1) {
        restSelector = selectors[1];
        for (size_t i = 2; i < selectors.size(); ++i) {
            restSelector += "/" + selectors[i];
        }
    } else {
        restSelector = "";
    }

    // Check if any of the children match the first selector.
    // If so, add them to the result vector.
    for (std::vector<XMLElement*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        XMLElement *child = *it;
        if (child->matchesSelector(firstSelector)) {
            if (restSelector.empty()) {
                result.push_back(child);
            } else {
                std::vector<XMLElement*> subResult = child->query(restSelector);
                result.insert(result.end(), subResult.begin(), subResult.end());
                if (firstSelector.substr(0, 2) == "**" || firstSelector.substr(0, 1) == "@") {
                    subResult = child->query(firstSelector + "/" + restSelector);
                    result.insert(result.end(), subResult.begin(), subResult.end());
                }
            }
        } else if (firstSelector.substr(0, 1) == "@") {
            std::vector<XMLElement*> subResult;
            if (restSelector.empty()) {
                result.push_back(child);
                subResult = child->query(firstSelector);
            }
            else
                subResult = child->query(firstSelector + "/" + restSelector);
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }
    return result;
}

bool XMLElement::matchesSelector(std::string selector) const {
    // Check if this element matches the specified selector.
    // The selector is in CSS selector format.
    // Examples:
    // - element
    // - element:not(selector)
    // - element[@attribute]
    // - element[@attribute='value']
    // - element:not(selector)[@attribute][@attribute='value']

    if (!visible)
        return false;
    // Check if selector has a name
    std::string name;
    if (selector.empty())
        name = this->name;
    else if (selector.find(':') != std::string::npos) {
        name = selector.substr(0, selector.find(':'));
        selector = selector.substr(selector.find(':'));
    } else if (selector.find('[') != std::string::npos) {
        name = selector.substr(0, selector.find('['));
        selector = selector.substr(selector.find('['));
    } else {
        name = selector.substr(0);
        selector = "";
    }
    if (!name.empty() && name != this->name && name != "*" && name != "**" && name != "@")
        return false;

    // Check if selector has a not clause. If so, make sure to extract all the content inside the parentheses.
    // Must be capable of handling nested parentheses.
    std::string notSelector;
    if (selector.find(":not(") != std::string::npos) {
        int start = selector.find(":not(") + 5;
        int end = start;
        int depth = 1;
        while (depth > 0) {
            if (selector[end] == '(')
                depth++;
            else if (selector[end] == ')')
                depth--;
            end++;
        }
        notSelector = name + selector.substr(start, end - start - 1);
        if (matchesSelector(notSelector))
            return false;
        selector = selector.substr(0, selector.find(":not(")) + selector.substr(end);
    }

    // Check if selector has any attribute or child selectors
    // If the selector looks like this: [@name] or [@name='value']
    // then it is an attribute selector.
    // If the selector looks like this: [name]
    // then it is a child selector. Child selectors don't have a value.

    std::vector<std::string> selectors;
    if (selector.find('[') != std::string::npos) {
        int start = selector.find('[');
        size_t end = start;
        int depth = 1;
        while (depth > 0 && end < selector.length()) {
            if (selector[end] == '[')
                depth++;
            else if (selector[end] == ']')
                depth--;
            end++;
        }
        std::string part = selector.substr(start, end - start);
        selectors.push_back(part);
        selector = selector.substr(part.length());
    }
    for (std::vector<std::string>::const_iterator it = selectors.begin(); it != selectors.end(); ++it) {
        std::string selector = *it;
        // Get rid of the enclosing brackets. This must keep any inner bracket pairs intact.
        if (selector[0] == '[')
            selector = selector.substr(1);
        if (selector.find_last_of(']') != std::string::npos)
            selector = selector.substr(0, selector.find_last_of(']'));
        bool isAttributeSelector = false;
        if (selector.empty())
            continue;
        if (selector[0] == '@') {
            isAttributeSelector = true;
            selector = selector.substr(1);
        }
        if (isAttributeSelector) {
            // Check if selector has a value
            std::string attribute;
            std::string value;
            if (selector.find('=') != std::string::npos) {
                attribute = selector.substr(0, selector.find('='));
                value = selector.substr(selector.find('=') + 1);
                if (value[0] == '\'' || value[0] == '"')
                    value = value.substr(1, value.size() - 2);
            } else {
                attribute = selector;
            }
            if (!value.empty()) {
                if (!hasAttribute(attribute) || getAttribute(attribute) != value)
                    return false;
            } else {
                if (!hasAttribute(attribute))
                    return false;
            }
        } else {
            std::string childSelector = selector;
            if (!hasChild(childSelector))
                return false;
        }
    }
    return true;
}

std::vector<std::string> XMLElement::split(const std::string &str, char split_char) const {
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

void XMLElement::setParent(XMLElement *parent) {
    this->parent = parent;
}

void XMLElement::remove() {
    if (parent)
        parent->removeChild(this);
    visible = false;
}

