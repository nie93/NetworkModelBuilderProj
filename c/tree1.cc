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

const xmlChar *
get_uuid(xmlNode * a_node) {
    xmlAttr *attr = a_node->properties;
    xmlAttr *cur_attr = NULL;
    xmlChar *uuid = NULL;
    for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
        if (xmlStrEqual(cur_attr->name, (xmlChar *) "uuid"))
        uuid = cur_attr->children->content;
    }
    return uuid;
}

const xmlChar *
get_ap(xmlNode * a_node) {
    xmlAttr *attr = a_node->properties;
    xmlAttr *cur_attr = NULL;
    xmlChar *ap = NULL;
    for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
        if (xmlStrEqual(cur_attr->name, (xmlChar *) "ap"))
        ap = cur_attr->children->content;
    }
    return ap;
}


static vector<xmlNode *> 
get_dev_related(vector<xmlNode *> elem, xmlNode *dev) {
    vector<xmlNode *> elems;
    const xmlChar *devuuid = get_uuid(dev);
    for (unsigned int i=0; i<elem.size(); i++) {
        xmlNode *subelem = elem.at(i);
        const xmlChar *subelem_ap = get_ap(subelem);
        if ((subelem_ap != NULL) & (xmlStrEqual(devuuid, subelem_ap))) {
            elems.push_back(subelem);
        }
    }
    return elems;
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


static vector<xmlNode *> 
get_dev_cluster_by_router(xmlNode * r, xmlNode * root) {
    vector<xmlNode *> cluster;
    cluster.push_back(r);
    vector<xmlNode *> comm = get_childrens(root);
    vector<xmlNode *> switches, hosts, ieds;
    switches = get_childrens(comm.at(1));
    hosts    = get_childrens(comm.at(2));
    ieds     = get_childrens(comm.at(3));    
    
    vector<xmlNode *> selsw = get_dev_related(switches, r);

    for (unsigned int i=0; i<selsw.size(); i++) {
        vector<xmlNode *> selho = get_dev_related(hosts, selsw.at(i));
        vector<xmlNode *> selie = get_dev_related(ieds, selsw.at(i));
        cluster.insert(cluster.end(), selho.begin(), selho.end());
        cluster.insert(cluster.end(), selie.begin(), selie.end());
    }
    return cluster;
}

static void
print_dev_vector(vector<xmlNode *> v){
    for (unsigned int i=0; i<v.size(); i++) {
        print_dev_info(v.at(i));
    }
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
    vector<xmlNode *> commdevs, routers;

    if (argc != 2)
        return(1);

    doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    xmlroot  = xmlDocGetRootElement(doc);
    commdevs = get_childrens(xmlroot);
    routers  = get_childrens(commdevs.at(0));

    vector<xmlNode *> devclus_r;
    devclus_r = get_dev_cluster_by_router(routers.at(0), xmlroot);
    print_dev_vector(devclus_r);

    
    
    vector<string> deviceList = get_devices_list(xmlroot->children);

    printf("---\n     Number of elements in root: %lu\n", xmlChildElementCount(xmlroot));
    print_vector(deviceList);



    
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
