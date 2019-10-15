import sys
import xml.etree.ElementTree as ET
from pdb import set_trace
from pprint import pprint

import ns.core
import ns.network
import ns.csma
import ns.internet
import ns.point_to_point
import ns.applications
import ns.wifi
import ns.bridge

ns.core.LogComponentEnable("UdpEchoClientApplication", ns.core.LOG_LEVEL_ALL)
ns.core.LogComponentEnable("UdpEchoServerApplication", ns.core.LOG_LEVEL_ALL)


class Ns3Simulator():
    def __init__(self):
        self.Devices = list()
        self.Links = list()
        return

    def create_links(self):
        pass
    
    def create_nodes(self):
        nCsma = len(self.Devices)

    def run_tapcsma(self):
        mode = "ConfigureLocal"
        tapName = "thetap"

        cmd = ns.core.CommandLine()
        cmd.AddValue("mode", "Mode setting of TapBridge", mode)
        cmd.AddValue("tapName", "Name of the OS tap device", tapName)
        cmd.Parse(sys.argv)
        # CommandLine cmd;
        # cmd.AddValue ("mode", "Mode setting of TapBridge", mode);
        # cmd.AddValue ("tapName", "Name of the OS tap device", tapName);
        # cmd.Parse (argc, argv);

        # GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
        # GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

        # NodeContainer nodes;
        # nodes.Create (4);
        nodes = ns.network.NodeContainer()
        nodes.Create(2)

        # CsmaHelper csma;
        # csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
        # csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
        csma = ns.csma.CsmaHelper()
        csma.SetChannelAttribute("DataRate", ns.core.StringValue("5Mbps"))
        csma.SetChannelAttribute("Delay", ns.core.TimeValue(ns.core.MilliSeconds (2)))

        # NetDeviceContainer devices = csma.Install (nodes);
        devices = csma.Install(nodes)

        # InternetStackHelper stack;
        # stack.Install (nodes);
        stack = ns.internet.InternetStackHelper()
        stack.Install(nodes)

        # Ipv4AddressHelper addresses;
        # addresses.SetBase ("10.1.1.0", "255.255.255.0");
        # Ipv4InterfaceContainer interfaces = addresses.Assign (devices);
        address = ns.internet.Ipv4AddressHelper()
        address.SetBase(ns.network.Ipv4Address("10.1.1.0"), ns.network.Ipv4Mask("255.255.255.0"))
        p2pInterfaces = address.Assign(devices)

        # TapBridgeHelper tapBridge;
        # tapBridge.SetAttribute ("Mode", StringValue (mode));
        # tapBridge.SetAttribute ("DeviceName", StringValue (tapName));
        # tapBridge.Install (nodes.Get (0), devices.Get (0));
        tapBridge = ns.bridge.BridgeHelper()
        tapBridge.SetAttribute("Mode", ns.core.StringValue (mode))
        tapBridge.SetAttribute("DeviceName", ns.core.StringValue (tapName))
        tapBridge.Install(nodes.Get (0), devices.Get (0))

        # csma.EnablePcapAll ("tap-csma", false);
        csma.EnablePcapAll("tap-csma", false)
        # Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

        # Simulator::Stop (Seconds (60.));
        # Simulator::Run ();
        # Simulator::Destroy ();
        ns.core.Simulator.Stop(ns.core.Seconds(60.0))
        ns.core.Simulator.Run()
        ns.core.Simulator.Destroy()
        pass


    def run(self):
        cmd = ns.core.CommandLine()
        cmd.nCsma = 3
        cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices")
        cmd.AddValue("verbose", "Tell echo applications to log if true")
        cmd.Parse(sys.argv)
        nCsma = int(cmd.nCsma)

        nCsma = 1 if int(nCsma) == 0 else int(nCsma)

        p2pNodes = ns.network.NodeContainer()
        p2pNodes.Create(2)

        csmaNodes = ns.network.NodeContainer()
        csmaNodes.Add(p2pNodes.Get(1))
        csmaNodes.Create(nCsma)

        pointToPoint = ns.point_to_point.PointToPointHelper()
        pointToPoint.SetDeviceAttribute("DataRate", ns.core.StringValue("5Mbps"))
        pointToPoint.SetChannelAttribute("Delay", ns.core.StringValue("2ms"))

        p2pDevices = pointToPoint.Install(p2pNodes)

        csma = ns.csma.CsmaHelper()
        csma.SetChannelAttribute("DataRate", ns.core.StringValue("100Mbps"))
        csma.SetChannelAttribute("Delay", ns.core.TimeValue(ns.core.NanoSeconds(6560)))

        csmaDevices = csma.Install(csmaNodes)

        stack = ns.internet.InternetStackHelper()
        stack.Install(p2pNodes.Get(0))
        stack.Install(csmaNodes)

        address = ns.internet.Ipv4AddressHelper()
        address.SetBase(ns.network.Ipv4Address("10.1.1.0"), ns.network.Ipv4Mask("255.255.255.0"))
        p2pInterfaces = address.Assign(p2pDevices)

        address.SetBase(ns.network.Ipv4Address("10.1.2.0"), ns.network.Ipv4Mask("255.255.255.0"))
        csmaInterfaces = address.Assign(csmaDevices)

        echoServer = ns.applications.UdpEchoServerHelper(9)
        echoClient = ns.applications.UdpEchoClientHelper(csmaInterfaces.GetAddress(nCsma), 9)

        serverApps = echoServer.Install(csmaNodes.Get(nCsma))
        serverApps.Start(ns.core.Seconds(1.0))
        serverApps.Stop(ns.core.Seconds(10.0))

        echoClient.SetAttribute("MaxPackets", ns.core.UintegerValue(1))
        echoClient.SetAttribute("Interval", ns.core.TimeValue(ns.core.Seconds (1.0)))
        echoClient.SetAttribute("PacketSize", ns.core.UintegerValue(1024))

        clientApps = echoClient.Install(p2pNodes.Get(0))
        clientApps.Start(ns.core.Seconds(2.0))
        clientApps.Stop(ns.core.Seconds(10.0))

        ns.internet.Ipv4GlobalRoutingHelper.PopulateRoutingTables()

        pointToPoint.EnablePcapAll("second")
        csma.EnablePcap ("second", csmaDevices.Get (1), True)
        ns.core.Simulator.Run()
        ns.core.Simulator.Destroy()

    def xml_import(self, xmlfile):
        tree = ET.parse(xmlfile)
        root = tree.getroot()
        self.Devices = [dev.attrib for parent in root for dev in parent]
        