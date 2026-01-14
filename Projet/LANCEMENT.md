# Guide de Lancement : Registry et Images Docker

Ce document décrit les étapes pour compiler les images, les stocker dans le Registry local (VM1) et les déployer sur les deux machines.

## 1. Démarrage du registry (VM1)

```bash
cd ~/ProjetFinal
docker compose down
docker compose up -d registry
```

## 2. Build et push des images (VM1)

```bash
cd ~/ProjetFinal

for service in c1_initiator c2_caesar c3_xor c4_decipher; do
    docker build -t $service ./$service
    docker tag $service localhost:5555/$service
    docker push localhost:5555/$service
done
```

## 3. Déploiement sur la VM2

```bash
cd ~/ProjetFinal
docker compose down
docker compose pull
docker compose -f docker-compose.yml up
```

## 4. Lancement sur la VM1

```bash
docker compose -f docker-compose.yml up
```

## 5. Interaction avec le projet (ÉTAPE CLÉ)

Après le lancement sur la VM1 :

1. **Ouvrir un nouveau terminal**
2. Attacher le conteneur initiateur :
```bash
docker attach c1_initiator
```
3. **Appuyer deux fois sur Entrée**
4. **Saisir les mots demandés**

C’est à ce moment précis que l’interaction utilisateur a lieu.

---

## Commandes Utiles

* Logs du conteneur initiateur (sur la VM1) :
```bash
docker logs -f c1_initiator
```

* Logs du chiffrement César (sur la VM1) :
```bash
docker logs -f c2_caesar
```

* Logs du conteneur xor (sur la VM2) : 
```bash
docker logs -f c3_xor
```

* Logs du conteneur decipher (sur la VM2) : 
```bash
docker logs -f c4_decipher
```

---

## Notes

- VM1 : Linux sans interface graphique
- VM2 : Linux avec interface graphique
- Le projet repose entièrement sur Docker, TCP et des transformations cryptographiques simples.
