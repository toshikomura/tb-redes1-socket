#define MSG_CD 0
#define MSG_ACK 1
#define MSG_NACK 2
#define MSG_LS 3
#define MSG_PRINT 4
#define MSG_PUT 5
#define MSG_GET 6
#define MSG_ATRIBUTO 10
#define MSG_DADOS 13
#define MSG_ERRO 14
#define MSG_FIM 15

#define MAX_MENSAGEM 18
#define MAX_DADO 15

#define DIR_INEXISTENTE 0
#define PERMISSAO_NEGADA 1
#define ESPACO_INSUFICIENTE 2
#define ARQ_INEXISTENTE 3

#define MARCA 30
#define GERADOR 127

#define TEMPO_ESPERA 5

typedef struct mensagem {
	unsigned char cabecalho[2];
	unsigned char *dado;
	unsigned char crc;
} mensagem;

void Calcula_CRC ( mensagem *msg );
mensagem *Codifica_Cabecalho_Insere_Dados ( char *comando, int sequencia, int tipo, int *tamanho );
int Decodifica_Cabecalho ( mensagem *msg, int *tamanho, int *sequencia, int *tipo );
char *Mensagem_Buffer ( mensagem msg, int tamanho );
mensagem *Buffer_Mensagem ( char *buffer );
void Envia_ACK ( int servidor );
void Envia_ERRO ( int servidor, int ERRO );
int Esperando_Leitura ( int *meu_socket );
void Envia_Espera ( mensagem *msg, int servidor, int *tamanho );
