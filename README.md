# RIP/IP Implementation
Router implementation, implementing IP and RIP on top of UDP
## Group Members
Tianchan Dong, Grace Chen
## Implementation Details
#### Main Class
The main class has knowedlge of all the interfaces and thus able to call all functionality. The main class provides functionality for parsing node files and setting up lists of node interface objects upon startup. Main.c is also responsible for accepting input from the terminal and performing whatever command was entered by the user; the main() method in the main class sets up all necessary tables before entering a while (1) loop to accept commands, as well as periodically check for and send updates on its topography.
##### Main
This method initializes the router and creates threads for all its socket interfaces. It then creates a time thread to control the period updates and deltions. Then main will enter an infinte while loop to accept user commands.
#####  parse_file
Opens and reads the file given by user command input and parses each line into information stored in node_interface. It also establishes a global count int which tracks how many neighbours this router has. node_interface struct is given by:
>typedef struct node_interface { 
    >int id; 
    >int port; 
    >char ipAddr[MAX_CHAR]; 
    >char vipThis[MAX_CHAR]; 
    >char vipRemote[MAX_CHAR]; 
    >char status[MAX_CHAR]; //up by default 
>} node_interface; 
##### create_listening_sock
Loop through the created address table and creates a socket which listens for each of the neighbouring interface. There will be one socket which binds to the router's address. The socket will have one child thread for listening on each of the router's neighbour interface.
#####receive_func
This is the function that each receiving thread will execute. Here the socket will first eastablish the listen. Once a packet is received, the packet will be deserialized, the IP header checksum will be checked and if valid, the IP destination address will be examined to determine further forwarding or local delivery. Furthering forwarding will use a table lookup to obtain next hop address and be sent off. Local forwarding will check IP protocol and if RIP 
(200) is detected, will be parsed by command. A request will trigger an atuomatic send with SHRP protocol in place and a response will result in updating the fowarding table. The infinite loop will return again to listen for more packets.
#####construct_flood_entries
Creates the entries list for an initial RIP packet based on the information in the forwarding table. Only create entries of neighbours with cost 1.
#####construct_and_send_IP
Takes in RIP fields and the destination address of the packet and first creates the RIP packet in the interface, serializes it, encapsulates the serialized buffer in an IP packet. The IP packet is also serialized and then the entire packet is sent.
#####initial_flood
This function establishes the flooding that occurs at powering on of a router interface.
#### RIP
The RIP class defines the RIP packet and entry struct used to store information about a neighbor. The RIP class provides functionality for constructing a list of such entries to be encapsulated by an RIP packet. The RIP class also provides functionality for serializing and de-serializing RIP packets because structs do not maintain their data when they are sent over sockets.
#### IP
The IP class defines the IP packet, which encapsulates the RIP packet. The IP class provides functionality for constructing an IP packet, for detecting an RIP packet, and for updating the information in the IP header before forwarding (if necessary). The IP class also provides functionality for serializing and de-serializing IP packets to avoid the struct sending problem mentioned above.
#### UDP Socket
UDP Socket provides functionality for creating and using UDP sockets. This class has methods for creating sockets, binding to sockets, sending over a socket, and receiving over a socket. This class is only responsible for setting up and using sockets. 
#### ipsum
Code given on the class website.
#### Interfaces
###### RIP IP Interface
The RIP-IP Interface connects the RIP and IP classes at the convenience of the main class, and provides functionality for constructing RIP packets encapsulated by IP headers for routing. This interface also houses the forwarding table used by nodes to determine which VIPs they should use, as well as to hold information on when the latest updates on neighboring nodes were last received.
###### UDP IP Interface
The UDP-IP Interface has functionality for serializing and de-serializing IP packets as well as a method for sending bytes in order.
## Sources
StaaackOverfloooooow
