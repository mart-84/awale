
# Todos
- Affichage et édition des biographies
- Reconnexion :
    - Proposer de reprendre la partie en cours (uniquement si l'autre joueur est toujours connecté)
    - Si le joueur refuse de reprendre : il perd
- A la déconnexion, si les deux joueurs d'une partie sont déconnectés : supprimer le match
- Vérifier le nombre de paramètres pour les commandes
- Pour accepter une invitation, vérifier aussi que le joueur n'est pas déjà dans une partie

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
- `/setbio <contenu>` : définit à `<contenu>` la biographie du joueuur connecté
- `/msg <pseudo> <message>` : envoie un message privé au joueur `<pseudo>` 
- `/chat <message>` : envoie un message dans le chat général

# Notes
- Un joueur ne peut avoir qu'un seul match en cours en même temps. Si plusieurs invitations sont envoyées, elles peuvent pas toutes être acceptées.