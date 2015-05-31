#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>



#define MAX_MSG_LEN 18
union conv{
    int32_t f_buf[MAX_MSG_LEN];
    char c_buf[4*MAX_MSG_LEN];
};

using namespace std;
void f_help()
{
     cout << "\nexit - end program"  << endl
          <<"stop server - stop server iDom and exit" << endl
          <<"test - send test message"<<endl
          <<"send - send Send own message" << endl
          <<"set server id - set new id" << endl
          <<"send_to RS232 / NODE  "<< endl;
}

uint32_t ChangeEndianness(uint32_t value)
{
  if (__BYTE_ORDER == __BIG_ENDIAN)
  {
   //printf("big-endian");
   return value;
  }
  else if( __BYTE_ORDER == __LITTLE_ENDIAN)
   { //printf("little-endian");
    uint32_t result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;

    return result;
    }
}
void binary(int val)
{    for (int i = 31; i >= 0; i--)
    {
        if ((i + 1) % 8 == 0)            cout << ' ';        cout << ((val >> i) % 2);    }    cout << endl;}

void Send_and_recv (int &gniazdo ,int32_t  bufor[MAX_MSG_LEN],int &max_msg)
{

    //binary(bufor[2]);
    conv msg;
    ///  to w funkcji dac
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        msg.f_buf[i]= ChangeEndianness( bufor[i]) ;
    }

   // binary(msg.f_buf[2]);
     cout << " wysylam\n";
int dane;
    if(( dane = send( gniazdo, msg.c_buf/*bufor*/, max_msg, MSG_DONTWAIT ) ) <= 0 ) // MSG_DONTWAIT
    {
        perror( "send() ERROR" );
        exit( - 1 );
    }
    std::cout << "wyslalem ilosc danych   " << dane << std::endl;
    //bzero( bufor, MAX_MSG_LEN );
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        bufor[i]=0;
    }

     cout << " przed recv \n";

    if(( dane = recv( gniazdo,  msg.c_buf/*bufor*/, max_msg, 0 ) ) <= 0 )
    {
        perror( "recv() ERROR" );
        exit( - 1 );

    }
   // binary(bufor[2]);
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
         bufor[i]= ChangeEndianness( msg.f_buf[i]) ;
    }
    //binary(bufor[2]);
    cout << "odebralem danych " << dane << endl;
//ssize_t bytes_recieved = -1;

//while (bytes_recieved == -1){
//    bytes_recieved = recv( gniazdo, bufor, max_msg, MSG_DONTWAIT );

//    if (bytes_recieved == 0)  cout << "host shut down." <<  endl ;
//    if (bytes_recieved == -1) cout << "recieve error!" <<  endl ;
//     cout << bytes_recieved << " bytes recieved :" <<  endl;
//}

     cout << "|Wiadomosc z serwera|: "<< bufor[1]<<  endl;
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
         cout << bufor[i] << " ";
    }
     cout <<"\n"<< sizeof(int32_t)<< " to wielkosc int32_ta\n" ;
} // end Send_and_recv


////////////////////////////////////////   MAIN  //////////////////////////////////////////
int main( int argc, char ** argv )
{   if (argc <3)
    {   printf("\e[0;31m");//a teraz na czerwono
         cout << "Try: \e[0;1m"<< argv[0] << "\e[0;33m [ip_adres] [port]\n";
        printf("\e[0;0m");//powrot do normy
        return 0;
    }

     string command;
    bool go_while = true;
    struct sockaddr_in serwer;
    int gniazdo;
    int32_t bufor[ MAX_MSG_LEN ];
    int max_msg = MAX_MSG_LEN*sizeof(int32_t);

    bzero( & serwer, sizeof( serwer ) );
    //bzero( bufor, MAX_MSG_LEN );
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        bufor[i]=0;
    }
       string s_ip;
    // przerobka  adresu na ip . //////////////////////////////////

    int i;
    struct hostent * he;
    struct in_addr ** addr_list;

    char *buf =argv[1];//[ 100 ];
    //scanf( "%99s", buf );

    if(( he = gethostbyname( buf ) ) == NULL )
    {
        herror( "gethostbyname" );
        return - 1;
    }

    // print information about this host:
    printf( "Official name is: %s\n", he->h_name );

    printf( "IP addresses: " );
    addr_list =( struct in_addr ** ) he->h_addr_list;
    for( i = 0; addr_list[ i ] != NULL; i++ )
    {
        printf( "%s ", inet_ntoa( * addr_list[ i ] ) );
        s_ip += inet_ntoa( * addr_list[ i ] );

    }
    printf( "\n" );
    /////////// koniec ./////////////////////////////////

    // cout << " string " << s_ip <<  endl;
    const char * ip = s_ip.c_str();
    uint16_t port = atoi( argv[ 2 ] );

    serwer.sin_family = AF_INET;
    serwer.sin_port = htons( port );
    if( inet_pton( AF_INET, ip, & serwer.sin_addr ) <= 0 )
    {
        perror( "Wrong ip adres\n" );
        exit( - 1 );
    }

    if(( gniazdo = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "socket() ERROR" );
        exit( - 1 );
    }

    socklen_t len = sizeof( serwer );
    if( connect( gniazdo,( struct sockaddr * ) & serwer, len ) < 0 )
    {
        perror( "connect() ERROR" );
        exit( - 1 );
    }
     cout << " polaczono z serwerem \n";
    do {
         cout << "\nSet command: \n> ";
         cin >> command;

        if (command=="exit")
        {
            go_while = false;
            break;
        }
        else if (command=="stop")
        {
             cin >> command;
            if (command=="server")
            {
                go_while = false;
                bufor[3]= bufor[5]=0;
                bufor[4]= bufor[6]=-1;
                Send_and_recv(gniazdo, bufor, max_msg);
                break;
            }
            else
            {
                 cout << "unknown command "<< command <<  endl;
            }
        }

        else if (command=="send_to")
        {
             cin >> command;
            if (command=="RS232")
            {

                bufor[16]=11;
                int przerwa;
                 cin >> przerwa;
                bufor[0]=bufor[2]=0;
                bufor[1]=bufor[3]=-1;
                int licznik =30000;
                while (licznik)
                {    bufor[16]=11;
                    sleep (przerwa);
                    Send_and_recv(gniazdo, bufor, max_msg);
                    --licznik;
                }

            }
            else if (command=="NODE")
            {

                bufor[16]=1001;
                Send_and_recv(gniazdo, bufor, max_msg);

            }
            else if (command=="NODE_SPAM")
            {
                int przerwa;
                 cin >> przerwa;
                bufor[0]=bufor[2]=0;
                bufor[1]=bufor[3]=-1;
                int licznik =30000;
                while (licznik)
                {    bufor[16]=1001;
                    sleep (przerwa);
                    Send_and_recv(gniazdo, bufor, max_msg);
                    --licznik;
                }
            }
            else
            {
                 cout << "unknown command "<< command <<  endl;
            }
        }
        else if (command=="test")
        {

            bufor[0]=bufor[2]=0;
            bufor[1]=bufor[3]=-1;
            Send_and_recv(gniazdo, bufor, max_msg);
            continue;
        }

        else if (command=="set")
        {
             cin >> command;
            if (command=="server")
            {
                 cin >> command;
                if (command=="id")
                { int32_t id;
                     cin >> id;
                    bufor[0] = id; bufor[1]=bufor[2]=13;
                    bufor[3] = 31;
                    Send_and_recv(gniazdo, bufor, max_msg);

                }
            }
            //Send_and_recv(gniazdo, bufor, max_msg);
            continue;
        }
        else if (command=="send")
        {
            for (unsigned int i = 0; i < MAX_MSG_LEN; ++i)
            {
                 cin >> bufor[i];
            }
            Send_and_recv(gniazdo, bufor, max_msg);
        }
        else if (command=="help")
        {
            f_help();
        }

        else if (command=="spam")
        {
            int przerwa;
             cin >> przerwa;
            bufor[0]=bufor[2]=0;
            bufor[1]=bufor[3]=-1;
            int licznik =10000;
            while (licznik)
            {
                sleep (przerwa);
                Send_and_recv(gniazdo, bufor, max_msg);
                --licznik;
            }
            continue;
        }



        else
        {
             cout << "unknown command: "<<command<<" \nTry \"help\"\n\n";
            continue;
        }


    } while(go_while);
     cout << "Koniec";
    shutdown( gniazdo, SHUT_RDWR );
     cout << "." <<  endl;
    return 0;
}
