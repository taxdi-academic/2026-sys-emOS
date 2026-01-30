# STM32 Build

## Prerequisites

To complete this section, we will need the following tools:
  - Docker
  - Socat
    - Useful for connecting applications inside separate containers
  - ssh (to facilitate VM usage)
      - Allows easier copy-paste operations
  - GIT (which will allow us to use the resources for this lab)

We will start by cloning the git repository, which contains the resources for the lab, with the command:
```
git clone https://forgens.univ-ubs.fr/gitlab/charton/virtos.git
```
Finally, we will go to the **/virtos/Ressource_OS_STM32/docker** folder to build the stm32

-------------------------------------------------------------------------------------------------------------------------------------

## BUILD PlatformIO CONTAINER

```
docker build -t stm32-qemu .
```
Once the container is created, we will use the network to connect to this container via port 4444. We will also mount the entire resources directory on it.

```
docker run --name stm32 -it -v $(pwd)/..:/workspace -p 4444:4444 stm32-qemu
```
Once the container is exited, it is possible to return to it using the command:

```
docker start -ai stm32
```
-------------------------------------------------------------------------------------------------------------------------------------

## BUILD STM32 Firmware (to be done from the container)
We will now build the stm32 firmware from the installed container. To do this, we will go to the **/OS-FunctionPrograms** folder and run the following command:

```
 pio run -t clean && pio run
```
A message displaying "LED Off" will appear if everything goes well.

-------------------------------------------------------------------------------------------------------------------------------------

## STM32 Access (from the host machine)

To access the STM32 from the host machine, we will execute the following command:

```
socat -,raw,echo=0 tcp:localhost:4444
```
