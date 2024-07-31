See [http://chatters.io](https://www.chatters.io/)

ChatterDevices contains firmware for devices that support the Chatters protocol.

Currently only the Communicator devices is supported. This device is also known as the "ChatterBox".
The ChatterBox is based on Adafruit M4 SAMD51, RFM95W, 256/512kb FRAM, and the DS3231 realtime clock.

About Chatters (protocol is in a different repo):
Chatters is a secure mesh communication platform that can use pretty much any medium to pass encrypted messages around. Within Chatters, 
the group of trusted mesh-connected devices is called a "Cluster". All devices in the cluster share a couple of symmetric keys and also
each has their own asymmetric elliptic curve keypair. This combination allows for secure group communication, as well as secure one-on-one 
communication, where devices assisting in mesh delivery between a pair of devices are not able to decrypt the payloads.

Chatters uses a couple of advanced mesh algorithms and techniques, allowing it to route messages through paths that are shortest and most
likely to succeed. Each Chatters device maintains a live ever-changing mesh graph, which is the device's view of how other devices are 
connected. This graph is constructed and maintained by monitoring traffic, pings, and other techniques.

Within a Chatters cluster, devices can be connected to other cluster devices using any combination of the following:
 * LoRa
 * UDP
 * Wired via Serial
 * CAN (wired)

Chatters automatically uses best path, but each device along the way decides which medium will be used for the next hop.

For instance, if the cluster is generally LoRa based, but two devices are connected via CAN, it is likely that CAN connection
is more likely to be used, since it is faster and more likely to succeed than any wireless hop.

Chatters also supports acknowledgements, by default. So even messages that make several hops will receive a confirmation/ack 
from the end recipient device. These acknowledgements are digitally signed by the recipient, so if a confirmation is received,
it is guaranteed to be authentic.

Chatters also uses a distributed mesh cache, where each device in the cluster is responsible for holding encrypted packets,
as requested, for delivery throughout the cluster. Typically, these packets are asymmetrically encrypted, so even the
devices holding mesh packets in their cache are not able to decrypt the packet payloads.


