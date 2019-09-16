# Programming Assignment Three - Operating Systems

Created thread-safe and reentrant code to solve the "Bounded Buffer" problem. Requester threads read in from name files and placed host names into a bounded buffer, and resolver threads read in from the bounded buffer and performed dnslookups to compute IP addresses. Both thread pools wrote output to shared files.

## File Descriptions

* **multi-lookup.c** - *Primary Code* - Takes in user input, checks for errors, creates threads and assigns them tasks. Provides code for both requester and resolver threads in their respective functions

* **multi-lookup.h** - *Header File* - Provides function prototypes and struct definition  

* **Makefile** - Builds the multi-lookup program. There are different options within the file, but "make all" deletes and recreates all generated files.

* **performance.txt** - *Performance Metrics* - Provides a few performance metrics based on running the function with different combinations of requester and resolver threads


## Instructions for Building and Running
* Navigate to the directory in which the multi-lookup.c function is located.
* Compile using the command: **make all**
* Run with the command: **./multi-lookup "num of requesters" "num of resolvers" "requester log" "resolver log" "[datafile ...]"**
* View results in requester log and resolver log files


### Emily Millican - Emily.Millican@Colorado.Edu

