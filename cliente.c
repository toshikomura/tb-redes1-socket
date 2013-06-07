#include "include.h"
#include "socket.h"
#include "protocolo.h"

int cliente, sequencia;

/*-------------------------------------------------------------------------*/
/* FUNCAO QUE DESCOBRE O VALOR DO COMANDO PEDIDO                           */
/*-------------------------------------------------------------------------*/
int Procura_Valor ( char *comando ) {

	if ( comando[0] == 'c' && comando[1] == 'd' && comando[2] == ' ' ) 
		return MSG_CD;

	if ( comando[0] == 'l' && comando[1] == 's' && ( comando[2] == ' ' || comando[2] == '\0' ) ) 
		return MSG_LS;

	if ( comando[0] == 'g' && comando[1] == 'e' && comando[2] == 't' && comando[3] == ' ' ) 
		return MSG_GET;

	if ( comando[0] == 'p' && comando[1] == 'u' && comando[2] == 't' && comando[3] == ' ' ) 
		return MSG_PUT;
}


main () {

	char *comando, *buffer;
	int *tamanho, *tipo, 
		valor_comando, sequencia = 0, s, r;
	mensagem *msg;

	comando = malloc ( sizeof ( char )*MAX_MENSAGEM );
	buffer = malloc ( sizeof ( char )*MAX_MENSAGEM );
	tamanho = malloc ( sizeof ( int ) );
	tamanho = malloc ( sizeof ( int ) ); 

	cliente = Cria_Socket ( (char *) "eth0" );

	while ( 1 ) {
		scanf ( " %[^\n]", comando );
		printf ("o comando é %s\n", comando );
		valor_comando = Procura_Valor ( comando );
		msg = Codifica_Cabecalho_Insere_Dados ( comando, sequencia, valor_comando, tamanho );	
		Envia_Espera ( msg, cliente ,tamanho );

		switch ( valor_comando ) {
			case MSG_CD:
					printf ( "cd\n" );
					r = recv ( cliente, buffer, (size_t) MAX_MENSAGEM, 0 );
					msg = Buffer_Mensagem ( buffer );

					if ( Decodifica_Cabecalho ( msg, tamanho, &sequencia, tipo ) == 1 ) {
						if ( *tipo == MSG_ERRO ) {

							switch ( msg->dado[0] ) {
								case DIR_INEXISTENTE:
										printf ( "Diretório Inexistente\n" );
										break;
								case PERMISSAO_NEGADA:
										printf ( "Permissão Negada\n" );
										break;
							}

						}

					}
					break;

			case MSG_LS: 
					printf ( "ls\n" );
					break;

			case MSG_GET:
					printf ( "get\n" );
					break;

			case MSG_PUT:
					printf ( "put\n" );
					break;

			default:
					printf ( "comando inexistente\n" );
					break;

		} 

	}
}
