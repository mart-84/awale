# Boucle principale du jeu :
- afficher le plateau
- vérifier si la partie est finie :
    - un des scores est supérieur à 25 points
    - famine d'un joueur
    - plus aucune capture possible (assez compliqué)
    => mettre fin au jeu et afficher le résultat
- jouer un coup
- calculer le nouveau score

# Boucle principale du jeu sur le serveur :
- envoyer le plateau aux deux joueurs
- vérifier si la partie est finie :
    => envoyer les résultats aux joueurs
- attendre que le joueur courant envoie son coup
- jouer le coup
    - si coup invalide, envoyer l'erreur au joueur courant et attendre de nouveau
- calculer le nouveau score