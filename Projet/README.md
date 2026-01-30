# Project: Exquisite Corpse

## 1. Project Goal

The objective is to pass a word through 4 containers distributed across 2 Proxmox VMs, applying cryptographic transformations and final validation.
Flow Diagram

    VM1 - C1 (Sourcing): User Input → TCP Send.

    VM1 - C2 (Encrypting): Caesar (+5) → TCP Send (via Vmbr1 Bridge).

    VM2 - C3 (Encrypting): XOR ("Nirvana") → TCP Send.

    VM2 - C4 (Decrypting): XOR ("Nirvana") + Caesar (−5) → TCP Send.

## 2. Container Technical Details

| Step | Location | Container | Main Action | Transformation / Protocol | Destination |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1. Source** | **VM1** | `c1_initiator` | Word selection | User input | `c2_caesar` (VM1) |
| **2. Caesar Encryption** | **VM1** | `c2_caesar` | Encryption | Caesar (+5) | `c3_xor` (VM2) |
| **4. XOR Encryption**| **VM2** | `c3_xor` | Encryption | XOR ("nirvana") | `c4_decipher` (VM2) |
| **5. Decryption + send**| **VM2** | `c4_decipher` | Decryption + send | XOR("nirvana") -> Caesar(-5) | `c1_initiator` (VM1) |

---

There are 4 relays in total:

| Service | Listening Port | Protocol | Data Type |
| :--- | :--- | :--- | :--- |
| **Clear relay** | `5000` | TCP | Binary (Bytes) |
| **Caesar encrypted relay** | `5001` | TCP | String (String) |
| **XOR encrypted relay** | `5002` | TCP | String (String) |
| **Clear decrypted return** | `5003` | TCP | Binary (Bytes) |

---

-----------------------------------------------------------------

## 3. Communication Logic (TCP Protocol)

Although UDP is possible, TCP is recommended here because XOR encryption on binary streams does not handle packet loss or disorder well.

---

## Notes

- VM1: Linux without graphical interface
- VM2: Linux with graphical interface
- The project relies entirely on Docker, TCP, and simple cryptographic transformations.


