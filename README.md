# Projet/TP - Chat (Programmation de sockets en C)

Ceci est un chat avec un serveur centralisé et muti-utilisateurs, chaque utilisateur a un nom d'utilisateur.
Nous avons dévéloppé ce chat à l'aide de sockets.

## Exécution du programme


### Compilation

Pour la compilation du programme, il faut utiliser la commande suivante:

```bash
    make all
```

Pour supprimer les fichiers de compilation, il faut utiliser la commande suivante:

```bash
    make clean
```


### Lancer le programme

#### Serveur

./serveur port_local

#### Client 

./client @adresse-IP port_local username


- Déconnexion:	/quit
	
- Afficher les utilisateurs en ligne: /online

- Envoyer des messages privés: /send message -uusername (sans espace)
	exemple: /send je t'aime -umanon |où manon = username

- Envoyer des messages publics: un message ne correspondant pas à ces commandes sera envoyé publiquement (à tous les utilisateurs).

#### Comment utiliser correctement ce programme

Pour bien utiliser ce programme, il faut suivre les étapes suivantes:

1- Comme dans tout programme serveur/client, il faut exécuter le serveur avant les clients.
2- Pour établir la connexion, les clients doivent se connecter au numéro de port du serveur.

### Auteurs

Charles Parisi - charles.parisi62@gmail.com
Nicolas Marra - nicolasmarra12@gmail.com
