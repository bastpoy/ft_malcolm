# FT_MALCOLM

## EXPLANATION

The goal of this project is to implement man in the middle attack with the langage C.
There is a program who will listen for incoming request on the broadcast by the target IP address requesting the source IP address. The program will send an ARP response to the target. At the end of the program the ARP table of the target should contain the IP source address

## LAUNCH THE PROGRAMM

./malcolm <ip_source> <mac_source> <ip_target> <mac_target>

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

## NETWORK NOTIONS

### Network specifications

- wlp2s0 => interface reseau qui correspond au WIFI
- lo => interface reseau virtuelle qui correspond a la loopback pour les tests et la communications internes
    il a une adresse IP egale a 127.0.0.1

- PORT ARP : 219

### Bash command to communicate

- nc : listen and connection with TCP and UDP (netcat)
    - nc <ip_address> <port>
    - echo "<message>" | nc <ip_address> <port>
- arp -a : get the routing table of a machine