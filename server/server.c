#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "listes/listeClient.h"
#include "listes/listeMatch.h"
#include "listes/listeInvitation.h"
#include "match.h"
#include "awale/awale.h"
#include "sauvegarde.h"

static void init_server(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if (err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   printf("Démarrage du serveur sur le port %d...\n", PORT);
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   elementListeClient *listeClients = NULL;
   elementListeMatch *listeMatchs = NULL;
   elementListeInvitation *listeInvitations = NULL;

   fd_set rdfs;

   printf("En attente de connexion...\n");

   while (1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      elementListeClient *ptr = listeClients;
      while (ptr != NULL)
      {
         if (ptr->client->isConnected)
         {
            FD_SET(ptr->client->sock, &rdfs);
         }

         ptr = ptr->suivant;
      }

      if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if (FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = {0};
         socklen_t sinsize = sizeof csin;
         SOCKET clientSock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if (clientSock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(clientSock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         Client *c = rechercherClientParNom(listeClients, buffer);
         if (c != NULL)
         {
            if (c->isConnected)
            {
               printf("Client %s already connected\n", buffer);
               write_client(clientSock, "Ce pseudo est déjà utilisé.\n");
               end_connection(clientSock);
            }
            else
            {
               printf("Client %s reconnected\n", buffer);
               c->isConnected = 1;
               c->sock = clientSock;
               actual++;
               send_welcome_message(c);

               // vérifier si le joueur a un match en cours
               MatchAwale *match = rechercherMatchClient(listeMatchs, c);
               if (match != NULL)
               {
                  Client *adversaire = match->joueur1 == c ? match->joueur2 : match->joueur1;
                  // afficher un message au joueur puis lui envoyer le plateau
                  buffer[0] = 0;
                  strncat(buffer, "Vous avez été déconnecté pendant un match contre ", BUF_SIZE - strlen(buffer) - 1);
                  strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                  strncat(buffer, ". Le match reprend !\n", BUF_SIZE - strlen(buffer) - 1);
                  write_client(c->sock, buffer);

                  // prévenir l'adversaire de la reconnexion
                  buffer[0] = 0;
                  strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                  strncat(buffer, c->name, BUF_SIZE - strlen(buffer) - 1);
                  strncat(buffer, " s'est reconnecté. Le match reprend !\n", BUF_SIZE - strlen(buffer) - 1);
                  write_client(adversaire->sock, buffer);

                  // envoyer le plateau aux deux joueurs
                  buffer[0] = 0;
                  sprintBoard(match->partie, buffer, match->joueur1->name, match->joueur2->name);
                  write_client(c->sock, buffer);
                  write_client(adversaire->sock, buffer);
               }
            }
            continue;
         }

         /* what is the new maximum fd ? */
         max = clientSock > max ? clientSock : max;

         FD_SET(clientSock, &rdfs);

         c = malloc(sizeof(Client));
         c->sock = clientSock;
         strncpy(c->name, buffer, BUF_SIZE - 1);
         c->isConnected = 1;
         listeClients = ajouterClient(listeClients, c);
         actual++;
         printf("New client connected as %s\n", buffer);
         send_welcome_message(c);
      }
      else
      {
         elementListeClient *ptr = listeClients;
         while (ptr != NULL)
         {
            if (ptr->client->isConnected == 0)
            {
               ptr = ptr->suivant;
               continue;
            }
            Client *client = ptr->client;

            ptr = ptr->suivant;

            /* a client is talking */
            if (FD_ISSET(client->sock, &rdfs))
            {
               int c = read_client(client->sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  // récuperer le match associé au joueur
                  MatchAwale *match = rechercherMatchClient(listeMatchs, client);
                  if (match != NULL)
                  {
                     // si l'adversaire est déconnecté, supprimer le match
                     Client *adversaire = match->joueur1 == client ? match->joueur2 : match->joueur1;
                     if (adversaire->isConnected == 0)
                     {
                        listeMatchs = supprimerMatch(listeMatchs, match, 0);
                        printf("Match supprimé suite à la déconnexion des deux joueurs\n");
                     }
                     else
                     {
                        // envoyer un message à l'adversaire
                        buffer[0] = 0;
                        strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, " s'est déconnecté. Le match reprendra quand il sera de nouveau connecté.\n", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, "Vous pouvez utiliser la commnande /abandonne pour arrêter le match.\n", BUF_SIZE - strlen(buffer) - 1);
                        write_client(adversaire->sock, buffer);
                     }
                  }
                  closesocket(client->sock);
                  remove_client(listeClients, *client);
                  actual--;
                  printf("Client %s déconnecté\n", client->name);
               }
               else if (buffer[0] == '/')
               {
                  // char *commande = strtok(buffer, " \n");
                  char **parametres = NULL;
                  int nbParams = tokeniserChaineDeCaracteres(buffer, ' ', &parametres);

                  if (nbParams == 0)
                  {
                     continue;
                  }

                  char *commande = parametres[0];

                  printf("client %s : %s\n", client->name, buffer);
                  if (strcmp(commande, "/help") == 0)
                  {
                     write_client(client->sock, "Liste des commandes disponibles :\n - /help : affiche ce message d'aide\n - /listejoueurs : affiche les autres joueurs connectes\n - /duel <pseudo> : défier le joueur <pseudo> dans un duel\n");
                  }
                  else if (strcmp(commande, "/listejoueurs") == 0)
                  {
                     if (actual == 1)
                     {
                        write_client(client->sock, "Aucun autre joueur n'est connecté\n");
                     }
                     else
                     {
                        buffer[0] = 0;
                        strncat(buffer, "Liste des joueurs connectés : \n", BUF_SIZE - strlen(buffer) - 1);

                        elementListeClient *ptr2 = listeClients;
                        while (ptr2 != NULL)
                        {
                           if (ptr2->client->isConnected && ptr2->client != client)
                           {
                              strncat(buffer, " - ", BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, ptr2->client->name, BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                           }

                           ptr2 = ptr2->suivant;
                        }

                        write_client(client->sock, buffer);
                     }
                  }
                  else if (strcmp(commande, "/duel") == 0)
                  {
                     char *adversaire = parametres[1];
                     Client *c = rechercherClientParNom(listeClients, adversaire);
                     if (c == NULL)
                     {
                        write_client(client->sock, "Ce joueur n'existe pas\n");
                     }
                     else if (c->isConnected == 0)
                     {
                        write_client(client->sock, "Ce joueur n'est pas connecte\n");
                     }
                     else if (c == client)
                     {
                        write_client(client->sock, "Vous ne pouvez pas vous défier vous-même ;)\n");
                     }
                     else
                     {
                        Invitation *invit = rechercherInvitationParClients(listeInvitations, client, c);
                        if (invit != NULL)
                        {
                           if (invit->invite == client)
                           {
                              buffer[0] = 0;
                              strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, c->name, BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, " vous a déjà envoyé une invitation. Utilisez la commande /accepte <pseudo> pour commencer le match\n", BUF_SIZE - strlen(buffer) - 1);
                              write_client(client->sock, buffer);
                           }
                           else
                           {
                              write_client(client->sock, "Une invitation a déjà été envoyée à ce joueur. Il vient d'être relancé\n");
                              buffer[0] = 0;
                              strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, " vous relance l'invitation pour son duel !\nUtilisez la commande /accepte <pseudo> pour commencer le match\n", BUF_SIZE - strlen(buffer) - 1);
                              write_client(c->sock, buffer);
                           }
                           continue;
                        }

                        // Si un match existe déjà entre les deux joueurs
                        MatchAwale *match = rechercherMatchClients(listeMatchs, client, c);
                        if (match != NULL)
                        {
                           write_client(client->sock, "Un match est déjà en cours avec ce joueur\n");
                           continue;
                        }

                        // Envoie du message au joueur qui lance le duel
                        buffer[0] = 0;
                        strncat(buffer, "Une invitation a été envoyé à ", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, c->name, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                        write_client(client->sock, buffer);

                        // Envoie du message au joueur qui reçoit le duel
                        buffer[0] = 0;
                        strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, " vous défie dans un duel !\nUtilisez la commande /accepte <pseudo> pour commencer le match", BUF_SIZE - strlen(buffer) - 1);
                        write_client(c->sock, buffer);

                        // Création de l'invitation
                        Invitation *invitation = malloc(sizeof(Invitation));
                        invitation->inviteur = client;
                        invitation->invite = c;
                        listeInvitations = ajouterInvitation(listeInvitations, invitation);
                     }
                  }
                  else if (strcmp(commande, "/accepte") == 0)
                  {
                     MatchAwale *match = rechercherMatchClient(listeMatchs, client);
                     if (match != NULL)
                     {
                        write_client(client->sock, "Vous êtes déjà en match\n");
                        continue;
                     }

                     char *adversaireNom = parametres[1];
                     Client *adversaire = rechercherClientParNom(listeClients, adversaireNom);
                     if (adversaire == NULL) // client inexistant
                     {
                        write_client(client->sock, "Ce joueur n'existe pas\n");
                        continue;
                     }

                     Invitation *invit = rechercherInvitationParClients(listeInvitations, adversaire, client);
                     if (invit == NULL) // aucune invitation en attente
                     {
                        write_client(client->sock, "Aucune invitation n'a été envoyée à ce joueur\n");
                        continue;
                     }

                     if (invit->invite != client) // l'invitation n'est pas pour ce joueur
                     {
                        write_client(client->sock, "Cette invitation ne vous est pas destinée\n");
                        continue;
                     }

                     // vérifier que l'adversaire est connecté
                     if (adversaire->isConnected == 0)
                     {
                        write_client(client->sock, "Votre adversaire n'est pas connecté. Le match est annulé.\n");
                        listeInvitations = supprimerInvitation(listeInvitations, invit);
                        continue;
                     }

                     // Vérifier si l'adversaire est en match avec un autre joueur
                     // Dans ce cas, on annule l'invitation et on ne lance pas de nouveau match
                     match = rechercherMatchClient(listeMatchs, adversaire);
                     if (match != NULL)
                     {
                        write_client(client->sock, "Votre adversaire est déjà en match. Veuillez réessayer dans quelques instants.\n");
                        continue;
                     }

                     // Transformer l'invitation en match
                     partie *p = malloc(sizeof(partie));
                     init(p);
                     partieSauvegardee *sauvegarde = creerSauvegarde(client->name, adversaire->name, p->joueurCourant);
                     match = malloc(sizeof(MatchAwale));
                     match->joueur1 = invit->inviteur;
                     match->joueur2 = invit->invite;
                     match->partie = p;
                     match->spectateurs = NULL;
                     match->sauvegarde = sauvegarde;
                     listeMatchs = ajouterMatch(listeMatchs, match);
                     listeInvitations = supprimerInvitation(listeInvitations, invit);

                     // Envoie du message au joueur qui lance le duel
                     buffer[0] = 0;
                     strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " a accepté votre invitation !\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(adversaire->sock, buffer);

                     // Envoie du message au joueur qui reçoit le duel
                     buffer[0] = 0;
                     strncat(buffer, "Vous avez accepté l'invitation de ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(client->sock, buffer);

                     buffer[0] = 0;
                     sprintBoard(match->partie, buffer, match->joueur1->name, match->joueur2->name);
                     write_client(client->sock, buffer);
                     write_client(adversaire->sock, buffer);
                  }
                  else if (strcmp(commande, "/chat") == 0)
                  {
                     if (nbParams < 2)
                     {
                        write_client(client->sock, "Veuillez entrer un message\n");
                        continue;
                     }
                     char message[BUF_SIZE];
                     message[0] = 0;
                     for (int i = 1; i < nbParams; i++)
                     {
                        strncat(message, parametres[i], BUF_SIZE - strlen(message) - 1);
                        strncat(message, " ", BUF_SIZE - strlen(message) - 1);
                     }
                     send_message_to_all_clients(listeClients, *client, message, 0);
                  }
                  else if (strcmp(commande, "/msg") == 0)
                  {
                     char *pseudo = parametres[1];
                     // Recherche le joueur avec son pseudo
                     Client *c = rechercherClientParNom(listeClients, pseudo);
                     if (c == NULL)
                     {
                        write_client(client->sock, "Ce joueur n'existe pas\n");
                        continue;
                     }
                     else if (c == client)
                     {
                        write_client(client->sock, "Vous ne pouvez pas vous envoyer de message à vous-même ;)\n");
                        continue;
                     }
                     else if (c->isConnected == 0)
                     {
                        write_client(client->sock, "Ce joueur n'est pas connecté\n");
                        continue;
                     }
                     char message[BUF_SIZE];
                     message[0] = 0;
                     for (int i = 2; i < nbParams; i++)
                     {
                        strncat(message, parametres[i], BUF_SIZE - strlen(message) - 1);
                        strncat(message, " ", BUF_SIZE - strlen(message) - 1);
                     }
                     buffer[0] = 0;
                     strncat(buffer, "Message de ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, message, BUF_SIZE - strlen(buffer) - 1);
                     write_client(c->sock, buffer);
                  }
                  else if (strcmp(commande, "/refuse") == 0)
                  {
                     char *adversaireNom = parametres[1];
                     Client *adversaire = rechercherClientParNom(listeClients, adversaireNom);
                     if (adversaire == NULL) // client inexistant
                     {
                        write_client(client->sock, "Ce joueur n'existe pas\n");
                        continue;
                     }

                     Invitation *invit = rechercherInvitationParClients(listeInvitations, adversaire, client);
                     if (invit == NULL) // aucune invitation en attente
                     {
                        write_client(client->sock, "Aucune invitation n'a été envoyée à ce joueur\n");
                        continue;
                     }

                     if (invit->invite != client) // l'invitation n'est pas pour ce joueur
                     {
                        write_client(client->sock, "Cette invitation ne vous est pas destinée\n");
                        continue;
                     }

                     // Supprimer l'invitation
                     listeInvitations = supprimerInvitation(listeInvitations, invit);

                     // Envoie du message au joueur qui lance le duel
                     buffer[0] = 0;
                     strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " a refusé votre invitation !\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(adversaire->sock, buffer);

                     // Envoie du message au joueur qui reçoit le duel
                     buffer[0] = 0;
                     strncat(buffer, "Vous avez refusé l'invitation de ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(client->sock, buffer);
                  }
                  else if (strcmp(commande, "/setbio") == 0)
                  {
                     // ajouter tous ce qui suit la commande dans la bio du joueur
                     if (nbParams < 2)
                     {
                        write_client(client->sock, "Veuillez entrer une bio\n");
                        continue;
                     }
                     char message[BUF_SIZE];
                     message[0] = 0;
                     for (int i = 1; i < nbParams; i++)
                     {
                        strncat(message, parametres[i], BUF_SIZE - strlen(message) - 1);
                        strncat(message, " ", BUF_SIZE - strlen(message) - 1);
                     }
                     client->bio = malloc(sizeof(char) * strlen(message));
                     strncpy(client->bio, message, strlen(message));
                     buffer[0] = 0;
                     strncat(buffer, "Votre bio a été mise à jour\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(client->sock, buffer);
                  }
                  else if (strcmp(commande, "/bio") == 0)
                  {
                     char *pseudo = parametres[1];
                     if (pseudo == NULL)
                     {
                        if (client->bio == NULL)
                        {
                           write_client(client->sock, "Vous n'avez pas de bio. Utilisez la commande /setbio pour en définir une\n");
                        }
                        else
                        {
                           buffer[0] = 0;
                           strncat(buffer, "Votre bio : ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, client->bio, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                           write_client(client->sock, buffer);
                        }
                     }
                     else
                     {
                        Client *c = rechercherClientParNom(listeClients, pseudo);
                        if (c == NULL)
                        {
                           write_client(client->sock, "Ce joueur n'existe pas\n");
                        }
                        else if (c->bio == NULL)
                        {
                           write_client(client->sock, "Ce joueur n'a pas de bio\n");
                        }
                        else
                        {
                           buffer[0] = 0;
                           strncat(buffer, "Bio de ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, c->name, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, c->bio, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                           write_client(client->sock, buffer);
                        }
                     }
                  }
                  else if (strcmp(commande, "/abandonne") == 0)
                  {
                     MatchAwale *match = rechercherMatchClient(listeMatchs, client);
                     if (match == NULL)
                     {
                        write_client(client->sock, "Vous n'êtes pas en match\n");
                        continue;
                     }

                     // vérifier que l'adversaire est connecté
                     Client *adversaire = match->joueur1 == client ? match->joueur2 : match->joueur1;
                     if (adversaire->isConnected == 0)
                     {
                        write_client(client->sock, "Votre adversaire n'est pas connecté. Le match est nul.\n");
                        listeMatchs = supprimerMatch(listeMatchs, match, 1);
                        continue;
                     }

                     // Envoyer un message à l'adversaire
                     buffer[0] = 0;
                     strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " a abandonné le match. Vous le remportez !\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(adversaire->sock, buffer);

                     // Envoyer un message au joueur qui abandonne
                     buffer[0] = 0;
                     strncat(buffer, "Vous avez abandonné le match contre ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, ". Vous avez perdu !\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(client->sock, buffer);

                     // Envoyer le résultat du match aux spectateurs
                     elementListeClient *spectateurs = match->spectateurs;
                     buffer[0] = 0;
                     strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " a abandonné le match contre ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, ". ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, adversaire->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " remporte le match !\n", BUF_SIZE - strlen(buffer) - 1);
                     while (spectateurs != NULL)
                     {
                        write_client(spectateurs->client->sock, buffer);
                        spectateurs = spectateurs->suivant;
                     }

                     // supprimer le match de la liste des matchs
                     listeMatchs = supprimerMatch(listeMatchs, match, 1);
                  }
                  else if (strcmp(commande, "/listematchs") == 0)
                  {
                     elementListeMatch *matchs = listeMatchs;

                     if (matchs == NULL)
                     {
                        strcpy(buffer, "Aucun match en cours\n");
                     }
                     else
                     {
                        strcpy(buffer, "Liste des matchs :\n");
                        while (matchs != NULL)
                        {
                           strncat(buffer, " - ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, matchs->match->joueur1->name, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, " contre ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, matchs->match->joueur2->name, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                           matchs = matchs->suivant;
                        }
                     }

                     write_client(client->sock, buffer);
                  }
                  else if (strcmp(commande, "/spec") == 0)
                  {
                     if (nbParams < 3)
                     {
                        write_client(client->sock, "Usage invalide de la commande. Tapez /spec <joueur1> <joueur2>");
                        continue;
                     }

                     char *joueur1 = parametres[1];
                     Client *c1 = rechercherClientParNom(listeClients, joueur1);
                     if (c1 == NULL)
                     {
                        write_client(client->sock, "Le joueur 1 n'existe pas\n");
                        continue;
                     }

                     char *joueur2 = parametres[2];
                     Client *c2 = rechercherClientParNom(listeClients, joueur2);
                     if (c2 == NULL)
                     {
                        write_client(client->sock, "Le joueur 2 n'existe pas\n");
                        continue;
                     }

                     MatchAwale *match = rechercherMatchClients(listeMatchs, c1, c2);
                     if (match == NULL)
                     {
                        write_client(client->sock, "Aucun match en cours entre ces deux joueurs\n");
                        continue;
                     }

                     // Ajouter le joueur à la liste des spectateurs
                     match->spectateurs = ajouterClient(match->spectateurs, client);

                     write_client(client->sock, "Vous êtes maintenant spectateur de ce match\n");
                     sprintBoard(match->partie, buffer, match->joueur1->name, match->joueur2->name);
                     write_client(client->sock, buffer);
                  }
                  else if (strcmp(commande, "/listeinvitations") == 0)
                  {
                     elementListeInvitation *listeInvit = rechercherInvitationClient(listeInvitations, client);
                     if (listeInvit == NULL)
                     {
                        write_client(client->sock, "Vous n'avez aucune invitation\n");
                        continue;
                     }

                     buffer[0] = 0;
                     strncat(buffer, "Liste de vos invitations :\n", BUF_SIZE - strlen(buffer) - 1);
                     while (listeInvit != NULL)
                     {
                        Invitation *invit = listeInvit->invitation;
                        if (invit->invite == client)
                        {
                           strncat(buffer, " - ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, invit->inviteur->name, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, " vous a invité\n", BUF_SIZE - strlen(buffer) - 1);
                        }
                        else
                        {
                           strncat(buffer, " - Vous avez invité ", BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, invit->invite->name, BUF_SIZE - strlen(buffer) - 1);
                           strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                        }
                        listeInvit = listeInvit->suivant;
                     }
                     clearListeInvitation(listeInvit, 0);
                     write_client(client->sock, buffer);
                  }
                  else if (strcmp(commande, "/listesaves") == 0)
                  {
                     write_client(client->sock, "Parties sauvegardées :\n");
                     char *liste = listerSauvegardes("");
                     if (liste == NULL || strlen(liste) == 0)
                     {
                        write_client(client->sock, "Aucune partie sauvegardée\n");
                     }
                     else
                     {
                        write_client(client->sock, liste);
                     }
                     if (liste != NULL)
                     {
                        free(liste);
                     }
                  }
                  else if (strcmp(commande, "/watchsave") == 0)
                  {
                     if (nbParams < 2)
                     {
                        write_client(client->sock, "Veuillez sélectionner le match à revisionner\n");
                        continue;
                     }

                     char *nomMatch = parametres[1];
                     partieSauvegardee *sauvegarde = chargerPartie(nomMatch);

                     if (sauvegarde == NULL)
                     {
                        write_client(client->sock, "Ce match n'existe pas\n");
                        continue;
                     }

                     // Rejouer les coups et envoyer le plateau à chaque fois
                     partie *p = malloc(sizeof(partie));
                     init(p);
                     p->joueurCourant = sauvegarde->premierJoueur;
                     sprintBoard(p, buffer, sauvegarde->nomJoueur1, sauvegarde->nomJoueur2);
                     for (int i = 0; i < sauvegarde->nbCoups; i++)
                     {
                        jouer(p, sauvegarde->coups[i]);
                        buffer[0] = 0;
                        sprintBoard(p, buffer, sauvegarde->nomJoueur1, sauvegarde->nomJoueur2);
                        write_client(client->sock, buffer);
                     }

                     // Envoyer le score
                     buffer[0] = 0;
                     strncat(buffer, "Fin de partie !\n", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, "Score : ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, sauvegarde->nomJoueur1, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                     char score[10];
                     sprintf(score, "%d", p->scores[0]);
                     strncat(buffer, score, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " - ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, sauvegarde->nomJoueur2, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                     sprintf(score, "%d", p->scores[1]);
                     strncat(buffer, score, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                     write_client(client->sock, buffer);

                     // Supprimer la partie
                     free(p);
                     free(sauvegarde->coups);
                     free(sauvegarde);
                  }
                  else
                  {
                     write_client(client->sock, "Commande inconnue\n");
                  }
               }
               else
               {
                  printf("client %s : %s\n", client->name, buffer);
                  // trouver la partie associée au joueur
                  //    Si pas de partie trouvée, envoyer un message d'erreur
                  MatchAwale *match = rechercherMatchClient(listeMatchs, client);
                  if (match == NULL)
                  {
                     write_client(client->sock, "Aucun match en cours\n");
                     continue;
                  }

                  // récuper le joueur courant dans la partie
                  char *joueurCourant = match->partie->joueurCourant == 0 ? match->joueur1->name : match->joueur2->name;
                  // Vérifier que c'est à lui de jouer
                  if (strcmp(client->name, joueurCourant) != 0)
                  {
                     write_client(client->sock, "Ce n'est pas à vous de jouer\n");
                     continue;
                  }

                  // Jouer le coup
                  int coup = interpreterCoup(match->partie, buffer[0]);
                  if (coup == -1)
                  {
                     write_client(client->sock, "Coup invalide\n");
                     continue;
                  }

                  int resultat = jouer(match->partie, coup);
                  if (resultat == EXIT_FAILURE)
                  {
                     write_client(client->sock, "Coup injouable\n");
                     continue;
                  }
                  else
                  {
                     // Enregistrer le coup dans l'historique
                     ajouterCoups(match->sauvegarde, &coup, 1);

                     buffer[0] = 0;
                     sprintBoard(match->partie, buffer, match->joueur1->name, match->joueur2->name);
                     write_client(match->joueur1->sock, buffer);
                     write_client(match->joueur2->sock, buffer);

                     // envoyer la partie à tous les spectateurs
                     elementListeClient *spectateurs = match->spectateurs;
                     while (spectateurs != NULL)
                     {
                        write_client(spectateurs->client->sock, buffer);
                        spectateurs = spectateurs->suivant;
                     }

                     if (finDePartie(match->partie) != PAS_DE_GAGNANT)
                     {
                        // Enregistrer la partie dans un fichier
                        sauvegarderPartie(match->sauvegarde);

                        // Envoyer le score aux deux joueurs
                        buffer[0] = 0;
                        strncat(buffer, "Fin de partie !\n", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, "Score : ", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, match->joueur1->name, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                        char score[10];
                        sprintf(score, "%d", match->partie->scores[0]);
                        strncat(buffer, score, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, " - ", BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, match->joueur2->name, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                        sprintf(score, "%d", match->partie->scores[1]);
                        strncat(buffer, score, BUF_SIZE - strlen(buffer) - 1);
                        strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
                        write_client(match->joueur1->sock, buffer);
                        write_client(match->joueur2->sock, buffer);
                        spectateurs = match->spectateurs;
                        while (spectateurs != NULL)
                        {
                           write_client(spectateurs->client->sock, buffer);
                           spectateurs = spectateurs->suivant;
                        }

                        // supprimer le match de la liste des matchs
                        listeMatchs = supprimerMatch(listeMatchs, match, 1);
                     }
                  }
               }
            }
         }
      }
   }

   clear_clients(listeClients, actual);
   end_connection(sock);
}

void send_welcome_message(Client *client)
{
   char buffer[BUF_SIZE];
   buffer[0] = 0;
   strcat(buffer, "=============================\n");
   strcat(buffer, "\033[0;31m\033[1m    ___                 __   \n");
   strcat(buffer, "   /   |_      ______ _/ /__ \n");
   strcat(buffer, "  / /| | | /| / / __ `/ / _ \\\n");
   strcat(buffer, " / ___ | |/ |/ / /_/ / /  __/\n");
   strcat(buffer, "/_/  |_|__/|__/\\__,_/_/\\___/ \033[0m\n");
   strcat(buffer, "=============================\n\n");
   strcat(buffer, "Bienvenue ");
   strcat(buffer, client->name);
   strcat(buffer, " sur le serveur !\n");
   strcat(buffer, "Tapez /help pour afficher la liste des commandes disponibles\n");
   write_client(client->sock, buffer);
}

static void clear_clients(elementListeClient *clients, int actual)
{
   elementListeClient *ptr = clients;
   while (ptr != NULL)
   {
      closesocket(ptr->client->sock);
      ptr = ptr->suivant;
   }
}

static void remove_client(elementListeClient *clients, Client client)
{
   /* we remove the client in the list */
   elementListeClient *ptr = clients;
   while (ptr != NULL)
   {
      if (ptr->client->sock == client.sock)
      {
         ptr->client->isConnected = 0;
         ptr->client->sock = -1;
         break;
      }
      ptr = ptr->suivant;
   }
}

static void send_message_to_all_clients(elementListeClient *clients, Client sender, const char *buffer, char from_server)
{
   char message[BUF_SIZE];
   message[0] = 0;
   elementListeClient *ptr = clients;
   if (from_server == 0)
   {
      strncpy(message, sender.name, BUF_SIZE - 1);
      strncat(message, " : ", sizeof message - strlen(message) - 1);
   }
   strncat(message, buffer, sizeof message - strlen(message) - 1);
   while (ptr != NULL)
   {
      /* we don't send message to the sender */
      if (sender.sock != ptr->client->sock && ptr->client->isConnected == 1)
      {
         write_client(ptr->client->sock, message);
      }
      ptr = ptr->suivant;
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = {0};

   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   int reuse = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
   {
      perror("setsockopt");
   }

   if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if (send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init_server();

   app();

   end();

   return EXIT_SUCCESS;
}
