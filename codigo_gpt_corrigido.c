// Correção de código para jogo de helicóptero em C

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // inclusão da biblioteca para o uso da função usleep
#include <stdbool.h>
#include <time.h>
#include <ncurses.h> // inclusão da biblioteca para o uso da função getch

#define JANELA_LINHAS 20
#define JANELA_COLUNAS 40
#define HELICOPTERO_LINHAS 3
#define HELICOPTERO_COLUNAS 10
#define PASSAGEIRO_LINHAS 2
#define PASSAGEIRO_COLUNAS 4
#define INICIO_HELICOPTERO_X 2
#define INICIO_HELICOPTERO_Y 2
#define INICIO_PASSAGEIRO1_X 2
#define INICIO_PASSAGEIRO1_Y 7
#define RESGATE_X 37
#define RESGATE_Y 7

char janela[JANELA_LINHAS][JANELA_COLUNAS];
pthread_mutex_t mutex_janela;
int passageiros_resgatados = 0;

void limpar_janela() {
    int i, j;
    for (i=0; i<JANELA_LINHAS; i++) {
        for (j=0; j<JANELA_COLUNAS; j++) {
            janela[i][j] = ' ';
        }
    }
}

void imprimir_janela() {
    int i, j;
    for (i=0; i<JANELA_COLUNAS+2; i++) {
        printf("#");
    }
    printf("\n");

    for (i=0; i<JANELA_LINHAS; i++) {
        printf("#");
        for (j=0; j<JANELA_COLUNAS; j++) {
            printf("%c", janela[i][j]);
        }
        printf("#\n");
    }

    for (i=0; i<JANELA_COLUNAS+2; i++) {
        printf("#");
    }
    printf("\n");
}

void mover_helicoptero(int x, int y) {
    int i, j;
    for (i=0; i<HELICOPTERO_LINHAS; i++) {
        for (j=0; j<HELICOPTERO_COLUNAS; j++) {
            janela[i+y][j+x] = ' ';
        }
    }

    janela[y][x+1] = '-';
    janela[y][x+2] = '-';
    janela[y+1][x] = '/';
    janela[y+1][x+1] = '+';
    janela[y+1][x+2] = '+';
    janela[y+1][x+3] = '\\';
    janela[y+2][x+1] = '-';
    janela[y+2][x+2] = '-';
}

void mover_passageiro(int x, int y) {
    int i, j;
    for (i=0; i<PASSAGEIRO_LINHAS; i++) {
        for (j=0; j<PASSAGEIRO_COLUNAS; j++) {
            janela[i+y][j+x] = ' ';
        }
    }

    janela[y][x+1] = '|';
    janela[y][x+2] = '|';
    janela[y+1][x+1] = 'o';
    janela[y+1][x+2] = 'o';
    janela[y+2][x] = '/';
    janela[y+2][x+1] = '\\';
    janela[y+2][x+2] = '/';
    janela[y+2][x+3] = '\\';
}

void *atualizar_janela(void *arg) {
    while (true) {
        pthread_mutex_lock(&mutex_janela);
        system("clear"); // limpando a tela com clear em vez de system("cls")
        imprimir_janela();
        pthread_mutex_unlock(&mutex_janela);

        if (passageiros_resgatados == 1) {
            printf("\nPARABENS! Passageiro resgatado com sucesso!\n");
            endwin(); // finalizando a ncurses
            exit(0);
        }
        else if (passageiros_resgatados == 2) {
            printf("\nVOCE VENCEU O JOGO! Parabens!\n");
            endwin(); // finalizando a ncurses
            exit(0);
        }

        usleep(30000);
    }
}

bool verificar_passageiro(int x_helicoptero, int y_helicoptero, int x_passageiro, int y_passageiro) {
    int i, j;
    for (i=0; i<HELICOPTERO_LINHAS; i++) {
        for (j=0; j<HELICOPTERO_COLUNAS; j++) {
            if (janela[y_helicoptero+i][x_helicoptero+j] != ' ' && janela[y_helicoptero+i][x_helicoptero+j] != '-') {
                if (i+y_helicoptero == y_passageiro && j+x_helicoptero == x_passageiro) {
                    return true;
                }
            }
        }
    }

    return false;
}

void *movimentar_helicoptero(void *arg) {
    int x_helicoptero = INICIO_HELICOPTERO_X;
    int y_helicoptero = INICIO_HELICOPTERO_Y;
    int mesmo_lugar = 0;
    int tecla;
    srand(time(NULL));

    while (true) {
        tecla = getch();

        if (tecla == 27) {
            printf("\nVOCE ENCERROU O JOGO!\n");
            endwin();
            exit(0);
        }
        else if (tecla == KEY_UP)
            if (y_helicoptero > 2 && mesmo_lugar == 0) {
            mover_helicoptero(x_helicoptero, y_helicoptero-1);
            y_helicoptero--;
            mesmo_lugar++;    
        }
        else if (verificar_passageiro(x_helicoptero, y_helicoptero-1, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y ) == true) {
            mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y );
            passageiros_resgatados++;           
        } else if (tecla == KEY_DOWN){  
            if (y_helicoptero < (JANELA_LINHAS - HELICOPTERO_LINHAS)){
                mover_helicoptero(x_helicoptero, y_helicoptero+1);
                y_helicoptero++;
                mesmo_lugar++;                    
            } else if (verificar_passageiro(x_helicoptero, y_helicoptero+1, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y )) {
                mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y );
                passageiros_resgatados++;               
            }
        } else if (tecla == KEY_LEFT) {
            if (x_helicoptero > 1 && mesmo_lugar == 0){
                mover_helicoptero(x_helicoptero-1, y_helicoptero );
                x_helicoptero--;
                mesmo_lugar++;
            } else if (verificar_passageiro(x_helicoptero-1, y_helicoptero, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y)) {
                mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y );
                passageiros_resgatados++;               
            }
        } else if (tecla == KEY_RIGHT) {
            if (x_helicoptero < JANELA_COLUNAS - HELICOPTERO_COLUNAS && mesmo_lugar == 0){
                mover_helicoptero(x_helicoptero+1, y_helicoptero);
                x_helicoptero++;
                mesmo_lugar++;
            } else if(verificar_passageiro(x_helicoptero+1, y_helicoptero, INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y ) == true) {
                mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y );
                passageiros_resgatados++;               
            }
        } else if (tecla == ' '){ 
            if (x_helicoptero == RESGATE_X && y_helicoptero == RESGATE_Y && mesmo_lugar == 0){
                passageiros_resgatados++;
            }    
            else{
                mesmo_lugar++; 
            }
        }

        if (mesmo_lugar > 0) {
            move(INICIO_HELICOPTERO_Y, INICIO_HELICOPTERO_X+8);
            printw("TURBULENCIA!"); // impressão da mensagem usando funções ncurses
            refresh();
            usleep(700000);
        }
        else{
            move(INICIO_HELICOPTERO_Y, INICIO_HELICOPTERO_X+8);
            printw("             "); // limpeza da mensagem anterior usando funções ncurses
            refresh();
        }
    }
}

int main() {
    limpar_janela();

    mover_helicoptero(INICIO_HELICOPTERO_X, INICIO_HELICOPTERO_Y);
    mover_passageiro(INICIO_PASSAGEIRO1_X, INICIO_PASSAGEIRO1_Y );
    janela[RESGATE_Y][RESGATE_X] = '*';

    initscr(); // inicialização da ncurses para o jogo
    noecho(); // para que não seja exibido o caractere digitado, como "passo"
    curs_set(FALSE); // para deixar o cursor (seta) fora da tela
    keypad(stdscr, TRUE); // habilitar as capturas das setas pelo ncurses

    pthread_mutex_init(&mutex_janela, NULL);

    pthread_t thread_janela, thread_helicoptero;
    pthread_create(&thread_janela, NULL, atualizar_janela, NULL);
    pthread_create(&thread_helicoptero, NULL, movimentar_helicoptero, NULL);

    pthread_join(thread_janela, NULL);
    pthread_join(thread_helicoptero, NULL);

    pthread_mutex_destroy(&mutex_janela);

    return 0;
}// Relizada uma correção na parte das função cursors e runwind. Adicionada também e inclusão da biblioteca ncurses e alteradas as teclas para keycode correspondente a elas. Foram removidos os comandos "system" utilizados no clean de tela pelo "clear" e a biblioteca utilizado foi inclusa já no código. 