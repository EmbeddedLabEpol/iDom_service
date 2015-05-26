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




#define MAX_MSG_LEN 18

void f_help()
{
    std::cout << "\n exit - end program\n stop server - stop server iDom and exit\n test - send test message\n send - send Send own message\n set server id - set new id\n send_to RS232 / NODE\n "<<std::endl;
}
void Send_and_recv (int &gniazdo ,float  bufor[MAX_MSG_LEN],int &max_msg)
{
    ///  to w funkcji dac
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        std::cout << bufor[i] << " ";
    }
    std::cout << " wysylam\n";
    if(( send( gniazdo, bufor, max_msg, MSG_DONTWAIT ) ) <= 0 ) // MSG_DONTWAIT
    {
        perror( "send() ERROR" );
        exit( - 1 );
    }

    //bzero( bufor, MAX_MSG_LEN );
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        bufor[i]=0;
    }

    std::cout << " przed recv \n";

    if(( recv( gniazdo, bufor, max_msg, 0 ) ) <= 0 )
    {
        perror( "recv() ERROR" );
        exit( - 1 );

    }
//ssize_t bytes_recieved = -1;

//while (bytes_recieved == -1){
//    bytes_recieved = recv( gniazdo, bufor, max_msg, MSG_DONTWAIT );

//    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
//    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
//    std::cout << bytes_recieved << " bytes recieved :" << std::endl;
//}

    std::cout << "|Wiadomosc z serwera|: "<< bufor[1]<< std::endl;
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        std::cout << bufor[i] << " ";
    }
    std::cout <<"\n"<< sizeof(float)<< " to wielkosc floata\n" ;
} // end Send_and_recv


////////////////////////////////////////   MAIN  //////////////////////////////////////////
int main( int argc, char ** argv )
{   if (argc <3)
    {   printf("\e[0;31m");//a teraz na czerwono
        std::cout << "Try: \e[0;1m"<< argv[0] << "\e[0;33m [ip_adres] [port]\n";
        printf("\e[0;0m");//powrot do normy
        return 0;
    }

    std::string command;
    bool go_while = true;
    struct sockaddr_in serwer;
    int gniazdo;
    float bufor[ MAX_MSG_LEN ];
    int max_msg = MAX_MSG_LEN*sizeof(float);

    bzero( & serwer, sizeof( serwer ) );
    //bzero( bufor, MAX_MSG_LEN );
    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        bufor[i]=0;
    }
      std::string s_ip;
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

    //std::cout << " string " << s_ip << std::endl;
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
    std::cout << " polaczono z serwerem \n";
    do {
        std::cout << "\nSet command: \n> ";
        std::cin >> command;

        if (command=="exit")
        {
            go_while = false;
            break;
        }
        else if (command=="stop")
        {
            std::cin >> command;
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
                std::cout << "unknown command "<< command << std::endl;
            }
        }

        else if (command=="send_to")
        {
            std::cin >> command;
            if (command=="RS232")
            {

                bufor[16]=11;
                int przerwa;
                std::cin >> przerwa;
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
                std::cin >> przerwa;
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
                std::cout << "unknown command "<< command << std::endl;
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
            std::cin >> command;
            if (command=="server")
            {
                std::cin >> command;
                if (command=="id")
                { float id;
                    std::cin >> id;
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
                std::cin >> bufor[i];
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
            std::cin >> przerwa;
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
            std::cout << "unknown command: "<<command<<" \nTry \"help\"\n\n";
            continue;
        }


    } while(go_while);
    std::cout << "Koniec";
    shutdown( gniazdo, SHUT_RDWR );
    std::cout << "." << std::endl;
    return 0;
}
