# Récapitulatif Technique : Projet Cadavre Exquis

Ce projet implémente une chaîne de traitement de données distribuée (C1 -> C2 -> C3 -> C4 -> C1) répartie sur deux machines virtuelles.

## 1. Arborescence des Fichiers (VM1)
L'intégralité du code source et des fichiers de configuration Docker se trouve sur la VM1.
```text
/root/ProjetFinal/
├── docker-compose.yml       # Orchestration Registry, C1 et C2
├── c1_initiator/
│   ├── main.c               # Choix du mot et réception finale
│   ├── dico.txt             # Liste des mots sources
│   └── dockerfile           # Image basée sur gcc:latest
├── c2_caesar/
│   ├── main.c               # Application du code César (+5)
│   └── dockerfile
├── c3_xor/
│   ├── main.c               # Application du XOR (Clé: Nirvana)
│   └── dockerfile
└── c4_decipher/
    ├── main.c               # Déchiffrement inverse (XOR puis César)
    └── dockerfile
```
## 2. Infrastructure Docker Registry

Un Registry privé a été mis en place pour permettre le transfert des images de la VM1 vers la VM2.

- **Service** : `registry:2`
- **Port Hôte** : `5555` (redirigé vers le port `5000` interne du conteneur)
- **Accès** :
  - `localhost:5555` (depuis VM1)
  - `192.168.200.24:5555` (depuis VM2)

---

## 3. Configuration des Orchestrateurs (`docker-compose.yml`)

### Sur VM1 (Build & Relais Initiaux)

```yaml
services:
  registry:
    image: registry:2
    ports:
      - "5555:5000"
    restart: always

  c1_initiator:
    build: ./c1_initiator
    image: localhost:5555/c1_initiator
    container_name: c1_initiator
    ports:
      - "5003:5003"
    depends_on:
      - c2_caesar

  c2_caesar:
    build: ./c2_caesar
    image: localhost:5555/c2_caesar
    container_name: c2_caesar
    ports:
      - "5000:5000"
```

### Sur VM2 (Relais de Traitement)

```yaml
services:
  c3_xor:
    image: 192.168.200.24:5555/c3_xor
    container_name: c3_xor
    ports:
      - "5001:5001"

  c4_decipher:
    image: 192.168.200.24:5555/c4_decipher
    container_name: c4_decipher
    ports:
      - "5002:5002"
```

## 4. Configuration Réseau du Démon Docker

Pour autoriser la communication HTTP vers le Registry, le fichier `/etc/docker/daemon.json` a été modifié sur les deux VMs :

```json
{
  "insecure-registries": ["192.168.200.24:5555"]
}
