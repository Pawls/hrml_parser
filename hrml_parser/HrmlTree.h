#ifndef HRML_TREE
#define HRML_TREE

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>

using namespace std;
#define watch(x) cout << (#x) << " is " << (x) << endl;

class HrmlTree
{
private:
    
    struct Node
    {
        string name;
        map<string, string> att_val;
        vector<Node*> child;
        Node* parent = nullptr;
    };

    Node* root = nullptr;
    Node* current_node = nullptr;
    enum class syntax { Tag, Attribute, Value };
    map<string, string>::iterator itr;

public:


    ~HrmlTree();

    void openBlock(string);
    void closeBlock();
    void destroyRecursive(Node*);
    void addAttribute(string, string);
    void cleanToken(string&);
    void parseInput(string);
    Node* traverseRecursive(Node*,string);
    bool checkRelation(string, string);
    HrmlTree();
};
#endif