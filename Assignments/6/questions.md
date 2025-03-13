1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client determines the output is fully received by reading an end byte, we define as RDSH_EOF_CHAR. This signals to the client that the message has been transmitted. To handle partial reads, or ensure complete transmission, one can use unique bytes to flag as EOF or end of message.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

This can be done by using an EOF marker, that flags that the stream of bytes is done.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain relevance througout different requests. It retains information and can be used for tracking or when multiple requests relate. Stateless protocols are independent requests. One reqeuest is not relevant to the previous or the next. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP does not guarente delivery or order, which is unlike TCP. It also does not guarentee error correction. It can be much faster and efficient if that is what matters. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Sockets! The sockets API allows for network communications.