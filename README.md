# Battleships TUI

A classic two-player naval strategy game implemented with socket-based networking (one client and one server). Players deploy fleets on a grid and take turns attacking coordinates; the system tracks hits and misses in real-time, declaring a winner when one fleet is completely destroyed.

---

## ⚙️ Prerequisites

- Linux/Unix-based operating system  
- GCC (GNU Compiler Collection)  
- Make build automation tool  
- Zero external dependencies (uses standard POSIX sockets and ANSI escape codes)

---

## 🚀 How to Build and Run

### Local Play (Same Machine)

#### 1. Compile the project

Navigate to the root directory and build the executables:

```bash
make
```

#### 2. Start the Server (Player 1)

Open the first terminal and start the host. It will bind to port 8080:

```bash
./server/server.out
```

#### 3. Start the Client (Player 2)

Open a second terminal and connect to the local server:

```bash
./client/client.out
```

---

### Network Play (Different Machines)

#### 1. Update the IP address

Open `client/client.c` and change the `SERVER_IP` macro to the host's LAN IP address:

```c
#define SERVER_IP "192.168.1.15"
```

#### 2. Recompile and Run

Run:

```bash
make
```

Then:

- Start `./server/server.out` on the host machine  
- Start `./client/client.out` on the connecting machine  

---

## 🎮 Gameplay Guide

The Terminal UI updates automatically, showing:

- Your fleet on the left  
- Your radar on the right  


## 🧹 Cleanup

To remove the compiled binaries and clean the working directory, run:

```bash
make clean
```