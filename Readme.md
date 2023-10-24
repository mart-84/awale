
# Todos
- Duel
- Affichage et édition des biographies
- Reconnexion :
    - Proposer de reprendre la partie en cours (uniquement si l'autre joueur est toujours connecté)
    - Si le joueur refuse de reprendre : il perd
- A la déconnexion, si les deux joueurs d'une partie sont déconnectés : supprimer le match


# Commandes
Les commandes commencent par le préfixe /
Elles sont sous la forme :
`/command <param1> <param2>`


La liste des commandes est la suivante :
- `/help` : affiche la liste des commandes
- `/listerjoueurs` : affiche les autres joueurs connectes
- `/defier <pseudo>` : défier le joueur `<pseudo>` dans un duel
    - `/accepter <pseudo>` : accepter le défi du joueur `<pseudo>`
    - `/refuser <pseudo>` : refuser le défi du joueur `<pseudo>`
- `/bio <pseudo>`: afficher la biographie du joueur `<pseudo>`, si aucun pseudo n'est spécifié, la commande affiche la biographie du joueur connecté
- `/setbio <contenu>` : définit à `<contenu>` la biographie du joueuur connecté