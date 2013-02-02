#include <stdio.h>
#include <winsock.h>

void main()
{

  char datidainviare[300];
  char datiricevuti[300];
  SOCKET clientsock;

  WORD wVersionRequested;
  WSADATA wsaData;
  int err, r, i, j;
  struct in_addr inaddr;
  struct hostent * hs;
  char c, smtp_server[15], mittente[35], destinatario[35], oggetto[255], * * testo_messm, testo_mess[1250];
  int smtp_port = 25, n_mail = 1;


  sockaddr_in clientsockaddr;

  hs = ( hostent * ) malloc( sizeof( hostent ) );

  printf( "Server SMTP da usare (es: mail.libero.it): " );
  gets( smtp_server );
  printf( "Porta SMTP (default=25): " );
  scanf( "%d", & smtp_port );
  printf( "Da (es: mittente@server.com): " );
  gets( mittente );
  gets( mittente );
  printf( "Per (es: destinatario@server.com): " );
  gets( destinatario );
  printf( "Soggetto (unica riga): " );
  gets( oggetto );
  printf( "Testo del msg (in + righe, scrivere --fine-- nell'ultima riga:\n" );


  testo_messm = ( char * * ) calloc( 250, sizeof( char * ) );
  i = -1;
  do
  {
    i++;
    testo_messm[i] = ( char * ) calloc( 1250, sizeof( char ) );
    gets( testo_messm[i] );
  }
  while ( strncmp( testo_messm[i], "--fine--", 8 ) != 0 );

  sprintf( testo_mess, "%s\n", testo_messm[0] );

  for ( j = 1; j < i; j++ )
  {
    strcat( testo_mess, testo_messm[j] );
    strcat( testo_mess, "\n" );
  }

  printf( "N.mail da inviare (default=1): " );
  scanf( "%d", & n_mail );

  printf( "  -------------------------\n " );

  //inizializzo
  wVersionRequested = MAKEWORD( 2, 2 );

  err = WSAStartup( wVersionRequested, & wsaData );
  if ( err != 0 )
  {
    {
      printf( "Esco al controllo 1\n" );
      system( "PAUSE" ); return;
    }
  }

  if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
  {
    WSACleanup();
    {
      printf( "Esco al controllo 2\n" );
      {
        system( "PAUSE" ); return;
      }
    }
  }

  /* riempio struct */
  clientsockaddr.sin_family = AF_INET;
  clientsockaddr.sin_addr.s_addr = inet_addr( smtp_server ); //localhost
  clientsockaddr.sin_port = htons( ( u_short )smtp_port );
  hs = gethostbyname( smtp_server );
  clientsockaddr.sin_family = hs->h_addrtype;
  memcpy( ( void * ) & clientsockaddr.sin_addr.s_addr, hs->h_addr, hs->h_length );

  clientsock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); //crea connessione

  err = connect( clientsock, ( sockaddr * ) & clientsockaddr, sizeof( clientsockaddr ) ); //connetto
  if ( err == -1 )
  {
    printf( "Errore connect\n" ); system( "PAUSE" ); return;
  }

  getchar();
  //risposta smtp
  r = recv( clientsock, datiricevuti, 300, 0 );


  getchar();
  datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );


  sprintf( datidainviare, "EHLO %s\r\n", smtp_server );
  err=send( clientsock, datidainviare, strlen( datidainviare ), 0 );
      printf("Risultato send=%d\n", err);
  r = recv( clientsock, datiricevuti, 300, 0 ); datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );

  for ( j = 0; j < n_mail + 1; j++ )
  {
    sprintf( datidainviare, "MAIL FROM: <%s>\r\n", mittente );
    send( clientsock, datidainviare, strlen( datidainviare ), 0 );
    r = recv( clientsock, datiricevuti, 300, 0 ); datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );
    sprintf( datidainviare, "RCPT TO: <%s>\r\n", destinatario );
    send( clientsock, datidainviare, strlen( datidainviare ), 0 );
    r = recv( clientsock, datiricevuti, 300, 0 ); datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );
    strcpy( datidainviare, "DATA\r\n" );
    send( clientsock, datidainviare, strlen( datidainviare ), 0 );
    r = recv( clientsock, datiricevuti, 300, 0 ); datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );

    sprintf( datidainviare, "from: %s\r\nto: %s\r\nsubject: %s\r\n\r\n%s\r\n.\r\n", mittente,
         destinatario, oggetto, testo_mess );
    printf( "%s", datidainviare );

    send( clientsock, datidainviare, strlen( datidainviare ), 0 ); //invio

    r = recv( clientsock, datiricevuti, 300, 0 ); datiricevuti[r] = '\0'; printf( "%s\n", datiricevuti );
  }
  system( "PAUSE" );
  closesocket( clientsock );

  WSACleanup();
}
