# Phone Directory application
Command-line application for managing and querying a phone directory, implemented as a Client-Server system using POSIX sockets. It allows multiple clients to perform operations like adding, deleting, updating, 
and searching for contacts concurrently. The system uses a multithreading approach, leveraging Multiplexing I/O for optimal performance. Informations between server and client are sent in ```JSON``` format. 
For simplicity, the application runs locally, but it can be extended for remote use.

## Compilation instructions
Execute the ```make``` command while inside the project folder (requires a c11 compiler).

## How to use the application
- To launch the application server execute `./server`
- To launch the client application execute `./client 127.0.0.1`

Once the client app is running, follow the instructions in the multi-option menu. The password required to modify the phone directory is ```qwerty```. The following operations are available for the client:
- Add 
- Delete
- Search by prefix
- Search by number
- Update a contact number
  
Datas is saved to a file to allow for repeated use.
