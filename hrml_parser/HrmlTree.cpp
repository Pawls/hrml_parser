#include "HrmlTree.h"

//Constructor
HrmlTree::HrmlTree() {
    openBlock("root");
}

void HrmlTree::cleanToken(string& tkn) {
    tkn.erase(remove(tkn.begin(), tkn.end(), '"'), tkn.end());
    tkn.erase(remove(tkn.begin(), tkn.end(), '>'), tkn.end());
    tkn.erase(remove(tkn.begin(), tkn.end(), '/'), tkn.end());
    tkn.erase(remove(tkn.begin(), tkn.end(), '<'), tkn.end());
}

void HrmlTree::openBlock(string tag_name) {
    Node* temp = new Node;
    temp->name = tag_name;


    if (root == nullptr)
        root = temp;
    else
        temp->parent = current_node;

    if (current_node != nullptr)
        current_node->child.push_back(temp);
    current_node = temp;
}

void HrmlTree::closeBlock() {
    if (current_node != root)
        current_node = current_node->parent;
}

void HrmlTree::addAttribute(string att, string val) {
    current_node->att_val[att] = val;
}

void HrmlTree::destroyRecursive(Node* this_node) {
    if (this_node) {
        for(int i = 0; i < this_node->child.size(); i++)
            destroyRecursive(this_node->child[i]);
        delete this_node;
    }
}

HrmlTree::~HrmlTree() {
    destroyRecursive(root);
}

HrmlTree::Node* HrmlTree::traverseRecursive(Node* this_node, string tag_name) {
    Node* output_node = nullptr;
    if (this_node == nullptr)
        this_node = root;

    if (this_node->name == tag_name) {
        output_node = this_node;
    }
    else {
        for (int i = 0; i < this_node->child.size(); i++) {
            output_node = traverseRecursive(this_node->child[i], tag_name);
            if (output_node != nullptr)
                break;
        }
    }
    return output_node;
}

bool HrmlTree::checkRelation(string parent_name, string child_name) {
    Node* node_ptr;
    node_ptr = traverseRecursive(nullptr, parent_name);
    if (node_ptr != nullptr) {
        for (int i = 0; i < node_ptr->child.size(); i++) {
            if (node_ptr->child[i]->name == child_name) {
                return true;
            }
        }
    }
    return false;
}

void HrmlTree::parseInput(string input_string) {
    stringstream ss(input_string);
    syntax state;
    char ch;
    string str_tag, str_attribute = "", str_value, flush, str, full_str;
    int n, q;

    ss >> n >> q;
    // Parse the HRML
    for (int i = 0; i < n; i++) {
        ss >> ch >> str; // Get tag
        if (ch == '<') { // Handle open tag
            state = syntax::Tag;
            str_tag = str;
            cleanToken(str_tag);
            if (str.find("/") != string::npos) {// If this is a closing tag
                closeBlock();
                getline(ss, flush, '\n'); // Go to next line
            }
            else
                openBlock(str_tag);
            if (str.find(">") != string::npos) {} // If we've reached end of line with no attributes//getline(ss, flush, '\n'); // Go to next line                
            else { // We have at least one attribute, so start scanning
                state = syntax::Attribute;
                while (ss >> str) {
                    if (state == syntax::Attribute) {
                        str_attribute = str;
                        cleanToken(str_attribute);
                        state = syntax::Value;
                        ss >> ch; // Get rid of the '=' between attribute and value
                    }
                    else if (state == syntax::Value) {
                        str_value = str;
                        cleanToken(str_value);
                        addAttribute(str_attribute, str_value);
                        if (str.find(">") != string::npos) {
                            state = syntax::Tag;
                            break;                                
                        }
                        else
                            state = syntax::Attribute;
                    }
                }
            }
        }
    }

    
    // Parse the queries
    string str_parent, str_child;
    bool no_children;
    Node* innermost_node;
    for (int i = 0; i < q; i++) { // Process each query
        innermost_node = nullptr;
        str_attribute = "";
        str_parent = "";
        str_child = "";
        no_children = true;
        while (ss >> ch) {
            if (ch != '.' && ch != '~') { // reading either parent or child
                if (no_children == true)
                    str_parent += ch;
                else
                    str_child += ch;
            }
            if (ch == '.') {
                if (no_children == false) { // if we encounter '.' at this point, we must check the current parent child relationship before reading the next tag
                    if (checkRelation(str_parent, str_child)) {
                        innermost_node = traverseRecursive(nullptr, str_child); //the relationship is valid, so move into child's scope
                        str_parent = str_child;
                        str_child = "";
                    }
                    else { // The parent child relationship was invalid
                        innermost_node = nullptr;
                        cout << "Not Found!\n";
                        getline(ss, flush, '\n');
                        break;
                    }
                }
                else { // If no_children was true after encountering '.', then this is a multi-level scope. Switch to str_child now.
                    innermost_node = traverseRecursive(nullptr, str_parent);
                    if (innermost_node != nullptr) {
                        if (innermost_node->parent == root) // All queries must travel from the root block (i.e. tag1.tag2.tag3~attribute)
                            no_children = false;
                        else {
                            innermost_node = nullptr;
                            cout << "Not Found!\n";
                            getline(ss, flush, '\n');
                            break;
                        }
                    }
                }
            }
            if (ch == '~') {
                if (no_children == false) { // if we encounter '~' at this point, we must check the current parent child relationship before breaking to get attribute
                    if (checkRelation(str_parent, str_child)) {
                        innermost_node = traverseRecursive(nullptr, str_child); //the relationship is valid, so move into child's scope
                        break;
                    }
                    else { // The parent child relationship was invalid
                        innermost_node = nullptr;
                        if (str_parent == str_child)
                            cout << "Invalid Query!\n"; // This will show "Invalid Query" when the parent is the same as child.
                        else
                            cout << "Not Found!\n";
                        //getline(ss, flush, '\n');
                        break;
                    }
                }
                else { // If no_children was true after encountering '~', then this is a single level scope. Break and check for attribute.
                    innermost_node = traverseRecursive(nullptr, str_parent);
                    if (innermost_node != nullptr) {
                        if (innermost_node->parent == root) {// All queries must travel from the root block (i.e. tag1.tag2.tag3~attribute)
                            break;
                        }
                        else {
                            cout << "Not Found!\n";
                            innermost_node = nullptr;
                            //getline(ss, flush, '\n');
                            break;
                        }
                    }
                    else {
                        cout << "Not Found!\n";
                        innermost_node = nullptr;
                        break;
                    }
                    //innermost_node = traverseRecursive(nullptr, str_parent);
                    //break;
                }
            }
        }

        if (ch == '~') {
            ss >> str_attribute;  
            if (innermost_node != nullptr) {
                if (innermost_node->att_val[str_attribute] != "")
                    cout << innermost_node->att_val[str_attribute] << endl;
                else
                    cout << "Not Found!\n";
            }
        }
    }
}

