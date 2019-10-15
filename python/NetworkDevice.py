import uuid
import xml.etree.ElementTree as ET
from pdb import set_trace
from pprint import pprint

class NetworkDevice():
    def __init__(self, devname, busname):
        self.UUID = uuid.uuid4()
        self.DeviceName = devname
        self.Location = str(busname)

    def __str__(self):
        return '%s \'%s\' at Location \"%s\"' \
            %(type(self), self.DeviceName, self.Location)

    def __repr__(self):
        return '%s \'%s\' at Location \"%s\"' \
            %(type(self), self.DeviceName, self.Location)

class Relay(NetworkDevice):
    def __init__(self, devname, busname, accesspoint, nearfar=(None, None)):
        self.AccessPoint = accesspoint.UUID
        self.NearEnd = nearfar[0]
        self.FarEnd = nearfar[1]
        super().__init__(devname, busname)

    def __str__(self):
        return super().__str__()

    def __repr__(self):
        return super().__str__()        

class Router(NetworkDevice):
    def __init__(self, devname, busname):
        super().__init__(devname, busname)

    def __str__(self):
        return super().__str__()

    def __repr__(self):
        return super().__str__()

class Switch(NetworkDevice):
    def __init__(self, devname, busname, accesspoint):
        self.AccessPoint = accesspoint.UUID
        super().__init__(devname, busname)

    def __str__(self):
        return super().__str__()

    def __repr__(self):
        return super().__str__()

class Host(NetworkDevice):
    def __init__(self, devname, busname, accesspoint):
        self.AccessPoint = accesspoint.UUID
        super().__init__(devname, busname)

    def __str__(self):
        return super().__str__()

    def __repr__(self):
        return super().__str__()

class Firewall(NetworkDevice):
    def __init__(self, devname, busname):
        super().__init__(devname, busname)

    def __str__(self):
        return super().__str__()

    def __repr__(self):
        return super().__str__()