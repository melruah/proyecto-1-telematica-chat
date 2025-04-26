# proyecto-1-telematica-chat
Chat Cliente-Servidor en C

# Introducción

Este proyecto consiste en un sistema de chat básico desarrollado en lenguaje C, siguiendo el modelo cliente-servidor. El objetivo era permitir que varios clientes se conectaran a un servidor y pudieran intercambiar mensajes en tiempo real a través de una red. Se utilizó programación con sockets TCP y se implementó el servidor en una máquina virtual en la nube (AWS EC2), mientras que el cliente se ejecutó desde una máquina virtual local (VirtualBox).

# Desarrollo
Se realizaron dos programas principales:

server.c: Código del servidor. Este fue cargado y ejecutado en una instancia de AWS EC2 con Ubuntu, escuchando en el puerto 8888 para aceptar conexiones entrantes. Se utilizó pthread para atender múltiples clientes de forma concurrente.

client.c: Código del cliente. Se ejecutó en una máquina virtual local (VirtualBox) con Ubuntu. El cliente solicita el nombre del usuario, establece la conexión con el servidor y permite enviar y recibir mensajes.

Se realizaron configuraciones adicionales como la apertura del puerto 8888 en el grupo de seguridad de AWS y la conexión en modo puente en la red de VirtualBox. Se verificó la conectividad mediante herramientas como ping, netstat y ss.

# Aspectos logrados y no logrados
 # Logros
Compilación y ejecución correcta de los códigos del servidor y el cliente.

Despliegue del servidor en AWS con escucha activa en el puerto 8888.

Verificación de conectividad desde la máquina virtual hacia AWS usando ping.

Implementación de comunicación multicliente con hilos (pthread).

Comprobación del estado del puerto y conexión con herramientas de red.

# Aspectos no logrados
No se logró establecer comunicación completa entre el cliente (VirtualBox) y el servidor (AWS).

Aunque no hubo errores visibles, el servidor no recibió los datos enviados por el cliente.

Se realizaron múltiples pruebas y configuraciones, pero la conexión nunca fue completamente exitosa.

# Conclusiones
El proyecto permitió entender de forma práctica cómo funciona la comunicación entre programas en red usando sockets en C. También se aprendió a desplegar servicios en la nube, a trabajar con redes virtuales y a diagnosticar problemas de conectividad. Aunque la conexión cliente-servidor no se estableció al 100.


# pruebas que hice :
# en client.c
ping 13.58.224.87
host 13.58.224.87
nslookup 13.58.224.87
telnet 13.58.224.87 8888
curl 13.58.224.87:8888

# en server.c

# Ver procesos escuchando en el puerto 8888
sudo netstat -tulnp | grep 8888
ss -tulnp | grep 8888
# Ver estado del firewall (ufw deshabilitado)
sudo ufw status
# Ver uso del puerto
lsof -i :8888
# Ver interfaces de red
ip a

# Referencias

Beej’s Guide to Network Programming – https://beej.us/guide/bgnet/html/
Man Pages – https://man7.org/linux/man-pages/
Repositorio base del proyecto CSocket – https://github.com/masoudy/CSocket
Documentación de AWS EC2 – https://docs.aws.amazon.com/ec2/


