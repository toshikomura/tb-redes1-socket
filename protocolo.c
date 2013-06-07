#include "include.h"
#include "socket.h"
#include "protocolo.h"

/*-------------------------------------------------------------------------*/
/*                     PROTOCOLO DETERMINADO                               */
/*   MARCA   TAMANHO  SEQUENCIA  TIPO      DADO                      CRC   */
/*| 011110 | 4 BITS | 2 BITS   | 4 BITS | 0 a 15 BITS             | 8 BITS */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE CODIFICA O CABECALHO PARA ENVIAR COM OS BITS CORRETAMENTE    */
/*-------------------------------------------------------------------------*/
mensagem *Codifica_Cabecalho_Insere_Dados ( char *comando, int sequencia, int tipo, int *tamanho ) {

	char *dado = malloc ( sizeof ( char )*MAX_DADO );
	int marca = MARCA, tamanho_aux = 0, i, j = 0, tamanho_aux2;

	mensagem *msg = (mensagem *) malloc ( sizeof (msg) );

	msg->dado = malloc ( sizeof ( char )*MAX_DADO );

	tamanho_aux = 0;

	switch ( tipo ) {
		case MSG_CD:
				i = 3;
				break;
		case MSG_NACK:
				i = 0;
				break;
		case MSG_PUT:
				i = 4;
				break;
		case MSG_GET:
				i = 4;
				break;
	}

	if ( tipo != MSG_LS && tipo != MSG_ACK ) {
		// enquano nao chegar no final da string
		while ( comando[i] != '\0' ) {
			dado[j] = comando[i];
			j++;
			i++;
			tamanho_aux++;
		}
	}

	// coloca dado na mensagem
	for ( i = 0; i < tamanho_aux; i++ ) 
		msg->dado[i] = dado[i];
	for ( i = tamanho_aux; i < MAX_DADO; i++ ) 
		msg->dado[i] = 0;

	// atribuicao do primeiro byte
	marca = marca<<2;
	tamanho_aux2 = tamanho_aux>>2;
	msg->cabecalho[0] = marca + tamanho_aux2;

	// atribuicao do segundo byte
	tamanho_aux2 = tamanho_aux<<6;
	sequencia = sequencia<<4;
	msg->cabecalho[1] = tamanho_aux2 + sequencia + tipo;

	*tamanho = tamanho_aux;

	return msg;
}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE DECODIFICA O CABECALHO PARA CHECAR SE MENSAGEM CHEGOU CORRETA*/
/*-------------------------------------------------------------------------*/
int Decodifica_Cabecalho ( mensagem *msg, int *tamanho, int *sequencia, int *tipo ) {

	int marca, tamanho_aux;

	marca = msg->cabecalho[0]>>2;
	if ( marca != MARCA )
		return -1;

	*tamanho = msg->cabecalho[0] -( marca * 4);
	*tamanho = (*tamanho) * 4;
	tamanho_aux = msg->cabecalho[1]>>6;
	*tamanho = *tamanho + tamanho_aux;
	*sequencia = ( msg->cabecalho[1]>>4 ) - tamanho_aux * 4;
	*tipo = msg->cabecalho[1] - (tamanho_aux * 64) - ((*sequencia) * 16); 

	return 1;
}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE PASSA OS DADOS DA MENSAGEM PARA O BUFFER PARA ENVIAR         */
/*-------------------------------------------------------------------------*/
char *Mensagem_Buffer ( mensagem msg, int tamanho ) {

	char *buffer = (char *) malloc ( sizeof ( char )*MAX_MENSAGEM );
	int i;

	memcpy ( buffer, &( msg.cabecalho[0] ), sizeof ( char ) );
	memcpy ( buffer + sizeof ( char ), &( msg.cabecalho[1] ), sizeof ( char ) );
	for ( i = 0; i < MAX_MENSAGEM; i++ ) 
		memcpy ( buffer + sizeof ( char )*2 + i, &( msg.dado[i] ), sizeof ( char ) );
	memcpy ( buffer + sizeof ( char )*2 + MAX_DADO, &( msg.crc ), sizeof ( char ) );

	return buffer;
}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE PASSA OS DADOS DO BUFFER PARA A MENSAGEM NO DESTINO          */
/*-------------------------------------------------------------------------*/
mensagem *Buffer_Mensagem ( char *buffer ) {

	int *marca, *tamanho, *sequencia, *tipo;

	mensagem *msg = (mensagem *) malloc ( sizeof ( mensagem ) );
	memcpy( &( msg->cabecalho ), buffer, sizeof ( char )*2 );

	msg->dado = malloc ( sizeof ( char )*MAX_DADO );

	memcpy ( msg->dado, buffer + sizeof ( char )*2, MAX_DADO );
	memcpy ( &( msg->crc ), buffer + sizeof ( char )*2 + MAX_DADO, sizeof ( char ) );

	return msg;
}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE ENVIA UM ACK                                                 */
/*-------------------------------------------------------------------------*/
void Envia_ACK ( int servidor ) {

	char *buffer = ( char *) malloc ( sizeof ( char )*MAX_MENSAGEM );
	int *tamanho = malloc ( sizeof ( int ) ), 
		s;
	mensagem *msg = ( mensagem *) malloc ( sizeof ( mensagem ) );

	*tamanho = 0;
	msg = Codifica_Cabecalho_Insere_Dados ( NULL, 0, MSG_ACK, tamanho );	

	buffer = Mensagem_Buffer ( *msg, *tamanho );
	int k;
	for (k = 0; k< MAX_MENSAGEM; k++) 
		printf ("%d ", buffer[k]);
	printf ("\n");
	s = send ( servidor , buffer, (size_t) MAX_MENSAGEM, 0 );

}



/*-------------------------------------------------------------------------*/
/* FUNCAO QUE ENVIA UM NACK COM O ERRO                                     */
/*-------------------------------------------------------------------------*/
void Envia_ERRO ( int servidor, int ERRO ) {

	char *buffer = ( char *) malloc ( sizeof ( char )*MAX_MENSAGEM );
	int *tamanho = malloc ( sizeof ( int ) ), 
	s;
	mensagem *msg = ( mensagem *) malloc ( sizeof ( mensagem ) );

	*tamanho = 1;
	msg = Codifica_Cabecalho_Insere_Dados ( (char *) &ERRO, 0, MSG_ERRO, tamanho );	

	buffer = Mensagem_Buffer ( *msg, *tamanho );
	int k;
	for (k = 0; k< MAX_MENSAGEM; k++) 
		printf ("%d ", buffer[k]);
	printf ("\n");
	s = send ( servidor , buffer, (size_t) MAX_MENSAGEM, 0 );

}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE ESPERA PARA LER                                             */
/*-------------------------------------------------------------------------*/
int Esperando_Leitura ( int *meu_socket ) {

	fd_set fd;
	struct timeval timeout;
	int retorno, resultado;
	
	// 5 segundos de timeout
	timeout.tv_sec = TEMPO_ESPERA;
	timeout.tv_usec = 0;

	FD_ZERO ( &fd );
	FD_SET ( *meu_socket, &fd );

	retorno = select ( sizeof ( fd ), &fd, NULL, NULL, &timeout );

	if ( retorno == -1 ) 
		Mensagem_Erro ( (char *) "na função select" );

	// Se recebeu alguma coisa
	if ( retorno > 0 ) {
		if ( FD_ISSET ( *meu_socket, &fd ) )
			return 1;
	}

	return 0;
}


/*-------------------------------------------------------------------------*/
/* FUNCAO QUE ENVIA A MENSAGEM E ESPERA UMA RESPOSTA                       */
/*-------------------------------------------------------------------------*/
void Envia_Espera ( mensagem *msg, int servidor, int *tamanho ) {

	char *buffer = ( char *) malloc ( sizeof ( char )*MAX_MENSAGEM );
	int espera = 0, s, r;

	buffer = Mensagem_Buffer ( *msg, *tamanho );

	// Verifica o que vai ser enviado
	int k;
	for (k = 0; k< MAX_MENSAGEM; k++) 
		printf ("%d ", buffer[k]);
	printf ("\n");
	// Final Verifica o que vai ser enviado

	// timeout
	while ( espera == 0 ) {
		printf ( "espeando %d segundo(s)\n", TEMPO_ESPERA );
		s = send ( servidor , buffer, (size_t) MAX_MENSAGEM, 0 );
		espera = Esperando_Leitura ( &servidor );
	}

}
