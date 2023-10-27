
# Todos
- Reconnexion :
    - Proposer de reprendre la partie en cours (uniquement si l'autre joueur est toujours connecté)
    - Si le joueur refuse de reprendre : il perd
- A la déconnexion, si les deux joueurs d'une partie sont déconnectés : supprimer le match

- Vérifier le nombre de paramètres pour les commandes

- Voir la liste de ses invitations

- Mettre à jour le /help et le Readme

- Refactoriser le code (adieu la fonction de 150000 lignes o/)

- Ajouter de la musique chez les clients, avec une commande pour la lancer et une commande pour l'arrêter -> dans un autre thread

- Ajouter une commande pour abandonner une partie /giveup
Le joueur qui la fait perd la partie

- Observateur : pouvoir spec une game, il faut d'abord pouvoir afficher la liste des parties et en choisir
    - /listeparties
    - /spec j1 j2

- Utiliser les flèches pour remonter dans les commandes


# Installation

make

# Commandes
Les commandes commencent par le préfixe /
Elles sont sous la forme :
`/command <param1> <param2>`


La liste des commandes est la suivante :
- `/help` : affiche la liste des commandes
- `/listejoueurs` : affiche les autres joueurs connectes
- `/duel <pseudo>` : défier le joueur `<pseudo>` dans un duel
    - `/accepte <pseudo>` : accepter le défi du joueur `<pseudo>`
    - `/refuse <pseudo>` : refuser le défi du joueur `<pseudo>`
- `/bio <pseudo>`: afficher la biographie du joueur `<pseudo>`, si aucun pseudo n'est spécifié, la commande affiche la biographie du joueur connecté
- `/setbio <contenu>` : définit à `<contenu>` la biographie du joueur connecté
- `/msg <pseudo> <message>` : envoie un message privé au joueur `<pseudo>` 
- `/chat <message>` : envoie un message dans le chat général
- `/quit` : fermer le jeu

# Notes
- Un joueur ne peut avoir qu'un seul match en cours en même temps. Si plusieurs invitations sont envoyées, elles peuvent pas toutes être acceptées.