import os
from pdb import set_trace
from pprint import pprint
from NetworkDevice import Router, Switch, Host, Relay
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom


class NetworkModelBuilder():
    def __init__(self, c):
        self.Mode = 'Default'
        self.CaseName = str()
        self.CommNodes = list()
        self.CommLinks = list()

        for field in c:
            if field == 'CasePath':
                self.CasePath = os.path.abspath(c[field])
            if field == 'CaseFormat':
                self.CaseFormat = c[field]
        
        if self.CaseFormat == 'ieeecdf':
            self.Case = parse_ieeecdf_dat(self.CasePath, c['IeeeCdfSeperators'])

    def visualize_network(self, filepath):
        pass
        
    def build_comm(self):
        self.add_comm_nodes()

    def add_comm_nodes(self):
        self.add_comm_routers('Main Router')
        self.add_comm_switches(['S1', 'S2'])
        self.add_comm_links_from_topology()
        self.add_comm_hosts('Engineer Workstation')
        self.add_comm_hosts('Control Server')
        self.add_comm_relays()
    
    def add_comm_routers(self, devname):
        if self.Case['BUS DATA']:
            self.CommNodes.extend([Router(devname, data[1]) \
                for data in self.Case['BUS DATA']])

    def add_comm_switches(self, devname):
        if self.Case['BUS DATA']:
            for data in self.Case['BUS DATA']:
                router = get_router_by_location(self.CommNodes, data[1])
                switches = [Switch(name, router.Location, accesspoint=router) for name in devname]            
                for sw in switches:
                    self.CommNodes.append(sw)
                    self.CommLinks.append((router, sw))

    def add_comm_relays(self):
        for bus in self.Case['BUS DATA']:
            busname = bus[1]
            switch = get_switches_by_location(self.CommNodes, busname)[1]
            branches = get_branches_connected_to_bus(self.Case['BRANCH DATA'], bus[0])
            for branch in branches:
                relay = Relay('Relay %03d-%03d' %(branch[0], branch[1]), busname, \
                    accesspoint=switch)
                        # nearfar=('BUS #%s' %branch[0], 'BUS #%s' %branch[1]))
                self.CommNodes.append(relay)
                self.CommLinks.append((switch, relay))

    def add_comm_hosts(self, devname):
        """
        Link: switch --- host(devname)
        """
        if self.Case['BUS DATA']:
            for data in self.Case['BUS DATA']:
                switch = get_switches_by_location(self.CommNodes, data[1])[0]
                host = Host(devname, data[1], accesspoint=switch)
                self.CommNodes.append(host)
                self.CommLinks.append((switch, host))            

    def add_comm_links_from_topology(self):
        if self.Case['BRANCH DATA']:
            for data in self.Case['BRANCH DATA']:
                fbus_name = str([busdata[1] for busdata in self.Case['BUS DATA'] \
                    if data[0] == busdata[0]][0])
                tbus_name = str([busdata[1] for busdata in self.Case['BUS DATA'] \
                    if data[1] == busdata[0]][0])
                fbus_router = get_router_by_location(self.CommNodes, fbus_name)
                tbus_router = get_router_by_location(self.CommNodes, tbus_name)
                self.CommLinks.append((fbus_router, tbus_router))
    
    def xml_serialize(self, filepath):
        # create the file structure
        root = ET.Element('Communication')
        routers = ET.SubElement(root, 'Routers')
        switches = ET.SubElement(root, 'Switches')
        hosts = ET.SubElement(root, 'Hosts')
        ieds = ET.SubElement(root, 'IEDs')

        for dev in self.CommNodes:
            if type(dev) == Router:
                item = ET.SubElement(routers, dev.__class__.__name__)
                item.set('name', dev.DeviceName)
                item.set('uuid', str(dev.UUID))
                item.set('location', dev.Location)
            elif type(dev) == Switch:
                item = ET.SubElement(switches, dev.__class__.__name__)
                item.set('name', dev.DeviceName)
                item.set('uuid', str(dev.UUID))
                item.set('ap', str(dev.AccessPoint))
            elif (type(dev) == Host):
                item = ET.SubElement(hosts, dev.__class__.__name__)
                item.set('name', dev.DeviceName)
                item.set('uuid', str(dev.UUID))
                item.set('ap', str(dev.AccessPoint))
            elif (type(dev) == Relay):
                item = ET.SubElement(ieds, dev.__class__.__name__)
                item.set('name', dev.DeviceName)
                item.set('uuid', str(dev.UUID))
                item.set('ap', str(dev.AccessPoint))
                item.set('nearend', str(dev.NearEnd))
                item.set('farend', str(dev.FarEnd))

        # create a new XML file with the results
        reparsed = minidom.parseString(ET.tostring(root, 'utf-8'))
        with open(filepath, 'w') as f:
            f.write(reparsed.toprettyxml(indent="  "))


# region [Common Methods]
def get_branches_connected_to_bus(branches, busid):
    return [b for b in branches if b[0] == busid or b[1] == busid]


def get_router_by_location(nodes, busname):
    for dev in nodes:
        if (dev.Location == busname and type(dev) == Router):
            return dev

def get_switches_by_location(nodes, busname):
    switches = list()
    router = get_router_by_location(nodes, busname)
    for dev in nodes:
        if (dev.Location == router.Location and type(dev) == Switch):
            switches.append(dev)
    return switches

def parse_ieeecdf_dat(filepath, seperators):
    tables = ['BUS DATA', 'BRANCH DATA']
    case = {table: list() for table in tables}
    with open(filepath) as f:
        r = f.readline()
        for table in tables:
            r = f.readline()
            if '%s FOLLOWS' %table in r:
                nitem = get_itemnum_from_header(r)
                case.update({'SIZE OF %s' %table: nitem})
            r = f.readline()
            while True:
                case[table].append(parse_entryrow(r, seperators[table]))
                r = f.readline()
                if r.startswith('-999'):
                    break    
    return case

def get_itemnum_from_header(s):
    for string in s.split():
        try: 
            n = int(string)
            break
        except:
            pass
    return n

def parse_entryrow(s, seperate):
    data = list()
    if seperate is not None:
        s = s.replace('\n', '')
        MAX_COLUMN = 140
        seperate = seperate + [MAX_COLUMN]

        for sepi in range(0, len(seperate)-1):
            x = s[seperate[sepi]:seperate[sepi+1]]
            if '.' in x:
                data.append(float(x))
            else:
                try:
                    data.append(int(x))
                except:
                    data.append(str(x.strip().replace(' ', '_')))
    else:
        for x in s.split():
            if '.' in x:
                data.append(float(x))
            else:
                try:
                    data.append(int(x))
                except:
                    data.append(str(x.replace(' ', ''))) 
    return data

# endregion