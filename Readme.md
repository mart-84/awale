# TP 2/3/4 - Programmation d’un serveur de Jeu Awalé

Le jeu et le serveur/client ont été développés par Martin Bonnefoy et Arthur Galet.


## Installation et exécution
Pour compiler le client et le serveur, il suffit d'utiliser la commande `make`. Elle construit deux exécutables dans le dossier `bin`.

Pour lancer le serveur, il faut exécuter la commande `./bin/server`. Elle ne prend aucun argument. Le serveur écoute alors sur le port 1977.

Pour lancer un client, il faut exécuter la commande `./bin/client 127.0.0.1 pseudo`. Elle prend deux arguments : l'adresse du serveur et le pseudo du joueur.


## Utilisation du jeu
Les commandes commencent par le préfixe `/`. Elles sont sous la forme :
`/command <param1> <param2>`

Liste des commandes disponibles :
- `/help` : affiche la liste des commandes disponibles
- `/listejoueurs` : affiche les autres joueurs connectes
- `/duel <pseudo>` : défier le joueur `<pseudo>` dans un duel
- `/accepte <pseudo>` : accepter une invitation de duel du joueur `<pseudo>`
- `/refuse <pseudo>` : refuser une invitation de duel du joueur `<pseudo>`
- `/chat <message>` : envoyer un message à tous les joueurs connectés
- `/msg <pseudo> <message>` : envoyer un message au joueur `<pseudo>`
- `/setbio <bio>` : définir votre bio
- `/bio` : afficher votre bio
- `/bio <pseudo>` : afficher la bio du joueur `<pseudo>`
- `/abandonne` : abandonner le match en cours, quitter le match si l'adversaire est déconnecté
- `/listematchs` : afficher la liste des matchs en cours
- `/spec <pseudo1> <pseudo2>` : regarder le match en cours entre `<pseudo1>` et `<pseudo2>`
- `/listeinvitations` : afficher la liste des invitations envoyées et reçues
- `/listesaves` : afficher la liste des parties enregistrées
- `/watchsave <file>` : revisionner la partie enregistrée dans le fichier `<file>`
- `/music enable` : lancer la musique
- `/music disable` : arrêter la musique
- `/quit` : quitter le jeu


## Notes
- Un joueur ne peut avoir qu'un seul match en cours en même temps. Si plusieurs invitations sont envoyées, elles ne peuvent pas toutes être acceptées.
- Si un joueur se déconnecte durant un match, le match est mis en attente. Il peut se reconnecter avec le même pseudo pour reprendre le match.
- Deux joueurs ne peuvent pas se connecter avec le même pseudo en même temps.
- La musique jouée avec les commandes `/music` est dans le fichier `music.mp3`.
- Les sauvegardes des parties sont situées dans le dossier `saves`. Chaque sauvegarde est un fichier binaire nommé au format `<joueur1>_<joueur2>_timestamp.awale`.