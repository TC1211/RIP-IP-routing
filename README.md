# RIP-IP-routing
Router implementing IP and RIP on top of UDP
## UDP Socket
The socket is initialized as UDP with the specification of IP and DGRAM stream. The port number and address is passed into the function argument. The node will only support one connection and the while loop will always keep it open and listening once the connection is established. The send and recive message at the moment needs to be defined with packet size. (NEED UPDATE). There is no way to get out of the while loop at the moment (note: add in the down command).
