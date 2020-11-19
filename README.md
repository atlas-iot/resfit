[![License](https://img.shields.io/badge/license-GPL%20v3.0%20or%20later-brightgreen.svg)](https://github.com/atlas-iot/resfit/blob/master/LICENSE)

# ATLAS RESFIT - Security Platform for IoT - general information
ATLAS RESFIT consists in a 3-tier IoT security platform which offers the following modules:
* A lightweight software client which runs on the IoT device ([ATLAS_Client])
* A gateway software which runs on the network edge and manages all the clients from the network ([ATLAS_Gateway])
* A cloud platform which allows managing the gateways and the clients ([ATLAS_Cloud])
* An Android management application which allows IoT command authorization ([ATLAS_Android])

ATLAS provides security management for a fleet of IoT devices and enables a reputation based Sensing-as-a-service platform. It also offers the capability to inspect the IoT device telemetry values and supports the CoAP lightweight protocol for the communication between the IoT device and the gateway.
On the IoT data plane layer, ATLAS provides an API which can be integrated with a user application and offers the following capabilities:
* Install a firewall rule on the gateway side
* Send packet statistics to the gateway and cloud
* Get the device with the most trusted reputation within a category and provide reputation feedback

### ATLAS Client
ATLAS Client is a software module which runs on the IoT devices and empowers the integration with the ATLAS Gateway software. This component consists in a lightweight agent which monitors various system parameters and which exposes an API to a custom application (data plane). ATLAS Client API has the following functions:
* Allows a custom application to install an MQTT application layer firewall rule on the gateway side. This firewall rule shapes the IoT device ingress traffic.
* Transmits network statistics to ATLAS Gateway component.
* Allows obtaining the device with the highest reputation within a sensor category (enables a Sensing-as-a-service architecture).
* Allows delivering reputation feedback for an IoT device which publishes data within a sensor category.

ATLAS Gateway installs *push alerts (update data periodically to the gateway)* and *threshold alerts (scan the system frequently and update data to the gateway only when a threshold is passed)* to ATLAS Client side in order to monitor the IoT device state in *real-time*.
ATLAS Client communicates with ATLAS Gateway using CoAPs protocol (secured using DTLS-PSK) and accepts asynchronous CoAPs transported commands from ATLAS Gateway.

### ATLAS Gateway
ATLAS Gateway is a software which runs on the network edge and manages all the IoT clients from the network. It enables the collection of telemetry data from connected IoT nodes (that have ATLAS Client software) and offeres a method of filtering malicious nodes through an integrated reputation based scoring architecture. ATLAS Gateway aggregates data from all the registered IoT nodes and synchronizes the information in real-time with the ATLAS Cloud module. ATLAS Gateway uses the CoAPs protocol to communicate with the downstream IoT nodes and it uses MQTTs protocol to communicate with the upstream cloud component.

### ATLAS Cloud
ATLAS Cloud is a SaaS portal used to manage ATLAS gateways and ATLAS client devices. The portal permits visualizing gateways and IoT client device telemetry data in **real-time**. More specifically, the portal allows the following main functions:
* add gateways to the platform using a unique identity and a pre-shared key
* visualize the gateway connection health
* visualize the IoT clients connected to the gateway
* visualize the IoT client connection health with the gateway
* visualize the IoT client system telemetry data (e.g. number of processes, used memory)
* visualize the IoT client network information: statistics collected from the IoT device and firewall statistics (ingress/egress passed and dropped packets) collected from the gateway MQTT firewall. 
* visualize the IoT device reputation: system reputation (evaluates the client behavior in the system using metrics like connection health and number of packets accepted by other nodes) and sensor reputation (evaluates the quality of sensor generated data).
* inspect the network statistics and reputation history in a real-time updated chart

The ATLAS Cloud portal communicates with the gateways using a TLS secured MQTT protocol and the user-interface web application is accessed by an administrator using a client digital certificate.

### ATLAS Android
ATLAS Android is a smartphone application which allows authorization/approval for IoT client commands.
This application allows adding the owner paradigm to ATLAS RESFIT: the administrator of the IoT network which is a separate entity from the cloud administrator. Thus, the cloud administrator may send commands to the IoT client device, which need approval from the owner.

The first steps in using ATLAS Android is to claim a gateway device, which consists in sharing a secret key between the Android application and the gateway. After the gateway is claimed, it will reject any further claim attempts.

If the cloud application issues a command to the IoT client, ATLAS Android will be notified using a Firebase and it will fetch all pending commands from the cloud.
The approved/rejected commands are cryptographically signed (HMAC) using the secret key shared with the gateway at claim time. If approved, the command will be transmitted to the gateway which cryptographically validates the command and then transmits the command to the IoT client device.

----

## Installation
Each individual component of ATLAS RESFIT has a custom installation script. Please see the *README file* of each component for a detailed installation guide.

----

## Authors
ATLAS RESFIT was developed by:
* Ion Bica
* Bogdan-Cosmin Chifor
* Stefan-Ciprian Arseni
* Mihai Coca
* Mirabela Medvei
* Ioana Cismas

This work was supported by a grant of the Romanian Ministry of Research and Innovation, CCCDI – UEFISCDI, project number PN-III-P1-1.2-PCCDI-2017-0272/ Avant-garde Technology Hub for Advanced Security (ATLAS), within PNCDI III.

----

## License
GNU General Public License v3.0 or later.

See LICENSE file to read the full text.

[ATLAS_Client]: https://github.com/atlas-iot/resfit/tree/master/atlas_client
[ATLAS_Gateway]: https://github.com/atlas-iot/resfit/tree/master/atlas_gateway
[ATLAS_Cloud]: https://github.com/atlas-iot/resfit/tree/master/atlas_cloud
[ATLAS_Mosquitto]: https://github.com/atlas-iot/mosquitto/tree/atlas_plugin
[ATLAS_Android]: https://github.com/atlas-iot/resfit/tree/master/atlas_android
