#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "listeClient.h"
#include "listeMatch.h"
#include "match.h"
#include "awale/awale.h"

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

   fd_set rdfs;

   printf("Waiting for connections...\n");

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
               // TODO : si une partie était en cours, lui proposer de la reprendre
               
               printf("Client %s reconnected\n", buffer);
               c->isConnected = 1;
               c->sock = clientSock;
               clientSock = c->sock;
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
         buffer[0] = 0;
         strcat(buffer, "=============================\n");
         strcat(buffer, "    ___                 __   \n");
         strcat(buffer, "   /   |_      ______ _/ /__ \n");
         strcat(buffer, "  / /| | | /| / / __ `/ / _ \\\n");
         strcat(buffer, " / ___ | |/ |/ / /_/ / /  __/\n");
         strcat(buffer, "/_/  |_|__/|__/\\__,_/_/\\___/ \n");
         strcat(buffer, "=============================\n\n");
         strcat(buffer, "Bienvenue ");
         strcat(buffer, c->name);
         strcat(buffer, " sur le serveur !\n");
         strcat(buffer, "Tapez /help pour afficher la liste des commandes disponibles\n");
         write_client(clientSock, buffer);

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
                  closesocket(client->sock);
                  remove_client(listeClients, *client);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  actual--;
                  send_message_to_all_clients(listeClients, *client, buffer, 1);
                  printf("Client %s disconnected\n", client->name);
               }
               else if (buffer[0] == '/')
               {
                  char *commande = strtok(buffer, " \n");
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
                     char *adversaire = strtok(NULL, " \n");
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
                        // Si un match existe déjà entre les deux joueurs
                        MatchAwale *match = rechercherMatchClients(listeMatchs, client, c);
                        if (match != NULL)
                        {
                           // Si l'invitation est en attente
                           if (match->etat == ATTENTE)
                           {
                              write_client(client->sock, "Une invitation a déjà été envoyée à ce joueur. Il vient d'être relancé\n");

                              buffer[0] = 0;
                              strncat(buffer, "Le joueur ", BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                              strncat(buffer, " vous relance l'invitation pour son duel !\n", BUF_SIZE - strlen(buffer) - 1);
                              write_client(c->sock, buffer);
                           }
                           else // Le match est en cours
                           {
                              write_client(client->sock, "Un match est déjà en cours avec ce joueur\n");
                           }
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
                        strncat(buffer, " vous défie dans un duel !\n", BUF_SIZE - strlen(buffer) - 1);
                        write_client(c->sock, buffer);

                        // Création du match et ajout à la liste des matchs
                        partie *p = malloc(sizeof(partie));
                        init(p);
                        MatchAwale *m = malloc(sizeof(MatchAwale));
                        m->joueur1 = client;
                        m->joueur2 = c;
                        m->partie = p;
                        m->etat = ATTENTE;
                        listeMatchs = ajouterMatch(listeMatchs, m);
                     }
                  }
                  else if (strcmp(commande, "/accepte") == 0)
                  {
                     char *adversaireNom = strtok(NULL, " \n");
                     Client *adversaire = rechercherClientParNom(listeClients, adversaireNom);
                     if (adversaire == NULL) // client inexistant
                     {
                        write_client(client->sock, "Ce joueur n'existe pas\n");
                        continue;
                     }

                     MatchAwale *match = rechercherMatchClients(listeMatchs, client, adversaire);
                     if (match == NULL) /// aucun match en attente
                     {
                        write_client(client->sock, "Aucune invitation n'a été envoyée à ce joueur\n");
                        continue;
                     }

                     if (match->etat != ATTENTE) // match déjà en cours
                     {
                        write_client(client->sock, "Aucune invitation n'est en attente avec ce joueur\n");
                        continue;
                     }

                     // Vérifier si l'adversaire est en match avec un autre joueur
                     // Dans ce cas, on annule l'invitation et on ne lance pas de nouveau match
                     elementListeMatch *ptr = rechercherMatchClient(listeMatchs, adversaire);
                     if (ptr != NULL)
                     {
                        elementListeMatch *ptr2 = ptr;
                        int adversaireEnMatch = 0;
                        while (ptr2 != NULL)
                        {
                           if (ptr2->match->etat == EN_COURS)
                           {
                              listeMatchs = supprimerMatch(listeMatchs, match, 1);
                              write_client(client->sock, "Votre adversaire est déjà en match. L'invitation a été annulée\n");
                              adversaireEnMatch = 1;
                              break;
                           }
                           ptr2 = ptr2->suivant;
                        }
                        if (adversaireEnMatch)
                        {
                           clearListeMatch(ptr);
                           continue;
                        }
                     }
                     clearListeMatch(ptr);

                     match->etat = EN_COURS;

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
                     char *message = buffer + 6; // 6 est la longueur de la commande + 1 espace
                     send_message_to_all_clients(listeClients, *client, message, 0);
                  }
                  else if (strcmp(commande, "/msg") == 0)
                  {
                     char *pseudo = strtok(NULL, " \n");
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
                     char *debutMsg = buffer + 5 + strlen(pseudo) + 1;
                     int lenMsg = strlen(debutMsg);
                     char *message = malloc(sizeof(char) * lenMsg);
                     strncpy(message, buffer + 5 + strlen(pseudo) + 1, lenMsg);
                     buffer[0] = 0;
                     strncat(buffer, "Message de ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, client->name, BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, " : ", BUF_SIZE - strlen(buffer) - 1);
                     strncat(buffer, message, BUF_SIZE - strlen(buffer) - 1);
                     free(message);
                     write_client(c->sock, buffer);
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
                  elementListeMatch *listeMatchsJoueur = rechercherMatchClient(listeMatchs, client);
                  MatchAwale *match = NULL;
                  if (listeMatchsJoueur == NULL)
                  {
                     write_client(client->sock, "Aucun match en cours\n");
                     continue;
                  }
                  else
                  {
                     elementListeMatch *ptr = listeMatchsJoueur;
                     while (ptr != NULL)
                     {
                        if (ptr->match->etat == EN_COURS)
                        {
                           match = ptr->match;
                           break;
                        }
                        ptr = ptr->suivant;
                     }
                     if (ptr == NULL)
                     {
                        write_client(client->sock, "Aucun match en cours\n");
                        continue;
                     }
                  }
                  clearListeMatch(listeMatchsJoueur);

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
                  if (resultat == -1)
                  {
                     write_client(client->sock, "Coup injouable\n");
                     continue;
                  } else {
                     buffer[0] = 0;
                     sprintBoard(match->partie, buffer, match->joueur1->name, match->joueur2->name);
                     write_client(match->joueur1->sock, buffer);
                     write_client(match->joueur2->sock, buffer);

                     if (finDePartie(match->partie) != PAS_DE_GAGNANT)
                     {
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

                        // supprimer le match de la liste des matchs
                        supprimerMatch(listeMatchs, match, 1);

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
