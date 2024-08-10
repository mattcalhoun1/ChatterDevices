# Chatter Devices

ChatterDevices contains the non-protocol code for the _ChatterBox_, a device that supports the _Chatter_ protocol.

## What is ChatterBox and Chatter?

See [http://chatters.io](https://www.chatters.io/)

### Chatter
Chatter is a secure mesh communication protocol and platform that can use pretty much any medium to pass encrypted messages around. Within Chatter, 
the group of trusted mesh-connected devices is called a "Cluster". All devices in the cluster share a couple of symmetric keys and also
each has their own asymmetric elliptic curve keypair. This combination allows for secure group communication, as well as secure one-on-one 
communication, where devices assisting in mesh delivery between a pair of devices are not able to decrypt the payloads.

Chatter uses a couple of advanced mesh algorithms and techniques, allowing it to route messages through paths that are shortest and most
likely to succeed. Each Chatter device maintains a live ever-changing mesh graph, which is the device's view of how other devices are 
connected. This graph is constructed and maintained by monitoring traffic, pings, and other techniques.

Within a Chatter cluster, devices can be connected to other cluster devices using any combination of the following: LoRa, UDP, Wired via Serial, CAN (wired)

Chatter automatically uses best path, but each device along the way decides which medium will be used for the next hop. For instance, if the cluster is generally LoRa based, but two devices are connected via CAN, it is likely that CAN connection
is more likely to be used, since it is faster and more likely to succeed than any wireless hop.

Chatter supports signed acknowledgements. So even messages that make several hops will receive a confirmation/ack 
from the end recipient device. These acknowledgements are digitally signed by the recipient, so if a confirmation is received,
it is guaranteed to be authentic.

Chatter uses a distributed mesh cache, where each device in the cluster is responsible for holding encrypted packets,
as requested, for delivery throughout the cluster. Typically, these packets are asymmetrically encrypted, so even the
devices holding mesh packets in their cache are not able to decrypt the packet payloads.

## Hardware Compatibility
In order to fully support the ChatterBox protocol, devices that are going to run it must have a few key components that allow the meshing and caching to work properly, and allow the cluster to remain secure and private. 

Chatter is very new (planned release Fall 2024) and currently the [ChatterBox](https://chatters.io/chatterbox) is the only supported device. The repo containing code for various devices is [ChatterDevices](https://github.com/mattcalhoun1/ChatterDevices). Within that repo, ChatterBox is called _Communicator_.

The ChatterBox is designed to be a device [you can build](https://www.chatters.io/build) yourself, with some decent soldering skills. It is based on Adafruit M4 SAMD51, RFM95W, 256/512kb FRAM, and the DS3231 realtime clock.

Compatibility with devices from other manufacturers is in the works.

[<img src="https://img.youtube.com/vi/rJjFlZsUep0/maxresdefault.jpg" width="50%">](https://youtu.be/rJjFlZsUep0)


## Change Log
Pre-built binaries can be found in the [ChatterBuilds](https://github.com/mattcalhoun1/ChatterBuilds) repo.

|      Date    | Short Description | Longer Description |
| ------------ | ----------------- | ------------------ |
| 2024-08-01 | Mesh Improvements | Improve mesh path finding, lock time changes except for root, more. |
| 2024-08-08 | Channel Hopping | Add ability to hop channels for security and also to be an RF "good citizen" |
| 2024-08-09 | Hopping + Fixes | Change channel hopping for mesh hops and for onboarding. Also, a few bug fixes. |
