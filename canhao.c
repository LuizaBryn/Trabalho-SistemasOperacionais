#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// getArrowKey()
#include <termios.h>
#include <pthread.h>

int x, missao = 0;
int origemHelicoptero = 105, oldOrigemHelicoptero = 106;
int temResgatado = 0;
int origemCanhao0 = 53, origemCanhao1 = 93;
int contaRefem = 10;
int contaResgatado = 0;
int posicaoHelice = 0;
int posicaRoda[2] = {0, 0};
int canhaoMovendo[2] = {0, 0};
int ponte_ocupada = 0; // 0 = livre, 1 = ocupada
int canhao_carregando = -1; // -1 = nenhum, 0 = Canhão 0, 1 = Canhão 1

void *printScreenThread(void *arg);
void *getArrowKeyThread(void *arg);
void explode_bomba(int x, int y);
void helicopteroE2D(int x, int y);
void helicopteroD2E(int x, int y);
void apaga_helicopteroE2D(int x, int y);

pthread_mutex_t ponte_mutex = PTHREAD_MUTEX_INITIALIZER;


// Function gotoxy
void gotoxy(int x, int y)
{
    printf("\033[%d;%df", y, x);
    fflush(stdout);
}

void bomba(int x, int y, int posicao)
{
    for (; y > 0; y--)
    {
        gotoxy(x, y);
        printf("o\n");
        usleep(20000);
        gotoxy(x, y);
        printf(" \n");
        switch (posicao)
        {
        case -1:
            x--;
            break;
        case 0:
            x;
            break;
        case 1:
            x++;
        }
    }
}

void inicializaCanhao(int x, int y, int canhao)
{
    int posicao;
    char roda = 'O';
    time_t t;
    if (canhaoMovendo[canhao])
    {
        switch (posicaRoda[canhao])
        {
        case 0:
            roda = '|';
            break;
        case 1:
            roda = '/';
            break;
        case 2:
            roda = '-';
            break;
        case 3:
            roda = '\\';
            break;
        }
        posicaRoda[canhao] = (posicaRoda[canhao] < 3 ? posicaRoda[canhao] + 1 : 0);
    }
    gotoxy(x, y + 1);
    printf(" ___+--+___");
    gotoxy(x, y + 2);
    printf("/          \\");
    gotoxy(x, y + 3);
    printf("\\          /");
    gotoxy(x, y + 4);
    printf(" -%c-%c--%c-%c-", roda, roda, roda, roda);
    gotoxy(x, y);
    printf("     ||");
}

void canhaoAtira(int x, int y)
{
    int posicao;
    time_t t;
    gotoxy(x, y + 1);
    printf(" ___+--+___");
    gotoxy(x, y + 2);
    printf("/          \\");
    gotoxy(x, y + 3);
    printf("\\          /");
    gotoxy(x, y + 4);
    printf(" -O-O--O-O-");
    gotoxy(x, y);
    srand((unsigned)time(&t));
    for (int foguete = 3; foguete > 0; foguete--)
    {
        posicao = (rand() % 3) - 1;
        gotoxy(0, 28);
        printf("%6d  %6d\n", foguete, posicao);
        gotoxy(x, y);
        switch (posicao)
        {
        case -1:
            printf("     \\\n");
            break;
        case 0:
            printf("     ||");
            break;
        case 1:
            printf("     //");
        }
        bomba(x + 5, y - 1, posicao);
    }
}


void apagaCanhao(int x, int y)
{
    int posicao;
    time_t t;
    gotoxy(x, y + 1);
    printf("           ");
    gotoxy(x, y + 2);
    printf("            ");
    gotoxy(x, y + 3);
    printf("            ");
    gotoxy(x, y + 4);
    printf("           ");
    gotoxy(x, y);
    printf("       ");
}

void moveCanhaoD2E(int origem, int Destino, int canhao)
{
    for (int x = origem; x > Destino;)
    {
        usleep(50000);
        apagaCanhao(x, 20);
        inicializaCanhao(--x, 20, canhao);
    }
}

void moveCanhaoE2D(int origem, int Destino, int canhao)
{
    for (int x = origem; x <= Destino;)
    {
        usleep(50000);
        apagaCanhao(x, 20);
        inicializaCanhao(++x, 20, canhao);
    }
}

void plataformaE(void)
{
    int linha;
    gotoxy(0, 7);
    printf("----------+\n");
    gotoxy(0, 8);
    printf("|         /\n");
    for (linha = 9; linha < 25; linha++)
    {
        gotoxy(0, linha);
        printf("|        |\n");
    }
}

void desenhaRefens(void)
{
    gotoxy(0, 6);
    printf("          ");
    for (int i = 0; i < contaRefem; i++)
    {
        gotoxy(0 + i, 6);
        printf("@");
    }
}

void desenhaResgatados(void)
{
    for (int i = 0; i < contaResgatado; i++)
    {
        gotoxy(109 + i, 6);
        printf("@");
    }
}

void plataformaD(void)
{
    int linha;
    gotoxy(108, 7);
    printf("+-----------\n");
    gotoxy(109, 8);
    printf("\\         |\n");
    for (linha = 9; linha < 25; linha++)
    {
        gotoxy(109, linha);
        printf(" |        | \n");
    }
}

void ponte(void)
{
    gotoxy(30, 25);
    printf("+-----------------------+\n");
    gotoxy(30, 26);
    printf("|         ---           |\n");
    gotoxy(30, 27);
    printf("|        /   \\          |\n");
    gotoxy(30, 28);
    printf("|       /     \\         |\n");
    gotoxy(30, 29);
    printf("|      /       \\        |\n");
}

void deposito(void)
{
    gotoxy(0, 20);
    printf("|\\|\\|\\|\\|\\|\\|\\|\\|\\|\\|\\");
    gotoxy(0, 21);
    printf("|                     |");
    gotoxy(0, 22);
    printf("|       +----+        |");
    gotoxy(0, 23);
    printf("|       |    |        |");
    gotoxy(0, 24);
    printf("|       |    |        |");
}


void explode_bomba(int x, int y)
{
    gotoxy(x, y);
    printf("*");
    usleep(50000);
    printf(" ");
    gotoxy(x, y - 1);
    printf("O");
    gotoxy(x - 1, y);
    printf("O O");
    gotoxy(x, y + 1);
    printf("O");
    usleep(50000);
    gotoxy(x, y - 1);
    printf(" ");
    gotoxy(x - 1, y);
    printf("   ");
    gotoxy(x, y + 1);
    printf(" ");

    gotoxy(x, y - 2);
    printf("o");
    gotoxy(x - 2, y);
    printf("o   o");
    gotoxy(x, y + 2);
    printf("o");
    usleep(50000);
    gotoxy(x, y - 2);
    printf(" ");
    gotoxy(x - 2, y);
    printf("     ");
    gotoxy(x, y + 2);
    printf(" ");
}

void helicopteroE2D(int x, int y)
{
    gotoxy(x + 3, y);
    switch (posicaoHelice)
    {
    case 0:
        printf("______+______\n");
        break;
    case 1:
        printf(" _____+_____ \n");
        break;
    case 2:
        printf("  ____+____  \n");
        break;
    case 3:
        printf("   ___+___   \n");
        break;
    case 4:
        printf("    __+__    \n");
        break;
    case 5:
        printf("     _+_    \n");
        break;
    case 6:
        printf("    __+__    \n");
        break;
    case 7:
        printf("   ___+___   \n");
        break;
    case 8:
        printf("  ____+____  \n");
        break;
    case 9:
        printf(" _____+_____ \n");
        break;
    case 10:
        printf("______+______\n");
        break;
    }
    posicaoHelice = (posicaoHelice < 10 ? posicaoHelice + 1 : 1);
    gotoxy(x + 7, y + 1);
    printf("__|__\n");
    gotoxy(x, y + 2);
    printf("/\\====/     \\\n");
    gotoxy(x, y + 3);
    printf("      \\      )\n");
    gotoxy(x, y + 4);
    if (!temResgatado)
        printf("       _+__+_\n");
    else
        printf("       _+_@+_\n");
}

void helicopteroD2E(int x, int y)
{
    gotoxy(x - 2, y);
    switch (posicaoHelice)
    {
    case 0:
        printf("______+______\n");
        break;
    case 1:
        printf(" _____+_____ \n");
        break;
    case 2:
        printf("  ____+____  \n");
        break;
    case 3:
        printf("   ___+___   \n");
        break;
    case 4:
        printf("    __+__    \n");
        break;
    case 5:
        printf("     _+_     \n");
        break;
    case 6:
        printf("    __+__    \n");
        break;
    case 7:
        printf("   ___+___   \n");
        break;
    case 8:
        printf("  ____+____  \n");
        break;
    case 9:
        printf(" _____+_____ \n");
        break;
    case 10:
        printf("______+______\n");
        break;
    }
    posicaoHelice = (posicaoHelice < 10 ? posicaoHelice + 1 : 1);
    gotoxy(x + 2, y + 1);
    printf("__|__\n");
    gotoxy(x + 1, y + 2);
    printf("/     \\====/\\\n");
    gotoxy(x, y + 3);
    printf("(      /\n");
    gotoxy(x + 2, y + 4);
    if (!temResgatado)
        printf("_+__+_       \n");
    else
        printf("_+@_+_       \n");
}

void apaga_helicopteroE2D(int x, int y)
{
    gotoxy(x, y);
    printf("               \n");
    gotoxy(x, y + 1);
    printf("               \n");
    gotoxy(x, y + 2);
    printf("               \n");
    gotoxy(x, y + 3);
    printf("               \n");
    gotoxy(x, y + 4);
    printf("               \n");
}

void canhaoCarrega(int canhao)
{   
    int origemCanhao;

    if (canhao == 0){
        origemCanhao = origemCanhao0;
    } else {
        origemCanhao = origemCanhao1;
    }

    canhaoMovendo[canhao] = 1; // Canhao em movimento para a ponte
    moveCanhaoD2E(origemCanhao, 40, canhao); // Canhao vai recarregar

    pthread_mutex_lock(&mutex_ponte);

    while (ponte_ocupada || canhao_carregando != -1) {
        pthread_mutex_unlock(&mutex_ponte);
        usleep(100000); // Aguarda um curto período e tenta novamente
        pthread_mutex_lock(&mutex_ponte);
    }

    ponte_ocupada = 1;
    canhao_carregando = canhao;

    pthread_mutex_unlock(&mutex_ponte);

    // Simula o tempo de recarga da bateria
    usleep(2000000); // 2 segundos

    pthread_mutex_lock(&mutex_ponte);
  
     // Atravessa a ponte
    moveCanhaoD2E(40, 23, canhao); // Canhao vai recarregar

    //Carrega
    canhaoMovendo[canhao] = 0; // Canhao parado
    usleep(10000);

    //Volta a ponte
    canhaoMovendo[canhao] = 1;
    moveCanhaoE2D(23, origemCanhao, canhao);
    ponte_ocupada = 0;
    canhao_carregando = -1;

    pthread_mutex_unlock(&mutex_ponte);

    return NULL;
}


void *printScreenThread(void *arg)
{
    while (1)
    {
        system("clear");
        gotoxy(0, 25);
        for (int coluna = 0; coluna < 120; coluna++)
        {
            printf("^");
        }
        ponte();
        plataformaE();
        desenhaRefens();
        deposito();
        plataformaD();
        if (origemHelicoptero - oldOrigemHelicoptero < 0)
            helicopteroD2E(origemHelicoptero, 0);
        else
            helicopteroE2D(origemHelicoptero, 0);

        usleep(100000);
        
    }

    pthread_exit(NULL);
}

void *canhao0_thread(void *arg) {
     inicializaCanhao(origemCanhao0, 20, 0);
     while (1) {
         canhaoAtira(origemCanhao0, 20); // Canhao 0 Dispara
         canhaoCarrega(0); // Canhao 0 vai carregar
    }
}

void *canhao1_thread(void *arg) {
   inicializaCanhao(origemCanhao1, 20, 1);
   while (1) {
        canhaoAtira(origemCanhao1, 20); // Canhao 1 Dispara
        canhaoCarrega(1); // Canhao 1 vai carregar
    }
}
      

void *getArrowKeyThread(void *arg)
{
    struct termios oldtc;
    struct termios newtc;
    int key;

    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);

    while (1)
    {
        key = getchar(); // Read the first character of the escape sequence

        if (key == 27)
        {                    // Check if it's an escape character (ASCII 27)
            key = getchar(); // Read the second character
            if (key == 91)
            {                    // Check if it's the bracket character (ASCII 91)
                key = getchar(); // Read the third character, which represents the arrow key
            }
        }

        switch (key)
        {
        // up arrow
        case 65:
            break;
        // down arrow
        case 66:
            break;
        // right arrow
        case 67:
            oldOrigemHelicoptero = origemHelicoptero;
            origemHelicoptero += 1;
            break;
        // left arrow
        case 68:
            oldOrigemHelicoptero = origemHelicoptero;
            origemHelicoptero -= 1;
            break;
        default:
            break;
        }

        // Você pode fazer o que quiser com a tecla lida aqui, por exemplo, imprimir ou processar.
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    pthread_exit(NULL);
}




int main()
{
    pthread_t screenThread;
    if (pthread_create(&screenThread, NULL, printScreenThread, NULL) != 0)
    {
        perror("Erro ao criar a thread da tela");
        exit(1);
    }
    pthread_t arrowKeyThread;
    if (pthread_create(&arrowKeyThread, NULL, getArrowKeyThread, NULL) != 0)
    {
        perror("Erro ao criar a thread de input");
        exit(1);
    }
        pthread_t canhao0Thread, canhao1Thread;

    // Cria as threads para os canhões
    if (pthread_create(&canhao0Thread, NULL, canhao0_thread, NULL) != 0) {
        perror("Erro ao criar a thread do Canhão 0");
        exit(1);
    }

    if (pthread_create(&canhao1Thread, NULL, canhao1_thread, NULL) != 0) {
        perror("Erro ao criar a thread do Canhão 1");
        exit(1);
    }

    // O restante do código principal pode continuar a executar enquanto a thread estiver em execução.

    // Aguarde a conclusão da thread (opcional).
    pthread_join(screenThread, NULL);
    pthread_join(canhao0Thread, NULL);
    pthread_join(canhao1Thread, NULL);
    pthread_join(arrowKeyThread, NULL);

    return 0;
}