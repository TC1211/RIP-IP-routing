# RIP/IP Implementation
Router implementation, implementing IP and RIP on top of UDP
## Group Members
Tianchan Dong, Grace Chen
## Implementation Details
#### Main Class
The main class is where the threads are housed; all other classes are accessed by the threads when needed. The main class provides functionality for parsing node files and setting up lists of node interface objects upon startup. Main.c is also responsible for accepting input from the terminal and performing whatever command was entered by the user; the main() method in the main class sets up all necessary tables before entering a while (1) loop to accept commands, as well as periodically check for and send updates on its topography.
#### RIP
The RIP class defines the RIP packet and entry struct used to store information about a neighbor. The RIP class provides functionality for constructing a list of such entries to be encapsulated by an RIP packet.
#### IP
The IP class defines the IP packet, which encapsulates the RIP packet. The IP class provides functionality for constructing an IP packet, for detecting an RIP packet, and for updating the information in the IP header before forwarding (if necessary).
#### UDP Socket
UDP Socket provides functionality for creating and using UDP sockets. The socket is initialized as UDP with the specification of IP and DGRAM stream. The port number and address is passed into the function argument. The node will only support one connection and the while loop will always keep it open and listening once the connection is established. The send and receive message at the moment needs to be defined with packet size. (NEED UPDATE). There is no way to get out of the while loop at the moment (note: add in the down command).
#### ipsum
Code given on the class website.
#### Interfaces
###### RIP IP Interface
The RIP-IP Interface connects the RIP and IP classes at the convenience of the main class, and provides functionality for constructing RIP packets encapsulated by IP headers for routing. This interface also houses the forwarding table used by nodes to determine which VIPs they should use, as well as to hold information on when the latest updates on neighboring nodes were last received.
###### UDP IP Interface

## Sources
StaaackOverfloooooow
