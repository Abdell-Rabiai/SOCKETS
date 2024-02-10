# include <iostream>
# include <string>
# include <vector>
// i wan to create a server that can handle multiple clients in c++
// i will use the poll as the server will be non-blocking
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <poll.h>
# include <cstring>
# include <arpa/inet.h>
# include <fcntl.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/stat.h>

int main() {


    // step 1: create a socket

    // step 2: bind the socket to an ip and port

    // step 3: listen for incoming connections

    // step 4: accept the incoming connections

    // step 5: create a poll

    // step 6: add the server socket to the poll

    // step 7: add the client sockets to the poll

    // step 8: handle the poll events

    // step 9: close the sockets
    return 0;
}

- we need implement mechanisms such as polling or using event-driven architectures
(e.g., using **`select`**, **`poll`**, or `Asynchronous I/O (Input/Output)`) to manage non-blocking sockets effectively.

`Asynchronous I/O (Input/Output)`: 

programming paradigm where operations can be initiated without waiting for their completion and the program is notified asynchronously when the operation finishes.
In traditional synchronous I/O, a program typically waits for each I/O operation to complete before proceeding to the next one instruction , potentially causing the program to block and wait.

And we don't want that, since we want to handle multiple clients at the same time

`multithreading`:
- one way to handle multiple clients is to use multithreading, but it's not the best way to handle multiple clients, because it's not scalable, and it's not efficient.

`multiprocessing`:
- another way to handle multiple clients is to use multiprocessing, but it's not the best way to handle multiple clients, because it's not scalable, and it's not efficient.

`multiplexing`:

- The best way to handle multiple clients is to use multiplexing, which is a way to handle multiple I/O operations at the same time, and it's scalable, and it's efficient.
select, poll, epoll, kqueue, etc. are examples of multiplexing.

`select`:
- The `select` system call allows a program to monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O operation (e.g., input possible).
A file descriptor is considered ready if it is possible to perform the corresponding I/O operation (e.g., read(2)) without blocking.


`poll`:
- The `poll` system call waits for one of a set of file descriptors to become ready to perform I/O.
The set of file descriptors to be monitored is specified in the `fds` argument, which is an array of structures of the following form:

- `struct pollfd {
    int   fd;         // file descriptor
    short events;     // requested events
    short revents;    // returned events
};`

- The `events` and `revents` fields are bit masks that specify the events the caller is interested in and the events that actually occurred.

- The `poll` system call waits until either:

    - one of the file descriptors becomes ready;
    - the call is interrupted by a signal handler; or
    - the timeout expires.

- If the `timeout` argument is not `NULL`, it specifies a maximum interval to wait for the event to occur; if `timeout` is `NULL`, `poll` waits indefinitely.

- The `revents` field is an output parameter, filled by the kernel with the events that actually occurred.

- The `poll` system call is similar to `select`, but more flexible and with better performance on large numbers of file descriptors.



