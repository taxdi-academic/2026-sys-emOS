# Guide de Lancement : Registry et Images Docker

Ce document décrit les étapes pour compiler les images, les stocker dans le Registry local (VM1) et les déployer sur les deux machines.

## 1. Initialisation du Registry (VM1)

Avant de pouvoir stocker des images, le service Registry doit être actif.
```bash
cd ~/ProjetFinal
# Arrêt propre de l'existant
docker compose down
# Lancement du Registry sur le port 5555
docker compose up -d registry
```

## 2. Compilation et Publication des Images (VM1)

Nous utilisons une boucle pour automatiser le build, le tag et le push des 4 services vers le registry local.

```bash
# Se placer à la racine de ProjetFinal
cd ~/ProjetFinal

# Boucle de déploiement des images
for service in c1_initiator c2_caesar c3_xor c4_decipher; do
    echo "--- Traitement de : $service ---"
    # 1. Build de l'image à partir du dossier correspondant
    docker build -t $service ./$service
    # 2. Marquage (Tag) de l'image pour le registry local (port 5555)
    docker tag $service localhost:5555/$service
    # 3. Envoi (Push) vers le registry
    docker push localhost:5555/$service
done

# Vérification du catalogue
curl http://localhost:5555/v2/_catalog
```
## 3. Lancement des Services Locaux (VM1)

Une fois les images dans le registry, lancez les deux premiers maillons de la chaîne : ```docker compose up -d c1_initiator c2_caesar```

## 4. Déploiement à distance (VM2)

Sur la VM2, nous téléchargeons les images finies depuis la VM1.

```bash
cd ~/ProjetFinal
# Suppression des anciens conteneurs
docker compose down
# Récupération des nouvelles images via le réseau
docker compose pull
# Lancement de C3 et C4
docker compose up -d
```

## 5. Commandes de Diagnostic (Logs)

Pour surveiller le passage du "Mot choisi" à travers les conteneurs :

* Voir le flux C1 : ```docker logs -f c1_initiator```
* Voir le flux C2 : ```docker logs -f c2_caesar```
* Relancer la chaîne : ```docker compose restart c1_initiator```
