# Projet : Cadavre Exquis

## 1. But du projet

L'objectif est de faire transiter un mot à travers 4 containers répartis sur 2 VMs Proxmox, en appliquant des transformations cryptographiques et une validation finale.
Schéma de flux

    VM1 - C1 (Sourcing) : Pioche un mot → Envoi TCP.

    VM1 - C2 (Crypting) : César (+5) → Envoi TCP (via Bridge Vmbr1).

    VM2 - C3 (Crypting) : XOR ("Nirvana") → Envoi TCP.

    VM2 - C4 (Decrypting) : XOR ("Nirvana") + César (−5) → Envoi TCP.

## 2. Détails Techniques des Containers

| Étape | Localisation | Container | Action Principale | Transformation / Protocole | Destination |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1. Source** | **VM1** | `c1_initiator` | Sélection du mot | Lecture `dict.txt` (Texte clair) | `c2_caesar` (VM1) |
| **2. Chiffrement Cesar** | **VM1** | `c2_caesar` | Chiffrement | César (+5) | `c3_xor` (VM2) |
| **3. Envoie** | **Bridge** | `vmbr1` | Routage réseau | TCP (Paquets binaires) | VM2 |
| **4. Chiffrement XOR**| **VM2** | `c3_xor` | Chiffrement | XOR ("nirvana") | `c4_decipher` (VM2) |
| **5. Déchiffrement + envoie**| **VM2** | `c4_decipher` | Déchiffrement + envoie | XOR("nirvana") -> César(-5) | `c1_initiator` (VM1) |

-----------------------------------------------------------------
Il y a en tout 4 relais : 

| Service | Port Écouté | Protocole | Type de Donnée |
| :--- | :--- | :--- | :--- |
| **Relais clair** | `5000` | TCP | Binaire (Bytes) |
| **Relais chiffré Cesar** | `5001` | TCP | Chaîne (String) |
| **Relais chiffré XOR** | `5002` | TCP | Chaîne (String) |
| **Retour déchiffré en clair** | `5003` | TCP | Binaire (Bytes) |

-----------------------------------------------------------------

## 3. Logique de Communication (Protocole TCP)

Bien que l'UDP soit possible, le TCP est recommandé ici car le chiffrement XOR sur des flux binaires supporte mal la perte de paquets ou le désordre.

---

**Notes :** Dans ce projet, la VM1 correspond à la VM sous linux sans GUI. La VM2 quand à elle, correspond à la VM linux avec GUI


