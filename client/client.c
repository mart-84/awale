#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ao/ao.h>
#include <mpg123.h>
#include <pthread.h>

#include "client.h"

#define BITS 8
#define MUSIC_PATH "music.mp3"

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

void *playMusic(void *param)
{
   mpg123_handle *mh;
   unsigned char *buffer_mp3;
   size_t buffer_size;
   size_t done;
   int err;

   int driver;
   ao_device *dev;

   ao_sample_format format;
   int channels, encoding;
   long rate;

   /* initializations */
   ao_initialize();
   driver = ao_default_driver_id();
   mpg123_init();
   mh = mpg123_new(NULL, &err);
   buffer_size = mpg123_outblock(mh);
   buffer_mp3 = (unsigned char *)malloc(buffer_size * sizeof(unsigned char));

   /* open the file and get the decoding format */
   mpg123_open(mh, MUSIC_PATH);
   mpg123_getformat(mh, &rate, &channels, &encoding);

   /* set the output format and open the output device */
   format.bits = mpg123_encsize(encoding) * BITS;
   format.rate = rate;
   format.channels = channels;
   format.byte_format = AO_FMT_NATIVE;
   format.matrix = 0;
   dev = ao_open_live(driver, &format, NULL);

   /* decode and play */
   while (mpg123_read(mh, buffer_mp3, buffer_size, &done) == MPG123_OK)
      ao_play(dev, (char *)buffer_mp3, done);

   /* clean up */
   free(buffer_mp3);
   ao_close(dev);
   mpg123_close(mh);
   mpg123_delete(mh);
   mpg123_exit();
   ao_shutdown();
   return NULL;
}

static void app(const char *address, const char *name)
{
   SOCKET sock = init_connection(address);
   char buffer[BUF_SIZE];
   pthread_t thread = 0;

   fd_set rdfs;

   /* send our name */
   write_server(sock, name);

   while (1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         fgets(buffer, BUF_SIZE - 1, stdin);
         {
            char *p = NULL;
            p = strstr(buffer, "\n");
            if (p != NULL)
            {
               *p = 0;
            }
            else
            {
               /* fclean */
               buffer[BUF_SIZE - 1] = 0;
            }
         }
         if (strcmp(buffer, "/quit") == 0)
         {
            printf("==============================================\n");
            printf("\033[0;31m\033[1m    ___                                 _     \n");
            printf("   /   | __  __   ________ _   ______  (_)____\n");
            printf("  / /| |/ / / /  / ___/ _ \\ | / / __ \\/ / ___/\n");
            printf(" / ___ / /_/ /  / /  /  __/ |/ / /_/ / / /    \n");
            printf("/_/  |_\\__,_/  /_/   \\___/|___/\\____/_/_/     \033[0m\n");
            printf("==============================================\n");
            break;
         }
         else if (strcmp(buffer, "/music enable") == 0)
         {
            if (thread != 0)
            {
               pthread_cancel(thread);
               thread = 0;
            }
            pthread_create(&thread, NULL, playMusic, NULL);
            printf("Music enabled\n");
            continue;
         }
         else if (strcmp(buffer, "/music disable") == 0)
         {
            if (thread != 0)
            {
               pthread_cancel(thread);
               thread = 0;
               printf("Music disabled\n");
            }
            continue;
         }
         write_server(sock, buffer);
      }
      else if (FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /* server down */
         if (n == 0)
         {
            printf("Server disconnected !\n");
            break;
         }
         puts(buffer);
      }
   }

   end_connection(sock);
}

static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = {0};
   struct hostent *hostinfo;

   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf(stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}

static void write_server(SOCKET sock, const char *buffer)
{
   if (send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   init();

   app(argv[1], argv[2]);

   end();

   return EXIT_SUCCESS;
}
