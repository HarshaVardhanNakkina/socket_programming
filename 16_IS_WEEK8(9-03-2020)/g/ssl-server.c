//SSL-Server.c
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define FAIL -1

void error(char *msg)
{
  perror(msg);
  abort();
}

int OpenListener(int port)
{
  int sd;
  struct sockaddr_in addr;

  sd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
  {
    error("can't bind port");
  }
  if (listen(sd, 10) != 0)
  {
    error("Can't configure listening port");
  }
  return sd;
}

int isRoot()
{
  if (getuid() != 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
SSL_CTX *InitServerCTX(void)
{
  SSL_METHOD *method;
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();     /* load & register all cryptos, etc. */
  SSL_load_error_strings();         /* load all error messages */
  method = TLSv1_2_server_method(); /* create new server-method instance */
  ctx = SSL_CTX_new(method);        /* create new context from method */
  if (ctx == NULL)
  {
    ERR_print_errors_fp(stderr);
    abort();
  }
  return ctx;
}

void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile)
{
  /* set the local certificate from CertFile */
  if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    abort();
  }
  /* set the private key from KeyFile (may be the same as CertFile) */
  if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    abort();
  }
  /* verify private key */
  if (!SSL_CTX_check_private_key(ctx))
  {
    fprintf(stderr, "Private key does not match the public certificate\n");
    abort();
  }
}

void ShowCerts(SSL *ssl)
{
  X509 *cert;
  char *line;

  cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
  if (cert != NULL)
  {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);
    X509_free(cert);
  }
  else
    printf("No certificates.\n");
}

void reverse(char *str)
{
  int n = strlen(str);
  for (int i = 0; i < n / 2; i++)
  {
    char temp = str[i];
    str[i] = str[n - i - 1];
    str[n - i - 1] = temp;
  }
}

void Servlet(SSL *ssl)
{ /* Serve the connection -- threadable */
  char buf[1024];
  char reply[1024];
  int sd, bytes;
  // const char *HTMLecho = "<html><body><pre>%s</pre></body></html>\n\n";

  if (SSL_accept(ssl) == FAIL) /* do SSL-protocol accept */
    ERR_print_errors_fp(stderr);
  else
  {
    // ShowCerts(ssl);                          /* get any certificates */
    bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
    if (bytes > 0)
    {
      buf[bytes] = 0;
      printf("Client msg: \"%s\"\n", buf);
      // sprintf(reply, reverse(buf), buf);
      strcpy(reply, buf);
      reverse(reply);                       /* construct reply */
      SSL_write(ssl, reply, strlen(reply)); /* send reply */
    }
    else
      ERR_print_errors_fp(stderr);
  }
  // sd = SSL_get_fd(ssl); /* get socket connection */
  // SSL_free(ssl);        /* release SSL state */
  // close(sd);            /* close connection */
}

int main(int count, char *strings[])
{
  SSL_CTX *ctx;
  int server;
  char *portnum;
  struct hostent *host_entry;
  struct in_addr **addr_list;

  if (!isRoot())
  {
    printf("This program must be run as root/sudo user!!");
    exit(0);
  }
  if (count != 2)
  {
    printf("Usage: %s <portnum>\n", strings[0]);
    exit(0);
  }
  SSL_library_init();

  portnum = strings[1];
  ctx = InitServerCTX();                             /* initialize SSL */
  LoadCertificates(ctx, "mycert.pem", "mycert.pem"); /* load certs */
  server = OpenListener(atoi(portnum));              /* create server socket */

  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  SSL *ssl;

  int client = accept(server, (struct sockaddr *)&addr, &len); /* accept connection as usual */
  printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  ssl = SSL_new(ctx);      /* get new SSL state with context */
  SSL_set_fd(ssl, client); /* set connection socket to SSL state */
  while (1)
  {
    char buf[1024];
    char reply[1024];
    int bytes;
    // const char *HTMLecho = "<html><body><pre>%s</pre></body></html>\n\n";

    if (SSL_accept(ssl) == FAIL) /* do SSL-protocol accept */
      ERR_print_errors_fp(stderr);
    else
    {
      // ShowCerts(ssl);                          /* get any certificates */
      bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
      if (bytes > 0)
      {
        puts(buf);
        if ((host_entry = gethostbyname(buf)) == NULL)
        {
          herror("gethostbyname");
          printf("could not resolve 🙏\n");
        }
        else
        {
          addr_list = (struct in_addr **)host_entry->h_addr_list;
          int i;
          sprintf(buf, "%s has resolved to %s \n", host_entry->h_name, inet_ntoa(*addr_list[0]));
          SSL_write(ssl, buf, strlen(buf));
        }
      }
      else
        ERR_print_errors_fp(stderr);
    }
    int sd;
    sd = SSL_get_fd(ssl);
    SSL_free(ssl);
    close(sd);
  }
  close(server);
  SSL_CTX_free(ctx);
}