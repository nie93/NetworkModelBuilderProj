/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream>
#include <vector>

using namespace std;

#ifdef LIBXML_TREE_ENABLED

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */


static void 
print_attribute_names(xmlAttr * a_attr)
{
    xmlAttr *cur_attr = NULL;
    for (cur_attr = a_attr; cur_attr; cur_attr = cur_attr->next) {
        if (cur_attr->type == XML_ATTRIBUTE_NODE) {
            printf("    <%s>: %s\n", cur_attr->name, cur_attr->children->content);
        }
    } 
}

static void
print_dev_info(xmlNode * a_node) {
    xmlAttr *attr = a_node->properties;
    print_attribute_names(attr);
}


static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
            if (cur_node->properties != NULL)
            print_attribute_names(cur_node->properties);
        }
        print_element_names(cur_node->children);
    }
}

static vector<xmlNode *> 
get_childrens(xmlNode * a_node) {
    vector<xmlNode *> elems;
    xmlNode *child = a_node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = child; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            elems.push_back(cur_node);
        }
    }
    return elems;
}

static vector<string>
get_devices_list(xmlNode * a_node) {
    vector<string> names;
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            string sName((char *) cur_node->name);
            names.push_back(sName);
        }
    }
    return names;
}

static void
print_vector(vector<string> v) {
    for (unsigned int i=0; i < v.size(); i++) {
        cout << v.at(i) << endl;
    }
}

class Device {
private:
    string m_uuid, m_name, m_type;
public:
    Device(string uuid, string name, string type) {
        m_uuid = uuid;
        m_name = name;
        m_type = type;
    }

    string get_uuid() {return m_uuid;}
    string get_type() {return m_type;}
    string get_name() {return m_name;}
    void set_uuid(string uuid) {m_uuid = uuid;}
    void set_type(string type) {m_type = type;}
    void set_name(string name) {m_name = name;}
};

int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    vector<string> devNames;
    xmlNode *xmlroot = NULL;
    vector<xmlNode *> elem;
    vector<xmlNode *> routers, switches, hosts, ieds;

    if (argc != 2)
        return(1);

    doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    xmlroot  = xmlDocGetRootElement(doc);
    elem     = get_childrens(xmlroot);
    routers  = get_childrens(elem.at(0));
    switches = get_childrens(elem.at(1));
    hosts    = get_childrens(elem.at(2));
    ieds     = get_childrens(elem.at(3));    

    for (unsigned int i=0; i<routers.size(); i++) {
        xmlNode *dev = routers.at(i);
        printf("--- ROUTER #%d\n", i);
        print_dev_info(dev);
    }

    for (unsigned int i=0; i<switches.size(); i++) {
        xmlNode *dev = switches.at(i);
        printf("--- SWITCH #%d\n", i);
        print_dev_info(dev);
    }
    
    for (unsigned int i=0; i<hosts.size(); i++) {
        xmlNode *dev = hosts.at(i);
        printf("--- HOST #%d\n", i);
        print_dev_info(dev);
    }
    
    for (unsigned int i=0; i<ieds.size(); i++) {
        xmlNode *dev = ieds.at(i);
        printf("--- IED #%d\n", i);
        print_dev_info(dev);
    }
    
    vector<string> deviceList = get_devices_list(xmlroot->children);
    // print_vector(deviceList);

    printf("     Number of elements in root: %lu\n", xmlChildElementCount(xmlroot));
    
    // devnode = devsnode->children->next;
    // print_element_names(devsnode);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
