# Battleship

protocole.h
Il s’agit d’un fichier d’en-tête qui définit certaines constantes utilisées dans les autres fichiers. Cela inclut la taille de la mémoire tampon, les préfixes de commande, les résultats et les détails sur les navires du jeu (nombre de navires, tailles, taille maximale).
game_grid.h
Il s’agit d’un fichier d’en-tête qui déclare les fonctions de gestion de la grille de jeu. Les fonctions comprennent l’initialisation de la grille, le placement d’un navire sur la grille, l’initialisation de la grille avec les navires, l’affichage de la grille, l’enregistrement d’un impact de bombe sur la grille et la vérification du résultat d’un impact de bombe.
serveur.c
Il s’agit du programme côté serveur. Il configure un serveur capable de gérer les connexions de deux clients. Il met en place deux grilles de jeu (une pour chaque client) et écoute les commandes de déplacement des clients. Lorsqu’il reçoit une commande de déplacement, il vérifie le résultat du déplacement et le communique au client. Si les navires d’un client ont tous été coulés, il met fin au jeu et annonce le gagnant.
Les principaux éléments de ce programme sont les suivants :
1.	handle_client() : Cette fonction est destinée à être exécutée sur un thread distinct pour chaque client. Il attend les messages de son client et les traite en conséquence.
2.	main() : Cette fonction configure le serveur et attend que les clients se connectent. Lorsqu’un client se connecte, il génère un nouveau thread pour gérer ce client.
3.	Mémoire partagée (shm) : Le serveur utilise la mémoire partagée pour communiquer l’état initial de la grille de jeu à chaque client.
client.c
Il s’agit du programme côté client. Il se connecte au serveur et communique avec lui pour jouer au jeu. Il attend les résultats du serveur et envoie des commandes de déplacement en réponse. Il mappe également l’état initial de la grille de jeu à partir de la mémoire partagée.
Les principaux éléments de ce programme sont les suivants :
1.	main() : Cette fonction se connecte au serveur, obtient l’état initial de la grille de jeu et entre dans une boucle de réception des résultats et d’envoi de mouvements.
2.	Mémoire partagée (shm) : le client utilise la mémoire partagée pour obtenir l’état initial de la grille de jeu à partir du serveur.
game_grid.c
Ce fichier définit les fonctions déclarées dans game_grid.h. Ces fonctions incluent init_grid(), place_ship(), init_grid_with_ships(), display_grid()   ,  record_bomb() et check_bomb(). Ensemble, ces fonctions sont utilisées pour gérer la grille de jeu.
