1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully recieved by checking for the EOF character. If there is an EOF than it is the end. In order to ensure complete message transition, you must receive the EOF character. For handling partial reads, you can simply output the number of bytes recieved. 

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

The networked shell must utilize flags to look for when sending/receiving. As mentioned earlier the client receives a stream ending with an EOF. The client also sends a null terminated string to the server. Without defined message boundaries, the shell can run into parsing issues. The buffer can be mixed together and become out of order with only partial bits of commands being sent, commands can get merged, and overall, it will be hard to have a functioning shell really.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol keeps track of data, such as user data and logs, and session information. Stateless however, do not keep track of any data, and all needed data must be sent in the request.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

As mentioned in the lecture, it isn't necessarily fair to call UDP "unreliable". The example given in class was thinking of it like UPS, where sure sometimes they do lose a package, but it is not very often. UDP is considerable faster, which allows it to be good for streaming, where losing a few packets won't be detrimental. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the Socket interface, which includes modules like accept, connect, bind, listen, etc., for network communication.