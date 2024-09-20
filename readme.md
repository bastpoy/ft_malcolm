# FT_MALCOLM

## EXPLANATION

The goal of this project is to implement man in the middle attack with the langage C.
There is a program who will listen for incoming request source ip address. And then it will send a ARP message to the target ip address. then the routing table of the target IP address will be update with the fraudulent ip adress and not the real source IP address.

TCP: 
- protocole utilise pour naviguer sur internet (HTTP/HTTPS) aussi pour le SMTP avec une gestion des paquets.
- Mise en place de la communication avec l'envoie de paquet au prealable pour etablir la communication
- le TCP est dit connecte

UDP
- protocole sans gestion des paquets. Protocole plus rapide que le TCP et permet de charger un flux video par exemple
- Pas dacquittement lors de l'envoie et de la reception des paquets
- l'UDP est dit non connecte

SOCKET
- un socket est le moyen de communiquer entre un client et un serveur generalement. Il offre des informations sur le client ou le serveur a contacter (adresse IP)

## STRUCTURES

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

struct sockaddr_in {
   uint8_t         sin_len;       /* longueur totale      */
   sa_family_t     sin_family;    /* famille : AF_INET     */
   in_port_t       sin_port;      /* le numéro de port    */
   struct in_addr  sin_addr;      /* l'adresse internet   */
   unsigned char   sin_zero[8];   /* un champ de 8 zéros  */
};

struct sockaddr {
   unsigned char   sa_len;         /* longueur totale         */
   sa_family_t     sa_family;      /* famille d'adresse     */
   char            sa_data[14];    /* valeur de l'adresse    */
};

struct in_addr {
   in_addr_t    s_addr;
};

struct hostent {
   char    *h_name;       /* Nom officiel de l'hôte.   */
   char   **h_aliases;    /* Liste d'alias.            */
   int      h_addrtype;   /* Type d'adresse de l'hôte. */
   int      h_length;     /* Longueur de l'adresse.    */
   char   **h_addr_list;  /* Liste d'adresses.         */
}

## NOTIONS RESEAUX

- wlp2s0 => interface reseau qui correspond au WIFI
- lo => interface reseau virtuelle qui correspond a la loopback pour les tests et la communications internes
    il a une adresse IP egale a 127.0.0.1


## FUNCTIONS AUTORISEES

- sendto(): send data over a socket
    - ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    - principalement utilise en connectionless socket, ou chaque paquet est independant les uns des autrese (UDP)
- recvfrom(): l'inverse de sendto et permet de recevoir un packet
- socket: permet de creer un socket
    - preciser le type d'adresse
    - le type de communication (TCP, UDP)
    - protocole : un protocole specifique a mettre en oeuvre (peut ne pas en avoir)
- setsocketopt(): permet de specifier des options liees au fd du socket
- if_nametoindex(): permet de recuperer l'index d'une interface reseau grace a son index
    - elle retourne donc un index
    - elle prend en parametre une chaine de caractere correspondant au nom de l'interface
- getuid():récupérer l'id de l'utilisateur
- signal et sigaction sont des fonctions qui permettent de gerer les signaux de retour d'une action.
- gethostbyname: recuperer des informations sur un domaine en envoyant un URL
    - Un peu dépassé car cette fonction ne prend pas en compte les addresses IPV6
- getaddreinfo: Prend en argument le nom de domaine a resoudre et des structures pour stocker le résultat.
- freeaddrinfo: libère la mémoire allouée par getaddrinfo
- getifaddrs: récupérer les interfaces réseaux disponibles sur une machine
    - Elle prend en paramètre un vers un pointeur de structure ou va être stocker le résultat
- freeifaddrs: free le pointeur alloué par getifaddrs
- htons et ntohs: convertisse les octet de l'hote vers l'octet du réseau.
    - Parce que l'ordre d'octets du réseau est normé.
    - Enfaite ce qu'il va faire c'est convertir en inversant le sens des bits