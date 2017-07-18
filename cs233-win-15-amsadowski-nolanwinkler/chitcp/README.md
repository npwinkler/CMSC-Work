CMSC 23300 - Networks and Distributed Systems
University of Chicago

Nolan Winkler and Aidan Sadowski

This is an implementation of the TCP protocol using finite state machines to complete the simple, testable TCP stack for the chiTCP project, whose documentation is available at http://chi.cs.uchicago.edu/chitcp/

Project breakdown:
We started from RFC 793 and it was given to us that we should treat TCP as a finite state machine, which we did by implementing functions that handle each user event (i.e. handler functions for each possible TCP state which transition to a new one based on what the user does).
