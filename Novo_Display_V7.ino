// V4 - Criadas novas variáveis fimEnviado e fimRecebido
//      Fixado o tamanho dos vetores dados[] e comandos[] em 30
//      Novo método para finalizar o processo

// V6 - Salvar e exibir os dados de produção
//      Nova variável => produçãoAtualizada

// V7 - Salvar e exibir os tempos dos motores
//      Nova variável => temposAtualizados

// V8 - Exibir a corrente da planta na tela de acionamentos
//      Nova variável de comando, verificaCorrente

// V9 - Leitura da TAG RFID

#include <Nextion.h>                //Biblioteca do display LCD
#include <Wire.h>                   //Biblioteca do I2C
#include <EEPROM.h>                 //Biblioteca da EEPROM do arduino

//Qual o intervalo de atualização dos dados do display(ms)?

 #define intervalo                1000
 long int     ultimaAtualizacao;

//Definições dos dados a serem trocados com o escravo

 #define slaveAdress              10
 
 #define setB1VM                  comandos[0]
 #define setB1AZ                  comandos[1]
 #define setB1VD                  comandos[2]
 
 #define setB2VM                  comandos[3]
 #define setB2AZ                  comandos[4]
 #define setB2VD                  comandos[5]
 
 #define setB3VM                  comandos[6]
 #define setB3AZ                  comandos[7]
 #define setB3VD                  comandos[8]

 #define acionaS1                 comandos[9]
 #define acionaS2                 comandos[10]
 #define acionaS3                 comandos[11]
 #define acionaS4                 comandos[12]
 #define acionaS5                 comandos[13]

 #define acionaEsteira            comandos[14]
 #define acionaPasso              comandos[15]
 #define acionaPassoCompleto      comandos[16]

 #define iniciaProcesso           comandos[17]

 #define lerTag                   comandos[18]

 #define fimRecebido              comandos[19]

 #define acionamentosAtualizados  comandos[20]

 #define producaoAtualizada       comandos[21]

 #define temposAtualizados        comandos[22]

 #define verificarCorrente        comandos[23]
 
 byte comandos[31];
 
 #define atualB1VM                  dados[0]
 #define atualB1AZ                  dados[1]
 #define atualB1VD                  dados[2]
 
 #define atualB2VM                  dados[3]
 #define atualB2AZ                  dados[4]
 #define atualB2VD                  dados[5]
 
 #define atualB3VM                  dados[6]
 #define atualB3AZ                  dados[7]
 #define atualB3VD                  dados[8]
 
 #define contS1                     dados[9]
 #define contS2                     dados[10]
 #define contS3                     dados[11]
 #define contS4                     dados[12]
 #define contS5                     dados[13]
  
 #define tempoPasso                 dados[14]
 #define tempoEsteira               dados[15]
 
 #define rfid1                      dados[16]
 #define rfid2                      dados[17]
 
 #define rfid3                      dados[18]
 #define rfid4                      dados[19] 
 
 #define vazio1                     dados[20]
 #define vazio2                     dados[21]
 
 #define ampereBLOW                 dados[22]
 #define ampereBHIGH                dados[23]
 
 #define fimEnviado                 dados[24]

 #define incVmProd                  dados[25]
 #define incAzProd                  dados[26]
 #define incVdProd                  dados[27]
 #define incVmRet                   dados[28]
 #define incAzRet                   dados[29]
 #define incVdRet                   dados[30]

 byte dados[31];

// --- Endereçamento das memórias EEPROM

const int    mem_prod_vm = 20,                                //Esferas produzidas
             mem_prod_az = 21,
             mem_prod_vd = 22,

             mem_retr_vm = 30,                                  //Esferas retrabalhadas
             mem_retr_az = 31,
             mem_retr_vd = 32,

             mem_acio_s1 = 40,                                //Numero e tempo de acionamentos
             mem_acio_s2 = 41,
             mem_acio_s3 = 42,
             mem_acio_s4 = 43,
             mem_acio_s5 = 44,

             mem_acio_esteira_s = 50,                         //Reserva três bytes da memória EEPROM para guardar os tempos
             mem_acio_esteira_m = 51,
             mem_acio_esteira_h = 52,

             mem_acio_passo_s = 60,
             mem_acio_passo_m = 61,
             mem_acio_passo_h = 62;


// ===========================================================================================================
// --- Declaração de variáveis globais ---

//String nomeCliente = "";

char nomeCliente[10];
int auxDigTexto = 0;

bool setCliente1 = LOW,
     setCliente2 = LOW,
     setCliente3 = LOW;

bool pageAbertura = LOW,
     pageHome = LOW,
     pagePersonalizado = LOW,
     pageSerie = LOW,
     pageManut = LOW,
     pageAcionamentos = LOW,
     pageAguarde = LOW,
     pageProduzindo = LOW,
     pageFinalizado = LOW,
     pageProducao = LOW,
     pageUtilizacao = LOW,
     pageCadastro = LOW,
     pageEmergencia = LOW,
     pagePausa = LOW;
     
// ===========================================================================================================
// --- Declaração de Objetos do Display Nextion---

//Texto
NexNumber CORRENTE_DC  = NexNumber(5, 16, "n0");

//Páginas
NexPage ABERTURA       = NexPage(0, 0, "ABERTURA");                //Pagina de iniciação do display
NexPage HOME           = NexPage(1, 0, "HOME");                    //Página onde é possível selecionar o tipo de operação
NexPage PERSONALIZADO  = NexPage(2, 0, "PERSONALIZADO");           //Página onde é possível produzir de forma personalizada
NexPage SERIE          = NexPage(3, 0, "SERIE");                   //Página onde é dada a ordem de produção em série
NexPage MANUT          = NexPage(4, 0, "MANUT");                   //Página que disponibiliza diversas opções de monitoramento
NexPage ACIONAMENTOS   = NexPage(5, 0, "ACIONAMENTOS");            //Página onde é possível assionar todos os elementos da planta um a um
NexPage AGUARDE        = NexPage(6, 0, "AGUARDE");                 //Página com a mensagem para aguardar(Não está sendo utilizada no momento)
NexPage PRODUZINDO     = NexPage(7, 0, "PRODUZINDO");              //Pagina mostrada enquanto a plata separa as peças selecionadas
NexPage FINALIZADO     = NexPage(8, 0, "FINALIZADO");              //Pagina mostrada enquanto a plata separa as peças selecionadas
NexPage PRODUCAO       = NexPage(9, 0, "PRODUCAO");                //Pagina mostrada enquanto a plata separa as peças selecionadas
NexPage UTILIZACAO     = NexPage(10, 0, "UTILIZACAO");             //Pagina que informa os dados de utilização dos atuadores
NexPage CADASTRO       = NexPage(11, 0, "CADASTRO");               //Pagina onde é informado o nome dos clientes
NexPage EMERGENCIA     = NexPage(12, 0, "EMERGENCIA");             //Pagina de desvio na rotina de emergência
NexPage PAUSA          = NexPage(13, 0, "PAUSA");                  //Pagina de desvio na rotina de pausa

//---------------------Variáveis do display

//Controle de número de peças a serem produziadas
NexNumber VM1        = NexNumber(2, 8, "n2");
NexNumber AZ1        = NexNumber(2, 5, "n1");
NexNumber VD1        = NexNumber(2, 4, "n0");
NexNumber VM2        = NexNumber(2, 13, "n3");
NexNumber AZ2        = NexNumber(2, 16, "n4");
NexNumber VD2        = NexNumber(2, 19, "n5");
NexNumber VM3        = NexNumber(2, 23, "n6");
NexNumber AZ3        = NexNumber(2, 26, "n7");
NexNumber VD3        = NexNumber(2, 19, "n8");

//Exibição da corrente dos motores
NexNumber I_KIT        = NexNumber(5, 16, "I0");       //Corrente consumida pelo kit
//NexNumber I_PAS        = NexNumber(5, 17, "n1");

//Controle de peças que já foram produzidas

NexNumber P_VM1        = NexNumber(7, 4, "n2");
NexNumber P_AZ1        = NexNumber(7, 3, "n1");
NexNumber P_VD1        = NexNumber(7, 2, "n0");
NexNumber P_VM2        = NexNumber(7, 7, "n3");
NexNumber P_AZ2        = NexNumber(7, 8, "n4");
NexNumber P_VD2        = NexNumber(7, 9, "n5");
NexNumber P_VM3        = NexNumber(7, 11, "n6");
NexNumber P_AZ3        = NexNumber(7, 12, "n7");
NexNumber P_VD3        = NexNumber(7, 13, "n8");

//Valores totais de peças produzidas

NexNumber PRO_VM        = NexNumber(8, 15, "n0");
NexNumber PRO_AZ        = NexNumber(8, 16, "n1");
NexNumber PRO_VD        = NexNumber(8, 17, "n2");

//Valores totais de peças retrabalhadas

NexNumber RET_VM        = NexNumber(8, 2, "n3");
NexNumber RET_AZ        = NexNumber(8, 3, "n4");
NexNumber RET_VD        = NexNumber(8, 4, "n5");

//Valores do número de acionamentos

NexNumber ACIO_S1        = NexNumber(10, 4, "n0");                      
NexNumber ACIO_S2        = NexNumber(10, 5, "n1");
NexNumber ACIO_S3        = NexNumber(10, 6, "n2");
NexNumber ACIO_S4        = NexNumber(10, 7, "n3");
NexNumber ACIO_S5        = NexNumber(10, 8, "n4");

//Tempo acionado dos motores
NexNumber SG_EST         = NexNumber(10, 12, "n8");                      
NexNumber MN_EST         = NexNumber(10, 11, "n7");
NexNumber HR_EST         = NexNumber(10, 9,  "n5");

NexNumber SG_PASSO       = NexNumber(10, 15, "n10");
NexNumber MN_PASSO       = NexNumber(10, 14, "n9");
NexNumber HR_PASSO       = NexNumber(10, 13, "n6");

//Nomes dos clientes cadastrados

NexText CLI_01           = NexText(2, 34, "t0");
NexText CLI_02           = NexText(2, 35, "t1");
NexText CLI_03           = NexText(2, 36, "t2");

NexText CLI_NOME         = NexText(11, 3, "t0");

//Código do cartão RFID

NexNumber ID0       = NexNumber(11, 33, "n0");
NexNumber ID1       = NexNumber(11, 34, "n1");
NexNumber ID2       = NexNumber(11, 35, "n2");
NexNumber ID3       = NexNumber(11, 36, "n3");

NexNumber CARD1_0       = NexNumber(2, 38, "n9");
NexNumber CARD1_1       = NexNumber(2, 39, "n10");
NexNumber CARD1_2       = NexNumber(2, 40, "n11");
NexNumber CARD1_3       = NexNumber(2, 41, "n12");

NexNumber CARD2_0       = NexNumber(2, 42, "n13");
NexNumber CARD2_1       = NexNumber (2, 43, "n14");
NexNumber CARD2_2       = NexNumber(2, 44, "n15");
NexNumber CARD2_3       = NexNumber(2, 45, "n16");

NexNumber CARD3_0       = NexNumber(2, 46, "n17");
NexNumber CARD3_1       = NexNumber(2, 47, "n18");
NexNumber CARD3_2       = NexNumber(2, 48, "n19");
NexNumber CARD3_3       = NexNumber(2, 49, "n20");

//--------------------------------------Botões do display

//Página ABERTURA
NexPicture IMG_LOGO = NexPicture(0, 3, "p2");                     //Logo da indústria 4.0

//Página HOME
NexButton BOT_PADRAO = NexButton(1, 4, "b2");               //Botão "Prod. Padrão"
NexButton BOT_PERSONALIZAR = NexButton(1, 2, "b0");         //Botão "Personalização"
NexButton BOT_MANUT_INFO = NexButton(1, 3, "b1");           //Botão "Manut./ Info."

//Página Personalização
NexText CLIENTE_1 = NexText(2, 34, "t0");                   //Texto "Cliente 1"
NexText CLIENTE_2 = NexText(2, 35, "t1");                   //Texto "Cliente 2"
NexText CLIENTE_3 = NexText(2, 36, "t2");                   //Texto "Cliente 3"

NexButton INC_VM_1 = NexButton(2, 9, "b4");                 //Botão "Incrementa vermelho BOX 1"
NexButton INC_VM_2 = NexButton(2, 12, "b6");                //Botão "Incrementa vermelho BOX 2"
NexButton INC_VM_3 = NexButton(2, 21, "b12");               //Botão "Incrementa vermelho BOX 3"

NexButton INC_AZ_1 = NexButton(2, 6, "b2");                 //Botão "Incrementa azul BOX 1"
NexButton INC_AZ_2 = NexButton(2, 15, "b8");                //Botão "Incrementa azul BOX 2"
NexButton INC_AZ_3 = NexButton(2, 24, "b14");               //Botão "Incrementa azul BOX 3"

NexButton INC_VD_1 = NexButton(2, 2, "b0");                 //Botão "Incrementa verde BOX 1"
NexButton INC_VD_2 = NexButton(2, 18, "b10");               //Botão "Incrementa verde BOX 2"
NexButton INC_VD_3 = NexButton(2, 27, "b16");               //Botão "Incrementa verde BOX 3"

NexButton DEC_VM_1 = NexButton(2, 10, "b5");                //Botão "Decrementa vermelho BOX 1"
NexButton DEC_VM_2 = NexButton(2, 13, "b7");                //Botão "Decrementa vermelho BOX 2"
NexButton DEC_VM_3 = NexButton(2, 22, "b13");               //Botão "Decrementa vermelho BOX 3"

NexButton DEC_AZ_1 = NexButton(2, 7, "b3");                 //Botão "Decrementa azul BOX 1"
NexButton DEC_AZ_2 = NexButton(2, 16, "b9");                //Botão "Decrementa azul BOX 2"
NexButton DEC_AZ_3 = NexButton(2, 25, "b15");               //Botão "Decrementa azul BOX 3"

NexButton DEC_VD_1 = NexButton(2, 3, "b1");                 //Botão "Decrementa verde BOX 1"
NexButton DEC_VD_2 = NexButton(2, 19, "b11");               //Botão "Decrementa verde BOX 2"
NexButton DEC_VD_3 = NexButton(2, 28, "b17");               //Botão "Decrementa verde BOX 3"

NexButton BOT_CONFIRMA = NexButton(2, 29, "b18");           //Botão Confirma
NexButton BOT_RET_PG2 = NexButton(2, 30, "b19");            //Botão Retorna

//Página SERIE
NexButton BOT_INICIAR = NexButton(3, 2, "b2");              //Botão Iniciar
NexButton BOT_RET_PG3 = NexButton(3, 3, "b19");             //Botão Retorna

//Página MANUT
NexButton BOT_ATUADORES = NexButton(4, 2, "b1");            //Botão Atuadores
NexButton BOT_PRODUCAO = NexButton(4, 4, "b0");             //Botão Dados de produção
NexButton BOT_MANUT = NexButton(4, 5, "b2");                //Botão Dados de manutenção
NexButton BOT_RET_PG4 = NexButton(4, 3, "b19");             //Botão Retorna

//Página ACIONAMENTOS
NexButton BOT_ON_S1 = NexButton(5, 2, "b1");               //Botão aciona S1
NexButton BOT_ON_S2 = NexButton(5, 4, "b2");               //Botão aciona S2
NexButton BOT_ON_S3 = NexButton(5, 5, "b3");               //Botão aciona S3
NexButton BOT_ON_S4 = NexButton(5, 6, "b4");               //Botão aciona S4
NexButton BOT_ON_S5 = NexButton(5, 7, "b5");               //Botão aciona S5
NexButton BOT_ON_EST = NexButton(5, 12, "b10");            //Botão aciona esteira

NexButton BOT_OFF_S1 = NexButton(5, 3, "b0");              //Botão desliga S1
NexButton BOT_OFF_S2 = NexButton(5, 8, "b6");              //Botão desliga S2
NexButton BOT_OFF_S3 = NexButton(5, 9, "b7");              //Botão desliga S3
NexButton BOT_OFF_S4 = NexButton(5, 10, "b8");             //Botão desliga S4
NexButton BOT_OFF_S5 = NexButton(5, 11, "b9");             //Botão desliga S5
NexButton BOT_OFF_EST = NexButton(5, 13, "b11");           //Botão desliga esteira

NexButton BOT_PASSO = NexButton(5, 14, "b12");             //Botão Aciona Passo
NexButton BOT_RET_PG5 = NexButton(5, 15, "b19");           //Botão Retorna

//Página PRODUZINDO
NexButton BOT_RET_PG7 = NexButton(7, 14, "b19");           //Botão Retorna

//Página FINALIZADO
NexButton BOT_NOVO = NexButton(8, 12, "b0");               //Botão Novo pedido
NexButton BOT_MENU_INICIAL = NexButton(8, 13, "b1");       //Botão Menu inicial

//Página PRODUÇÃO
NexButton BOT_RET_PG9 = NexButton(9, 5, "b19");            //Botão Retorna

//Página UTILIZAÇÃO
NexButton BOT_RET_PG10 = NexButton(10, 2, "b19");          //Botão Retorna

//Página CADASTRO
NexButton BOT_LER_TAG = NexButton(11, 32, "b0");           //Botão Ler TAG

NexHotspot LET_Q = NexHotspot(11, 4, "m0");                //Letra Q
NexHotspot LET_W = NexHotspot(11, 5, "m1");                //Letra W
NexHotspot LET_E = NexHotspot(11, 6, "m2");                //Letra E
NexHotspot LET_R = NexHotspot(11, 7, "m3");                //Letra R
NexHotspot LET_T = NexHotspot(11, 8, "m4");                //Letra T
NexHotspot LET_Y = NexHotspot(11, 9, "m5");                //Letra Y
NexHotspot LET_U = NexHotspot(11, 10, "m6");               //Letra U
NexHotspot LET_I = NexHotspot(11, 11, "m7");               //Letra I
NexHotspot LET_O = NexHotspot(11, 12, "m8");               //Letra O
NexHotspot LET_P = NexHotspot(11, 13, "m9");               //Letra P

NexHotspot LET_A = NexHotspot(11, 22, "m18");              //Letra A
NexHotspot LET_S = NexHotspot(11, 21, "m17");              //Letra S
NexHotspot LET_D = NexHotspot(11, 20, "m16");              //Letra D
NexHotspot LET_F = NexHotspot(11, 19, "m15");              //Letra F
NexHotspot LET_G = NexHotspot(11, 18, "m14");              //Letra G
NexHotspot LET_H = NexHotspot(11, 17, "m13");              //Letra H
NexHotspot LET_J = NexHotspot(11, 16, "m12");              //Letra J
NexHotspot LET_K = NexHotspot(11, 15, "m11");              //Letra K
NexHotspot LET_L = NexHotspot(11, 14, "m10");              //Letra L

NexHotspot LET_Z = NexHotspot(11, 23, "m19");              //Letra Z
NexHotspot LET_X = NexHotspot(11, 24, "m20");              //Letra X
NexHotspot LET_C = NexHotspot(11, 25, "m21");              //Letra C
NexHotspot LET_V = NexHotspot(11, 26, "m22");              //Letra V
NexHotspot LET_B = NexHotspot(11, 27, "m23");              //Letra B
NexHotspot LET_N = NexHotspot(11, 28, "m24");              //Letra N
NexHotspot LET_M = NexHotspot(11, 29, "m25");              //Letra M

NexHotspot LET_ENTER = NexHotspot(11, 31, "m27");          //Enter
NexHotspot LET_ESPACO = NexHotspot(11, 30, "m26");         //Espaço

//======================================================================================
// Criação do Buffer para interações com o display

char buffer[10] = {0};

//======================================================================================
// Criação da lista de monitoramento do display

NexTouch *nex_Listen_List[] = 
{
    &IMG_LOGO,
    
    &BOT_PADRAO,
    &BOT_PERSONALIZAR,
    &BOT_MANUT_INFO,

    &CLIENTE_1,
    &CLIENTE_2,
    &CLIENTE_3,

    &INC_VM_1,
    &INC_VM_2,
    &INC_VM_3,

    &INC_AZ_1,
    &INC_AZ_2,
    &INC_AZ_3,

    &INC_VD_1,
    &INC_VD_2,
    &INC_VD_3,

    &DEC_VM_1,
    &DEC_VM_2,
    &DEC_VM_3,

    &DEC_AZ_1,
    &DEC_AZ_2,
    &DEC_AZ_3,

    &DEC_VD_1,
    &DEC_VD_2,
    &DEC_VD_3,

    &BOT_CONFIRMA,
    &BOT_RET_PG2,

    &BOT_INICIAR,
    &BOT_RET_PG3,

    &BOT_ATUADORES,
    &BOT_PRODUCAO,
    &BOT_MANUT,
    &BOT_RET_PG4,

    &BOT_ON_S1,
    &BOT_ON_S2,
    &BOT_ON_S3,
    &BOT_ON_S4,
    &BOT_ON_S5,
    &BOT_ON_EST,

    &BOT_OFF_S1,
    &BOT_OFF_S2,
    &BOT_OFF_S3,
    &BOT_OFF_S4,
    &BOT_OFF_S5,
    &BOT_OFF_EST,

    &BOT_PASSO,
    &BOT_RET_PG5,

    &BOT_NOVO,
    &BOT_MENU_INICIAL,

    &BOT_RET_PG7,

    &BOT_RET_PG9,
    
    &BOT_RET_PG10,

    &BOT_LER_TAG,

    &LET_Q,
    &LET_W,
    &LET_E,
    &LET_R,
    &LET_T,
    &LET_Y,
    &LET_U,
    &LET_I,
    &LET_O,
    &LET_P,
    
    &LET_A,
    &LET_S,
    &LET_D,
    &LET_F,
    &LET_G,
    &LET_H,
    &LET_J,
    &LET_K,
    &LET_L,
    
    &LET_Z,
    &LET_X,
    &LET_C,
    &LET_V,
    &LET_B,
    &LET_N,
    &LET_M,

    &LET_ENTER,
    &LET_ESPACO,    
    NULL
};

//=======================================================================================
//Rotina de chamadas do display

//----------------------------Tela de abertura-------------------------------------------
void pushImgLogo(void *ptr)
{
  HOME.show();  
  limpaPaginas();   
  pageHome = HIGH;                                         
}

//----------------------------Tela HOME--------------------------------------------------
void pushBotPadrao(void *ptr)
{
  SERIE.show();
  limpaPaginas();
  pageSerie = HIGH;
}

void pushBotPersonalizar(void *ptr)
{
  PERSONALIZADO.show();
  limpaPaginas();
  pagePersonalizado = HIGH;
}

void pushBotManutInfo(void *ptr)
{
  MANUT.show();
  limpaPaginas();
  pageManut = HIGH;
}

//---------------------------Tela Personalizado-------------------------------------------

void pushCliente1(void *ptr)
{
 setCliente1 = HIGH;                                                                      //seta a variável se acesso ao cliente 
 limpaNomeCliente();
 limpaPaginas();
 pageCadastro = HIGH;
 CADASTRO.show();                                                                         //Abre a tela para a digitação
}
void pushCliente2(void *ptr)
{
 setCliente2 = HIGH;
 limpaNomeCliente();
 limpaPaginas();
 pageCadastro = HIGH;
 CADASTRO.show(); 
}
void pushCliente3(void *ptr)
{
 setCliente3 = HIGH;
 limpaNomeCliente();
 CADASTRO.show();
 limpaPaginas();
 pageCadastro = HIGH;
 CADASTRO.show(); 
}

void pushIncVm1(void *ptr)
{
 uint32_t     vmAtual;
 VM1.getValue(&vmAtual);
 vmAtual++;
 VM1.setValue(vmAtual);
 setB1VM = vmAtual;
}
void pushIncVm2(void *ptr)
{
 uint32_t     vmAtual;
 VM2.getValue(&vmAtual);
 vmAtual++;
 VM2.setValue(vmAtual);
 setB2VM = vmAtual;
}
void pushIncVm3(void *ptr)
{
 uint32_t     vmAtual;
 VM3.getValue(&vmAtual);
 vmAtual++;
 VM3.setValue(vmAtual); 
 setB3VM = vmAtual;
}

void pushIncAz1(void *ptr)
{
 uint32_t     azAtual;
 AZ1.getValue(&azAtual);
 azAtual++;
 AZ1.setValue(azAtual); 
 setB1AZ = azAtual;
}
void pushIncAz2(void *ptr)
{
 uint32_t     azAtual;
 AZ2.getValue(&azAtual);
 azAtual++;
 AZ2.setValue(azAtual); 
 setB2AZ = azAtual;
}
void pushIncAz3(void *ptr)
{
 uint32_t     azAtual;
 AZ3.getValue(&azAtual);
 azAtual++;
 AZ3.setValue(azAtual); 
 setB3AZ = azAtual;
}

void pushIncVd1(void *ptr)
{
 uint32_t     vdAtual;
 VD1.getValue(&vdAtual);
 vdAtual++;
 VD1.setValue(vdAtual); 
 setB1VD = vdAtual;
}
void pushIncVd2(void *ptr)
{
 uint32_t     vdAtual;
 VD2.getValue(&vdAtual);
 vdAtual++;
 VD2.setValue(vdAtual); 
 setB2VD = vdAtual;
}
void pushIncVd3(void *ptr)
{
 uint32_t     vdAtual;
 VD3.getValue(&vdAtual);
 vdAtual++;
 VD3.setValue(vdAtual); 
 setB3VD = vdAtual;
}

void pushDecVm1(void *ptr)
{
 uint32_t     vmAtual;
 VM1.getValue(&vmAtual);
 if(vmAtual>0) vmAtual--;
 VM1.setValue(vmAtual); 
 setB1VM = vmAtual;
}
void pushDecVm2(void *ptr)
{
 uint32_t     vmAtual;
 VM2.getValue(&vmAtual);
 if(vmAtual>0) vmAtual--;
 VM2.setValue(vmAtual);  
 setB2VM = vmAtual;
}
void pushDecVm3(void *ptr)
{
 uint32_t     vmAtual;
 VM3.getValue(&vmAtual);
 if(vmAtual>0) vmAtual--;
 VM3.setValue(vmAtual); 
 setB3VM = vmAtual; 
}

void pushDecAz1(void *ptr)
{
 uint32_t     azAtual;
 AZ1.getValue(&azAtual);
 if(azAtual>0) azAtual--;
 AZ1.setValue(azAtual);  
 setB1AZ = azAtual; 
}
void pushDecAz2(void *ptr)
{
  uint32_t     azAtual;
 AZ2.getValue(&azAtual);
 if(azAtual>0) azAtual--;
 AZ2.setValue(azAtual); 
 setB2AZ = azAtual;  
}
void pushDecAz3(void *ptr)
{
 uint32_t     azAtual;
 AZ3.getValue(&azAtual);
 if(azAtual>0) azAtual--;
 AZ3.setValue(azAtual);  
 setB3AZ = azAtual; 
}

void pushDecVd1(void *ptr)
{
 uint32_t     vdAtual;
 VD1.getValue(&vdAtual);
 if(vdAtual>0) vdAtual--;
 VD1.setValue(vdAtual); 
 setB1VD = vdAtual;  
}
void pushDecVd2(void *ptr)
{
 uint32_t     vdAtual;
 VD2.getValue(&vdAtual);
 if(vdAtual>0) vdAtual--;
 VD2.setValue(vdAtual);
 setB2VD = vdAtual;  
}
void pushDecVd3(void *ptr)
{
 uint32_t     vdAtual;
 VD3.getValue(&vdAtual);
 if(vdAtual>0) vdAtual--;
 VD3.setValue(vdAtual);
 setB3VD = vdAtual;  
}

void pushBotConfirma(void *ptr)
{

  //Confirma as quantidades de todas as caixas
  uint32_t valor;
  
  VM1.getValue(&valor);
  setB1VM = valor;
  VM2.getValue(&valor);
  setB2VM = valor;
  VM3.getValue(&valor);
  setB3VM = valor;

  AZ1.getValue(&valor);
  setB1AZ = valor;
  AZ2.getValue(&valor);
  setB2AZ = valor;
  AZ3.getValue(&valor);
  setB3AZ = valor;

  VD1.getValue(&valor);
  setB1VD = valor;
  VD2.getValue(&valor);
  setB2VD = valor;
  VD3.getValue(&valor);
  setB3VD = valor;
  
  limpaComandos();
  iniciaProcesso = 1;  
  transmitirComandos();
  iniciaProcesso = 0;
  PRODUZINDO.show();
  limpaPaginas();
  pageProduzindo = HIGH;
  
  
}
void pushBotRetPg2(void *ptr)
{
  HOME.show();
  limpaPaginas();
  pageHome = HIGH;
}

//----------------------------------Tela SERIE----------------------------------------------------------
void pushBotIniciar(void *ptr)
{
  
}
void pushBotRetPg3(void *ptr)
{
  HOME.show();
  limpaPaginas();
  pageHome = HIGH;
}

//-----------------------------------Tela MANUT----------------------------------------------------------
void pushBotAtuadores(void *ptr)
{
  ACIONAMENTOS.show();
  limpaPaginas();
  pageAcionamentos = HIGH;
  verificarCorrente = 1;
}
void pushBotProducao(void *ptr)
{
  PRODUCAO.show();
  limpaPaginas();
  pageProducao = HIGH;
  
  
}
void pushBotManut(void *ptr)
{
  UTILIZACAO.show();
  limpaPaginas();
  pageUtilizacao = HIGH;
}
void pushBotRetPg4(void *ptr)
{
  HOME.show();
  limpaPaginas();
  pageHome = HIGH;
}

//--------------------------------------Tela ACIONAMENTOS--------------------------------------------------------
void pushBotOnS1(void *ptr)
{
  acionaS1 = 1;
  transmitirComandos();
}
void pushBotOnS2(void *ptr)
{
  acionaS2 = 1;
  transmitirComandos();
}
void pushBotOnS3(void *ptr)
{
  acionaS3 = 1;
  transmitirComandos();
}
void pushBotOnS4(void *ptr)
{
  acionaS4 = 1;
  transmitirComandos();
}
void pushBotOnS5(void *ptr)
{
  acionaS5 = 1;
  transmitirComandos();
}
void pushBotOnEst(void *ptr)
{
  acionaEsteira = 1;
  transmitirComandos();
}

void pushBotOffS1(void *ptr)
{
  acionaS1 = 0;
  transmitirComandos();
}
void pushBotOffS2(void *ptr)
{
  acionaS2 = 0;
  transmitirComandos();
}
void pushBotOffS3(void *ptr)
{
  acionaS3 = 0;
  transmitirComandos();
}
void pushBotOffS4(void *ptr)
{
  acionaS4 = 0;
  transmitirComandos();
}
void pushBotOffS5(void *ptr)
{
  acionaS5 = 0;
  transmitirComandos();
}
void pushBotOffEst(void *ptr)
{
  acionaEsteira = 0;
  transmitirComandos();
}

void pushBotPasso(void *ptr)
{
  acionaPasso = 1;
  transmitirComandos();
  acionaPasso = 0;
}
void pushBotRetPg5(void *ptr)
{
  MANUT.show();
  limpaPaginas();
  pageManut = HIGH;
}

//-----------------------------------------Tela FINALIZADO----------------------------------
void pushBotNovo(void *ptr)
{
  PERSONALIZADO.show();
  limpaPaginas();
  pagePersonalizado = HIGH;
}
void pushBotMenuInicial(void *ptr)
{
  HOME.show();
  limpaPaginas();
  pageHome = HIGH;
}

//-----------------------------------------Tela PRODUZINDO-----------------------------------
void pushBotRetPg7(void *ptr)
{
  limpaPaginas();
  pageHome = HIGH;  
  HOME.show();
  
}

//-----------------------------------------Tela PRODUCAO-----------------------------------
void pushBotRetPg9(void *ptr)
{
  MANUT.show();
  limpaPaginas();
  pageManut = HIGH;
}

//-----------------------------------------Tela UTILIZACAO---------------------------------   
void pushBotRetPg10(void *ptr)
{
  MANUT.show();
  limpaPaginas();
  pageManut = HIGH;
}

//------------------------------------------Tela CADASTRO
void limpaNomeCliente(){
  for(int i=0; i<sizeof(nomeCliente); i++){                                                //Limpa os caracteres do texto anterior
  nomeCliente[i] = ' ';
 }
 auxDigTexto = 0;                                                                         //Reseta a variável de posicionamento para a escrita
}

void atualizaTexto(){
  auxDigTexto++;                                                                          //Incrementa a variável de auxílio para posicionamento de caractere
  CLI_NOME.setText(nomeCliente);                                                          //Envia o texto digitado até então para o display
}


void pushBotLerTag(void *ptr)
{
  lerTag = 1;
  rfid1 = 0;
  rfid2 = 0;
  rfid3 = 0;
  rfid4 = 0;
  transmitirComandos();
  lerTag = 0;
}
void pushLetQ(void *ptr)
{
  nomeCliente[auxDigTexto] = 'Q';
  atualizaTexto();
}
void pushLetW(void *ptr)
{
  nomeCliente[auxDigTexto] = 'W';
  atualizaTexto();
  
}
void pushLetE(void *ptr)
{
  nomeCliente[auxDigTexto] = 'E';
  atualizaTexto();
}
void pushLetR(void *ptr)
{
  nomeCliente[auxDigTexto] = 'R';
  atualizaTexto();
}
void pushLetT(void *ptr)
{
  nomeCliente[auxDigTexto] = 'T';
  atualizaTexto();
}
void pushLetY(void *ptr)
{
  nomeCliente[auxDigTexto] = 'Y';
  atualizaTexto();
}
void pushLetU(void *ptr)
{
  nomeCliente[auxDigTexto] = 'U';
  atualizaTexto();
}
void pushLetI(void *ptr)
{
  nomeCliente[auxDigTexto] = 'I';
  atualizaTexto();
}
void pushLetO(void *ptr)
{
  nomeCliente[auxDigTexto] = 'O';
  atualizaTexto();
}
void pushLetP(void *ptr)
{
  nomeCliente[auxDigTexto] = 'P';
  atualizaTexto();
}
    
void pushLetA(void *ptr)
{
  nomeCliente[auxDigTexto] = 'A';
  atualizaTexto();
}
void pushLetS(void *ptr)
{
  nomeCliente[auxDigTexto] = 'S';
  atualizaTexto();
}
void pushLetD(void *ptr)
{
  nomeCliente[auxDigTexto] = 'D';
  atualizaTexto();
}
void pushLetF(void *ptr)
{
  nomeCliente[auxDigTexto] = 'F';
  atualizaTexto();
}
void pushLetG(void *ptr)
{
  nomeCliente[auxDigTexto] = 'G';
  atualizaTexto();
}
void pushLetH(void *ptr)
{
  nomeCliente[auxDigTexto] = 'H';
  atualizaTexto();
}
void pushLetJ(void *ptr)
{
  nomeCliente[auxDigTexto] = 'J';
  atualizaTexto();
}
void pushLetK(void *ptr)
{
  nomeCliente[auxDigTexto] = 'K';
  atualizaTexto();
}
void pushLetL(void *ptr)
{
  nomeCliente[auxDigTexto] = 'L';
  atualizaTexto();
}
    
void pushLetZ(void *ptr)
{
 nomeCliente[auxDigTexto] = 'Z';
  atualizaTexto(); 
}
void pushLetX(void *ptr)
{
  nomeCliente[auxDigTexto] = 'X';
  atualizaTexto();
}
void pushLetC(void *ptr)
{
  nomeCliente[auxDigTexto] = 'C';
  atualizaTexto();
}
void pushLetV(void *ptr)
{
  nomeCliente[auxDigTexto] = 'V';
  atualizaTexto();
}
void pushLetB(void *ptr)
{
  nomeCliente[auxDigTexto] = 'B';
  atualizaTexto();
}
void pushLetN(void *ptr)
{
  nomeCliente[auxDigTexto] = 'N';
  atualizaTexto();
}
void pushLetM(void *ptr)
{
  nomeCliente[auxDigTexto] = 'M';
  atualizaTexto();
}

void pushLetEnter(void *ptr)
{

 limpaPaginas();
 PERSONALIZADO.show();
 if(setCliente1) {
  CLI_01.setText(nomeCliente);
  CARD1_0.setValue(rfid1);
  CARD1_1.setValue(rfid2);
  CARD1_2.setValue(rfid3);
  CARD1_3.setValue(rfid4);
  
 }
 
 if(setCliente2) {
  CLI_02.setText(nomeCliente);
  CARD2_0.setValue(rfid1);
  CARD2_1.setValue(rfid2);
  CARD2_2.setValue(rfid3);
  CARD2_3.setValue(rfid4);
 }
 if(setCliente3) {
  CLI_03.setText(nomeCliente);
  CARD3_0.setValue(rfid1);
  CARD3_1.setValue(rfid2);
  CARD3_2.setValue(rfid3);
  CARD3_3.setValue(rfid4);
 }

 rfid1 = 0;
 rfid2 = 0;
 rfid3 = 0;
 rfid4 = 0;

 setCliente1 = LOW;
 setCliente2 = LOW;
 setCliente3 = LOW;

 auxDigTexto = 0;
 Serial.println("teste");
}
void pushLetEspaco(void *ptr)
{
  nomeCliente[auxDigTexto] = ' ';
  atualizaTexto();
}

/*
void atualizarDados(){
  Wire.requestFrom(slaveAdress, sizeof(dados));

  for(int i = 0; i < Wire.available(); i++){
        dados[i] = Wire.read();
  }


 P_VM1.setValue(atualB1VM);
 P_AZ1.setValue(atualB1AZ);
 P_VD1.setValue(atualB1VD);

 P_VM2.setValue(atualB2VM);
 P_AZ2.setValue(atualB2AZ);
 P_VD2.setValue(atualB2VD);

 P_VM3.setValue(atualB3VM);
 P_AZ3.setValue(atualB3AZ);
 P_VD3.setValue(atualB3VD);

 ACIO_S1.setValue(contS1);
 ACIO_S2.setValue(contS2);
 ACIO_S3.setValue(contS3);
 ACIO_S4.setValue(contS4);
 ACIO_S5.setValue(contS5);

  
}
*/

void setup() {

  Wire.begin();
  nexInit();

    IMG_LOGO.attachPop(pushImgLogo, &IMG_LOGO);
    
    BOT_PADRAO.attachPop(pushBotPadrao, &BOT_PADRAO);
    BOT_PERSONALIZAR.attachPop(pushBotPersonalizar, &BOT_PERSONALIZAR);
    BOT_MANUT_INFO.attachPop(pushBotManutInfo, &BOT_MANUT_INFO);

    CLIENTE_1.attachPop(pushCliente1, &CLIENTE_1);
    CLIENTE_2.attachPop(pushCliente2, &CLIENTE_2);
    CLIENTE_3.attachPop(pushCliente3, &CLIENTE_3);

    INC_VM_1.attachPush(pushIncVm1, &INC_VM_1);
    INC_VM_2.attachPush(pushIncVm2, &INC_VM_2);
    INC_VM_3.attachPush(pushIncVm3, &INC_VM_3);

    INC_AZ_1.attachPush(pushIncAz1, &INC_AZ_1);
    INC_AZ_2.attachPush(pushIncAz2, &INC_AZ_2);
    INC_AZ_3.attachPush(pushIncAz3, &INC_AZ_3);

    INC_VD_1.attachPush(pushIncVd1, &INC_VD_1);
    INC_VD_2.attachPush(pushIncVd2, &INC_VD_2);
    INC_VD_3.attachPush(pushIncVd3, &INC_VD_3);

    DEC_VM_1.attachPush(pushDecVm1, &DEC_VM_1);
    DEC_VM_2.attachPush(pushDecVm2, &DEC_VM_2);
    DEC_VM_3.attachPush(pushDecVm3, &DEC_VM_3);

    DEC_AZ_1.attachPush(pushDecAz1, &DEC_AZ_1);
    DEC_AZ_2.attachPush(pushDecAz2, &DEC_AZ_2);
    DEC_AZ_3.attachPush(pushDecAz3, &DEC_AZ_3);

    DEC_VD_1.attachPush(pushDecVd1, &DEC_VD_1);
    DEC_VD_2.attachPush(pushDecVd2, &DEC_VD_2);
    DEC_VD_3.attachPush(pushDecVd3, &DEC_VD_3);

    BOT_CONFIRMA.attachPop(pushBotConfirma, &BOT_CONFIRMA);
    BOT_RET_PG2.attachPop(pushBotRetPg2, &BOT_RET_PG2);

    BOT_INICIAR.attachPop(pushBotIniciar, &BOT_INICIAR);
    BOT_RET_PG3.attachPop(pushBotRetPg3, &BOT_RET_PG3);

    BOT_ATUADORES.attachPop(pushBotAtuadores, &BOT_ATUADORES);
    BOT_PRODUCAO.attachPop(pushBotProducao, &BOT_PRODUCAO);
    BOT_MANUT.attachPop(pushBotManut, &BOT_MANUT);
    BOT_RET_PG4.attachPop(pushBotRetPg4, &BOT_RET_PG4);

    BOT_ON_S1.attachPop(pushBotOnS1, &BOT_ON_S1);
    BOT_ON_S2.attachPop(pushBotOnS2, &BOT_ON_S2);
    BOT_ON_S3.attachPop(pushBotOnS3, &BOT_ON_S3);
    BOT_ON_S4.attachPop(pushBotOnS4, &BOT_ON_S4);
    BOT_ON_S5.attachPop(pushBotOnS5, &BOT_ON_S5);
    BOT_ON_EST.attachPop(pushBotOnEst, &BOT_ON_EST);

    BOT_OFF_S1.attachPop(pushBotOffS1, &BOT_OFF_S1);
    BOT_OFF_S2.attachPop(pushBotOffS2, &BOT_OFF_S2);
    BOT_OFF_S3.attachPop(pushBotOffS3, &BOT_OFF_S3);
    BOT_OFF_S4.attachPop(pushBotOffS4, &BOT_OFF_S4);
    BOT_OFF_S5.attachPop(pushBotOffS5, &BOT_OFF_S5);
    BOT_OFF_EST.attachPop(pushBotOffEst, &BOT_OFF_EST);

    BOT_PASSO.attachPop(pushBotPasso, &BOT_PASSO);
    BOT_RET_PG5.attachPop(pushBotRetPg5, &BOT_RET_PG5);

    BOT_NOVO.attachPop(pushBotNovo, &BOT_NOVO);
    BOT_MENU_INICIAL.attachPop(pushBotMenuInicial, &BOT_MENU_INICIAL);

    BOT_RET_PG7.attachPop(pushBotRetPg7, &BOT_RET_PG7);

    BOT_RET_PG9.attachPop(pushBotRetPg9, &BOT_RET_PG9);
    
    BOT_RET_PG10.attachPop(pushBotRetPg10, &BOT_RET_PG10);

    BOT_LER_TAG.attachPop(pushBotLerTag, &BOT_LER_TAG);

    LET_Q.attachPush(pushLetQ, &LET_Q);
    LET_W.attachPush(pushLetW, &LET_W);
    LET_E.attachPush(pushLetE, &LET_E);
    LET_R.attachPush(pushLetR, &LET_R);
    LET_T.attachPush(pushLetT, &LET_T);
    LET_Y.attachPush(pushLetY, &LET_Y);
    LET_U.attachPush(pushLetU, &LET_U);
    LET_I.attachPush(pushLetI, &LET_I);
    LET_O.attachPush(pushLetO, &LET_O);
    LET_P.attachPush(pushLetP, &LET_P);
    
    LET_A.attachPush(pushLetA, &LET_A);
    LET_S.attachPush(pushLetS, &LET_S);
    LET_D.attachPush(pushLetD, &LET_D);
    LET_F.attachPush(pushLetF, &LET_F);
    LET_G.attachPush(pushLetG, &LET_G);
    LET_H.attachPush(pushLetH, &LET_H);
    LET_J.attachPush(pushLetJ, &LET_J);
    LET_K.attachPush(pushLetK, &LET_K);
    LET_L.attachPush(pushLetL, &LET_L);
    
    LET_Z.attachPush(pushLetZ, &LET_Z);
    LET_X.attachPush(pushLetX, &LET_X);
    LET_C.attachPush(pushLetC, &LET_C);
    LET_V.attachPush(pushLetV, &LET_V);
    LET_B.attachPush(pushLetB, &LET_B);
    LET_N.attachPush(pushLetN, &LET_N);
    LET_M.attachPush(pushLetM, &LET_M);

    LET_ENTER.attachPop(pushLetEnter, &LET_ENTER);
    LET_ESPACO.attachPush( pushLetEspaco, &LET_ESPACO);

    ultimaAtualizacao = millis();
    verificarCorrente = 0;
}

void transmitirComandos(){
  
  Wire.beginTransmission(10);
  for(int i=0; i<sizeof(comandos); i++){
    Wire.write(comandos[i]);
  }
  Wire.endTransmission(10);
  
}

void limpaComandos(){
  acionaS1 = 0;
  acionaS2 = 0;
  acionaS3 = 0;
  acionaS4 = 0;
  acionaS5 = 0;

  
}

void atualizaProduzindo(){

    uint32_t valor;
     
    valor = atualB1VM;
    P_VM1.setValue(valor);

    valor = atualB1AZ;
    P_AZ1.setValue(valor); 

    valor = atualB1VD;
    P_VD1.setValue(valor); 

    valor = atualB2VM;
    P_VM2.setValue(valor); 

    valor = atualB2AZ;
    P_AZ2.setValue(valor); 

    valor = atualB2VD;
    P_VD2.setValue(valor);  

    valor = atualB3VM;
    P_VM3.setValue(valor);  

    valor = atualB3AZ;
    P_AZ3.setValue(valor);  

    valor = atualB3VD;
    P_VD3.setValue(valor);      
  
}

void atualizaProduzido(){
    uint32_t valor;

    valor = EEPROM.read(mem_prod_vm);
    PRO_VM.setValue(valor);

    valor = EEPROM.read(mem_prod_az);
    PRO_AZ.setValue(valor);

    valor = EEPROM.read(mem_prod_vd);
    PRO_VD.setValue(valor);

    valor = EEPROM.read(mem_retr_vm);
    RET_VM.setValue(valor);

    valor = EEPROM.read(mem_retr_az);
    RET_AZ.setValue(valor);

    valor = EEPROM.read(mem_retr_vd);
    RET_VD.setValue(valor);
    
}

void atualizaUtilizacao(){
    uint32_t valor;

    valor = EEPROM.read(mem_acio_s1);
    ACIO_S1.setValue(valor);
    
    valor = EEPROM.read(mem_acio_s2);
    ACIO_S2.setValue(valor);

    valor = EEPROM.read(mem_acio_s3);
    ACIO_S3.setValue(valor);

    valor = EEPROM.read(mem_acio_s4);
    ACIO_S4.setValue(valor);

    valor = EEPROM.read(mem_acio_s5);
    ACIO_S5.setValue(valor);

    valor = EEPROM.read(mem_acio_passo_s);
    SG_PASSO.setValue(valor);

    valor = EEPROM.read(mem_acio_passo_m);
    MN_PASSO.setValue(valor);

    valor = EEPROM.read(mem_acio_passo_h);
    HR_PASSO.setValue(valor);

    valor = EEPROM.read(mem_acio_esteira_s);
    SG_EST.setValue(valor);

    valor = EEPROM.read(mem_acio_esteira_m);
    MN_EST.setValue(valor);

    valor = EEPROM.read(mem_acio_esteira_h);
    HR_EST.setValue(valor);

}

void salvaDados(){
  
}

void lerDados(){

  
  if(millis() - ultimaAtualizacao >= intervalo){
    Serial.println("lendo!");
    Wire.requestFrom(slaveAdress, sizeof(dados));    
    for (int i=0;0 < Wire.available(); i++) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      dados[i] = c;
    }
/*
    for(int i=0; i<sizeof(dados); i++){
      Serial.println(dados[i]);
    }
*/
  //-------------------------Verifica se haverá incremento dos acionamentos

  if(contS1 != 0 || contS2 != 0 || contS3 != 0 || contS4 != 0 || contS5 != 0) 
    atualizaAcionamentos();

  //-------------------------Verifica se haverá incremento na produção

  if(incVmProd != 0 || incAzProd != 0 || incVdProd != 0 || 
   incVmRet != 0 || incAzRet != 0 || incVdRet != 0 ) 
   atualizaProducao();

  //------------------Verifica se haverá incremento no tempo dos motores

  if(tempoPasso != 0 || tempoEsteira !=0)
    atualizaTempos();

  //-------------------------Atualização dos dados do display

   if(pageProduzindo) atualizaProduzindo();
   if(pageProducao) atualizaProduzido();
   if(pageUtilizacao) atualizaUtilizacao();

   salvaDados();

   if(verificarCorrente == 1){
    transmitirComandos();
    LerCorrente();
   }

   if(pageCadastro){

    ID0.setValue(rfid1);
    ID1.setValue(rfid2);
    ID2.setValue(rfid3);
    ID3.setValue(rfid4);
  
   }

   ultimaAtualizacao = millis();
  }

  if(fimEnviado == 1) finalizaProcesso();

  
}

//----- Ler corrente da esteira ------
//Função: Ler a corrente de entrada do circuito(fonte de alimentação)

void LerCorrente(){

  Serial.println("Lendo Corrente");
  
  double valorMaximo;
  valorMaximo = ampereBLOW;

  valorMaximo = valorMaximo * 0.004;
  valorMaximo = valorMaximo / 0.185;
  valorMaximo = valorMaximo * 0.707;
  
  Serial.println(valorMaximo);
  uint32_t correnteKit = valorMaximo * 1000;

  I_KIT.setValue(correnteKit);
     

}//FIM LER CORRENTE

//-------------------Atualiza tempo dos motores

void atualizaTempos(){
  int valor_s, valor_m, valor_h;
  
  if(tempoPasso != 0){
    int valor_s, valor_m, valor_h;
    valor_s = EEPROM.read(mem_acio_passo_s);
    valor_s = valor_s + tempoPasso;
    for( valor_m = EEPROM.read(mem_acio_passo_m) ; valor_s >= 60 ; valor_s = valor_s - 60){
      valor_m++;
      for( valor_h = EEPROM.read(mem_acio_passo_h) ; valor_m >= 60 ; valor_m = valor_m - 60){
        valor_h++;
      }
    }
    
    EEPROM.write(mem_acio_passo_s, valor_s);
    EEPROM.write(mem_acio_passo_m, valor_m);
    EEPROM.write(mem_acio_passo_h, valor_h);

    tempoPasso = 0;    
  }

  if(tempoEsteira != 0){
    int valor_s, valor_m, valor_h;
    valor_s = EEPROM.read(mem_acio_esteira_s);
    valor_s = valor_s + tempoEsteira;
    for( valor_m = EEPROM.read(mem_acio_esteira_m) ; valor_s >= 60 ; valor_s = valor_s - 60){
      valor_m++;
      for( valor_h = EEPROM.read(mem_acio_esteira_h) ; valor_m >= 60 ; valor_m = valor_m - 60){
        valor_h++;
      }
    }

    Serial.print("Tempo esteira: "); Serial.print(valor_s); 
    Serial.print(" : "); Serial.print(valor_m); Serial.print(" : "); 
    Serial.println(valor_h);
    
    EEPROM.write(mem_acio_esteira_s, valor_s);
    EEPROM.write(mem_acio_esteira_m, valor_m);
    EEPROM.write(mem_acio_esteira_h, valor_h);

    tempoEsteira = 0;  
  }

  temposAtualizados = 1;
  transmitirComandos();
  temposAtualizados = 0;
  
}


//-------------------Atualiza a contagem da produção na memória

void atualizaProducao(){
  int valor;

  Serial.println("Atualiza produção");

  //Atualiza Produção Vermelho
  if(incVmProd!=0){
    valor = EEPROM.read(mem_prod_vm);
    valor = valor + incVmProd;
    EEPROM.write(mem_prod_vm, valor);
    incVmProd = 0;
  }

  //Atualiza Produção Azul
  if(incAzProd!=0){
    valor = EEPROM.read(mem_prod_az);
    valor = valor + incAzProd;
    EEPROM.write(mem_prod_az, valor);
    incAzProd = 0;
  }

  //Atualiza Produção Verde
  if(incVdProd!=0){
    valor = EEPROM.read(mem_prod_vd);
    valor = valor + incVdProd;
    EEPROM.write(mem_prod_vd, valor);
    incVdProd = 0;
  }

  //Atualiza Retrabalho Vermelho
  if(incVmRet!=0){
    valor = EEPROM.read(mem_retr_vm);
    valor = valor + incVmRet;
    EEPROM.write(mem_retr_vm, valor);
    incVmRet = 0;
  }

  //Atualiza Retrabalho Azul
  if(incAzRet!=0){
    valor = EEPROM.read(mem_retr_az);
    valor = valor + incAzRet;
    EEPROM.write(mem_retr_az, valor);
    incAzRet = 0;
  }

  //Atualiza Retrabalho Verde
  if(incVdRet!=0){
    valor = EEPROM.read(mem_retr_vd);
    valor = valor + incVdRet;
    EEPROM.write(mem_retr_vd, valor);
    incVdRet = 0;
  }

  producaoAtualizada = 1;
  transmitirComandos();
  producaoAtualizada = 0;
  

}

//-------------------Atualiza a contagem dos acionamentos na memória

void atualizaAcionamentos(){
  int valor;

  //Atualizar S1
  if(contS1!=0){
    valor = EEPROM.read(mem_acio_s1);
    valor = valor + contS1;
    EEPROM.write(mem_acio_s1, valor);
    contS1 = 0;
  }
  
  //Atualizar S2
  if(contS2!=0){
    valor = EEPROM.read(mem_acio_s2);
    valor = valor + contS2;
    EEPROM.write(mem_acio_s2, valor);
    contS2 = 0;
  }
  
  //Atualizar S3
  if(contS3!=0){
    valor = EEPROM.read(mem_acio_s3);
    valor = valor + contS3;
    EEPROM.write(mem_acio_s3, valor);
    contS3 = 0;
  }


  //Atualizar S4
  if(contS4!=0){
    valor = EEPROM.read(mem_acio_s4);
    valor = valor + contS4;
    EEPROM.write(mem_acio_s4, valor);
    contS4 = 0;
  }


  //Atualizar S5
  if(contS5!=0){
    valor = EEPROM.read(mem_acio_s5);
    valor = valor + contS5;
    EEPROM.write(mem_acio_s5, valor);
    contS5 = 0;
  }

  acionamentosAtualizados = 1;
  transmitirComandos();
  acionamentosAtualizados = 0;

}

//-------------------Realiza os procedimentos para finalização do ultimo ciclo
void finalizaProcesso(){
    limpaPaginas();
    pageFinalizado = 1;
    fimRecebido = 1;

    setB1VM = 0;
    setB1AZ = 0;
    setB1VD = 0;

    setB2VM = 0;
    setB2AZ = 0;
    setB2VD = 0;

    setB3VM = 0;
    setB3AZ = 0;
    setB3VD = 0;
    
    transmitirComandos();
    fimRecebido = 0;
    fimEnviado = 0;
    
    FINALIZADO.show();
}


//-----------------------------Limpa asvariáveis de identificação de página atual
void limpaPaginas(){
     pageAbertura = LOW,
     pageHome = LOW,
     pagePersonalizado = LOW,
     pageSerie = LOW,
     pageManut = LOW,
     pageAcionamentos = LOW,
     pageAguarde = LOW,
     pageProduzindo = LOW,
     pageFinalizado = LOW,
     pageProducao = LOW,
     pageUtilizacao = LOW,
     pageCadastro = LOW,
     pageEmergencia = LOW,
     pagePausa = LOW;

     verificarCorrente = 0;
     transmitirComandos();
     
}

void loop() {

 nexLoop(nex_Listen_List);
 lerDados();
 

}
