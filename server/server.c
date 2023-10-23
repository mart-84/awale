#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "listeclient.h"

static void init(void)
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
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   elementListeClient* listeClients = NULL;

   fd_set rdfs;

   while (1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for (i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
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
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if (csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         // TODO : vérifier que le nom n'est pas déjà pris
         int pseudoPris = 0;
         for (int i = 0; i <= actual; i++)
         {
            if (strcmp(clients[i].name, buffer) == 0)
            {
               printf("Client name %s already taken\n", buffer);
               pseudoPris = 1;
               break;
            }
         }

         if (pseudoPris)
         {
            write_client(csock, "Ce pseudo est déjà utilisé.\n");
            end_connection(csock);
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = {csock};
         strncpy(c.name, buffer, BUF_SIZE - 1);
         c.isConnected = 1;
         clients[actual] = c;
         listeClients = ajouterClient(listeClients, &c);
         actual++;
         printf("New client connected as %s\n", buffer);
      }
      else
      {
         int i = 0;
         for (i = 0; i < actual; i++)
         {
            break;
            /* a client is talking */
            if (FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               else if (buffer[0] == '/')
               {
                  char* commande = strtok(buffer, " \n");
                  if (strcmp(commande, "/help") == 0) {
                     printf("client %d (%s) : /help\n", i, clients[i].name);
                     write_client(clients[i].sock, "Liste des commandes disponibles :\n");
                     write_client(clients[i].sock, " - /help : affiche ce message d'aide\n");
                     write_client(clients[i].sock, " - /listejoueurs : affiche les autres joueurs connectes\n");
      
                  } else if (strcmp(commande, "/listejoueurs") == 0) {
                     printf("client %d (%s) : /listejoueurs\n", i, clients[i].name);
                     if (actual == 1) {
                        write_client(clients[i].sock, "Aucun autre joueur n'est connecte\n");
                     } else {
                        write_client(clients[i].sock, "Liste des joueurs connectes : \n");
                        for (int j = 0; j < actual; j++) {
                           if (j != i) {
                              write_client(clients[i].sock, " - ");
                              write_client(clients[i].sock, clients[j].name);
                              write_client(clients[i].sock, "\n");
                           }
                        }
                     }
                  } else if (strcmp(commande, "/duel") == 0) {
                     printf("client %d (%s) : /duel\n", i, clients[i].name);
                     char* adversaire = strtok(NULL, " \n");

                     //TODO recherche de l'adversaire puis recuperation de sa socket puis transmission du défi et gestion de la reponse

                     write_client(clients[i].sock, "Bon bah si ");
                     write_client(clients[i].sock, adversaire);
                     write_client(clients[i].sock, " est connecte tu peux lui insulter sa mere\n");
                  }
               }
               else
               {
                  send_message_to_all_clients(clients, client, actual, buffer, 0);
               }
               break;
            }
         }

         elementListeClient* ptr = listeClients;
         while(ptr != NULL) {
            if (ptr->client->isConnected == 0) {
               ptr = ptr->suivant;
               continue;
            }
            Client* client = ptr->client;

            ptr = ptr->suivant;
            
            /* a client is talking */
            if (FD_ISSET(client->sock, &rdfs))
            {
               int c = read_client(client->sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  closesocket(client->sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, *client, actual, buffer, 1);
               }
               else if (buffer[0] == '/')
               {
                  char* commande = strtok(buffer, " \n");
                  if (strcmp(commande, "/help") == 0) {
                     printf("client %d (%s) : /help\n", i, client->name);
                     write_client(client->sock, "Liste des commandes disponibles :\n");
                     write_client(client->sock, " - /help : affiche ce message d'aide\n");
                     write_client(client->sock, " - /listejoueurs : affiche les autres joueurs connectes\n");
      
                  } else if (strcmp(commande, "/listejoueurs") == 0) {
                     printf("client %d (%s) : /listejoueurs\n", i, client->name);
                     if (actual == 1) {
                        write_client(client->sock, "Aucun autre joueur n'est connecte\n");
                     } else {
                        write_client(client->sock, "Liste des joueurs connectes : \n");

                        elementListeClient* ptr2 = listeClients;
                        while (ptr2 != NULL) {
                           if (ptr2->client->isConnected && ptr2 != ptr) {
                              write_client(client->sock, " - ");
                              write_client(client->sock, ptr2->client->name);
                              write_client(client->sock, "\n");
                           }

                           ptr2 = ptr2->suivant;
                        }
                     }
                  } else if (strcmp(commande, "/duel") == 0) {
                     printf("client %d (%s) : /duel\n", i, client->name);
                     char* adversaire = strtok(NULL, " \n");

                     //TODO recherche de l'adversaire puis recuperation de sa socket puis transmission du défi et gestion de la reponse

                     write_client(client->sock, "Bon bah si ");
                     write_client(client->sock, adversaire);
                     write_client(client->sock, " est connecte tu peux lui insulter sa mere\n");
                  }
               }
               else
               {
                  send_message_to_all_clients(clients, *client, actual, buffer, 0);
               }
               break;
            }      
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for (i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for (i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if (sender.sock != clients[i].sock)
      {
         if (from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
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
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
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
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
