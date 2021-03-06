#include "include.h"
#include "socket.h"

void Mensagem_Erro ( char *erro ) {
	printf ("Error %s\n", erro);
	exit ( 1 );
}


int Cria_Socket ( char *dispositivo ) {
	int meusocket, deviceid;
    struct ifreq ifre; // id do dispositivo
    struct sockaddr_ll sockll; // associacao
    struct packet_mreq mre; // para modo promisco

	// criando socket
	meusocket = socket ( AF_PACKET, SOCK_RAW, 0 );

	// teste de erro
	if ( meusocket == -1 ) 
		Mensagem_Erro ( (char *) "criar socket" );
	
	// pegando o ID do dispositivo
	memset ( &ifre, 0, sizeof ( struct ifreq ) );
	memcpy ( ifre. ifr_name, dispositivo, strlen( dispositivo ) );

	if ( ioctl ( meusocket, SIOCGIFINDEX, &ifre ) == -1 ) 
		Mensagem_Erro ( (char *) "no ioctl" );
	deviceid = ifre.ifr_ifindex;

	// associando dispositivo ao socket
	memset ( &sockll, 0, sizeof( sockll ) );

	sockll.sll_family = AF_PACKET;
	sockll.sll_ifindex = deviceid;
	sockll.sll_protocol = htons(ETH_P_ALL);

	if( bind ( meusocket, (struct sockaddr *) &sockll, sizeof( sockll ) ) == -1 ) 
		Mensagem_Erro ( (char *) "no bind" );

	// coloca socket em mode promiscuo para que possa receber todos os pacotes
	memset ( &mre, 0, sizeof( mre ) );

	mre.mr_ifindex = deviceid;
	mre.mr_type = PACKET_MR_PROMISC;

	if ( setsockopt ( meusocket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mre, sizeof( mre ) ) == -1 )
		Mensagem_Erro ( (char *) "no setsockopt" );

	return meusocket;

}
