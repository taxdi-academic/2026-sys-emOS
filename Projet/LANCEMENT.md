# Launch Guide: Registry and Docker Images

This document describes the steps to build images, store them in the local Registry (VM1), and deploy them on both machines.

## 1. Starting the registry (VM1)

```bash
cd ~/ProjetFinal
docker compose down
docker compose up -d registry
```

## 2. Build and push images (VM1)

```bash
cd ~/ProjetFinal

for service in c1_initiator c2_caesar c3_xor c4_decipher; do
    docker build -t $service ./$service
    docker tag $service localhost:5555/$service
    docker push localhost:5555/$service
done
```

## 3. Deployment on VM2

```bash
cd ~/ProjetFinal
docker compose down
docker compose pull
docker compose -f docker-compose.yml up
```

## 4. Launch on VM1

```bash
docker compose -f docker-compose.yml up
```

## 5. Interaction with the project (KEY STEP)

After launching on VM1:

1. **Open a new terminal**
2. Attach the initiator container:
```bash
docker attach c1_initiator
```
3. **Press Enter twice**
4. **Enter the requested words**

This is when user interaction takes place.

---

## Useful Commands

* Initiator container logs (on VM1):
```bash
docker logs -f c1_initiator
```

* Caesar encryption logs (on VM1):
```bash
docker logs -f c2_caesar
```

* XOR container logs (on VM2):
```bash
docker logs -f c3_xor
```

* Decipher container logs (on VM2):
```bash
docker logs -f c4_decipher
```

---

## Notes

- VM1: Linux without graphical interface
- VM2: Linux with graphical interface
- The project relies entirely on Docker, TCP, and simple cryptographic transformations.
