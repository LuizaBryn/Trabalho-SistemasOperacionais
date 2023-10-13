// Incluindo as bibliotecas necessárias para o jogo
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>

// Definindo o tamanho da janela do jogo
#define JANELA_LINHAS 20
#define JANELA_COLUNAS 40

// Definindo o tamanho do helicóptero
#define HELICOPTERO_LINHAS 3
#define HELICOPTERO_COLUNAS 10

// Definindo o tamanho do passageiro
#define PASSAGEIRO_LINHAS 2
#define PASSAGEIRO_COLUNAS 4

// Definindo a posição inicial do helicóptero
#define INICIO_HELICOPTERO_X 2
#define INICIO_HELICOPTERO_Y 2

// Definindo a posição inicial do primeiro passageiro
#define INICIO_PASSAGEIRO1_X 2
#define INICIO_PASSAGEIRO1_Y 7

// Definindo a posição do objetivo do jogo
#define RESGATE_X 37
#define RESGATE_Y 7

// Criando variáveis globais
char janela[JANELA_LINHAS][JANELA_COLUNAS];  // Janela do jogo
pthread_mutex_t mutex_janela;  // Mutex para sincronizar o acesso à janela
int passageiros_resgatados = 0;  // Contador de passageiros resgatados

// Função para preencher a janela com espaços
void limpar_janela()
{
    int i, j;
    for(i=0; i<JANELA_LINHAS; i++){
        for(j=0; j<JANELA_COLUNAS; j++){
            janela[i][j] = ' ';
        }
    }
}

// Função para imprimir a janela na tela
void imprimir_janela()
{
    int i, j;
    for(i=0; i<JANELA_COLUNAS+2; i++){
        printf("#");
    }
    printf("\n");

    for(i=0; i<JANELA_LINHAS; i++){
        printf("#");
        for(j=0; j<JANELA_COLUNAS; j++){
            printf("%c", janela[i][j]);
        }
        printf("#\n");
    }

    for(i=0; i<JANELA_COLUNAS+2; i++){
        printf("#");
    }
    printf("\n");
}

// Função para mover o helicóptero na janela
void mover_helicoptero(int x, int y)
{
    int i, j;
    for(i=0; i<HELICOPTERO_LINHAS; i++){
        for(j=0; j<HELICOPTERO_COLUNAS; j++){
            janela[i+y][j+x] = ' ';
        }
    }

    // Desenhando o helicóptero na nova posição
    janela[y][x+1] = '-';
    janela[y][x+2] = '-';
    janela[y+1][x] = '/';
    janela[y+1][x+1] = '+';
    janela[y+1][x+2] = '+';
    janela[y+1][x+3] = '\\';
    janela[y+2][x+1] = '-';
    janela[y+2][x+2] = '-';
}

// Função para mover o passageiro na janela
void mover_passageiro(int x, int y)
{
    int i, j;
    for(i=0; i<PASSAGEIRO_LINHAS; i++){
        for(j=0; j<PASSAGEIRO_COLUNAS; j++){
            janela[i+y][j+x] = ' ';
        }
    }

    // Desenhando o passageiro na nova posição
    janela[y][x+1] = '|';
    janela[y][x+2] = '|';
    janela[y+1][x+1] = 'o';
    janela[y+1][x+2] = 'o';
    janela[y+2][x] = '/';
    janela[y+2][x+1] = '\\';
    janela[y+2][x+2] = '/';
    janela[y+2][x+3] = '\\';
}

// Função thread para atualizar a janela do jogo
void *atualizar_janela(void *arg)
{
    while(true){
        pthread_mutex_lock(&mutex_janela);
        system("cls");  // limpando a tela antes de imprimir a nova janela

        imprimir_janela();
        pthread_mutex_unlock(&mutex_janela);

        if(passageiros_resgatados == 1){
            printf("\nPARABENS! Passageiro resgatado com sucesso!\n");
            exit(0);  // encerrando o jogo
        }
        else if(passageiros_resgatados == 2){
            printf("\nVOCE VENCEU O JOGO! Parabens!\n");
            exit(0);  // encerrando o jogo
        }

        usleep(30000);  // delay para não atualizar a janela muito rapidamente
    }
}

// Função para verificar se o helicóptero está sobre um passageiro
bool verificar_passageiro(int x_helicoptero, int y_helicoptero, int x_passageiro, int y_passageiro)
{
    int i, j;
    for(i=0; i<HELICOPTERO_LINHAS; i++){
        for(j=0; j<HELICOPTERO_COLUNAS; j++){
            if(janela[y_helicoptero+i][x_helicoptero+j] != ' ' && janela[y_helicoptero+i][x_helicoptero+j] != '-'){
                if(i+y_helicoptero == y_passageiro && j+x_helicoptero == x_passageiro){  // verificando se o helicóptero está sobre o passageiro
                    return true;
                }
            }
        }
    }

    return false;
}

// Função thread para controlar o movimento do helicóptero
void *movimentar_helicoptero(void *arg)
{
    int x_helicoptero = INICIO_HELICOPTERO_X;  // posição atual do helicóptero na janela
    int y_helicoptero = INICIO_HELICOPTERO_Y;
    int mesmo_lugar = 0;  // contador para garantir que o helicóptero não se mova dobrado
    int tecla;
    srand(time(NULL));  // inicializando o gerador de números aleatórios

    while(true){
        tecla = getch();  // recebendo uma tecla digitada pelo usuário

        if(tecla == 27){  // ESC
            printf("\nVOCE ENCERROU O JOGO!\n");
            exit(0);  // encerrando o jogo
        }
        else if(tecla == 224){  // teclas especiais, como setas do teclado, que retornam dois valores
            tecla = getch();
            switch(tecla){
                case 72:  // seta para cima
                    if(y_helicoptero > 2 && mesmo_lugar == 0){
                        mover_helicoptero(x_helicoptero, y_helicoptero-1);
                        y_helicoptero--;
                        mesmo_lugar++;
                    }
                    else if(verificar_passageiro(x_helicoptero, y_helicoptero-1, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y)){  // verificando se o helicóptero está sobre o passageiro
                        mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y);
                        passageiros_resgatados++;
                    }
                    break;
                case 80:  // seta para baixo
                    if(y_helicoptero < JANELA_LINHAS-HELICOPTERO_LINHAS && mesmo_lugar == 0){
                        mover_helicoptero(x_helicoptero, y_helicoptero+1);
                        y_helicoptero++;
                        mesmo_lugar++;
                    }
                    else if(verificar_passageiro(x_helicoptero, y_helicoptero+1, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y)){  // verificando se o helicóptero está sobre o passageiro
                        mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y);
                        passageiros_resgatados++;
                    }
                    break;
                case 75:  // seta para a esquerda
                    if(x_helicoptero > 1 && mesmo_lugar == 0){
                        mover_helicoptero(x_helicoptero-1, y_helicoptero);
                        x_helicoptero--;
                        mesmo_lugar++;
                    }
                    else if(verificar_passageiro(x_helicoptero-1, y_helicoptero, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y)){  // verificando se o helicóptero está sobre o passageiro
                        mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y);
                        passageiros_resgatados++;
                    }
                    break;
                case 77:  // seta para a direita
                    if(x_helicoptero < JANELA_COLUNAS-HELICOPTERO_COLUNAS && mesmo_lugar == 0){
                        mover_helicoptero(x_helicoptero+1, y_helicoptero);
                        x_helicoptero++;
                        mesmo_lugar++;
                    }
                    else if(verificar_passageiro(x_helicoptero+1, y_helicoptero, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y)){  // verificando se o helicóptero está sobre o passageiro
                        mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y);
                        passageiros_resgatados++;
                    }
                    break;
            }
        }
        else if(tecla == ' '){
            if(x_helicoptero == RESGATE_X && y_helicoptero == RESGATE_Y && mesmo_lugar == 0){  // verificando se o helicóptero chegou ao objetivo do jogo
                passageiros_resgatados++;
            }
            mesmo_lugar++;
        }
        else{
            mesmo_lugar = 0;  // o helicóptero só pode se mover de novo se outra tecla foi pressionada
        }

        if(mesmo_lugar > 0){  // se o helicóptero não se mover, produz uma turbulência
            move(INICIO_HELICOPTERO_Y, INICIO_HELICOPTERO_X+8);
            printf("TURBULENCIA!");
            usleep(700000);
        }
        else{
            move(INICIO_HELICOPTERO_Y, INICIO_HELICOPTERO_X+8);
            printf("            ");  // limpando a tela da mensagem anterior
        }
    }
}

// Função principal
int main()
{
    limpar_janela();

    // Desenhando o helicóptero na janela
    mover_helicoptero(INICIO_HELICOPTERO_X, INICIO_HELICOPTERO_Y);

    // Desenhando o primeiro passageiro na janela
    mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y);

    // Desenhando o objetivo do jogo na janela
    janela[RESGATE_Y][RESGATE_X] = '*';

    // Inicializando o mutex para sincronizar o acesso à janela
    pthread_mutex_init(&mutex_janela, NULL);

    // Criando as threads para atualizar a janela e movimentar o helicóptero
    pthread_t thread_janela, thread_helicoptero;

    pthread_create(&thread_janela, NULL, atualizar_janela, NULL);
    pthread_create(&thread_helicoptero, NULL, movimentar_helicoptero, NULL);

    pthread_join(thread_janela, NULL);
    pthread_join(thread_helicoptero, NULL);

    pthread_mutex_destroy(&mutex_janela);

    return 0;
}