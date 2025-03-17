1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_A typical strategy is for the server to send a special end-of-transmission character (e.g., 0x04) once it has finished sending output. The client reads in a loop and checks for that marker. Handling partial reads is done by accumulating incoming data until the marker is found; this ensures all bytes are received even if they arrive in multiple chunks._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_A networked shell must explicitly frame commands in the stream, for example by sending a null terminator ('\0') after each command or by sending a length field first. The client then reads until it finds the terminator or has read the stated length. Without this, commands can merge or split arbitrarily in the stream, causing data corruption_

3. Describe the general differences between stateful and stateless protocols.

_Stateful protocols remember past requests and maintain session context (e.g., FTP). Stateless protocols treat each request independently, with no stored context between them (e.g., simple HTTP)._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_UDP is lightweight and low-latency, making it ideal for real-time applications such as gaming that can tolerate lost packets but need speed and reduced overhead._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_The operating system provides sockets, which let applications open connections (TCP or UDP) and exchange data using familiar calls like send() and recv()._