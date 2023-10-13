#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

int main() {
    initscr(); // Inicia a tela
    noecho();  // Desativa a exibição dos caracteres digitados
    keypad(stdscr, TRUE); // Habilita as teclas especiais (setas)

    int x = 10, y = 10; // Posição inicial do helicóptero

    while (1) {
        clear(); // Limpa a tela

        // Desenha o helicóptero na posição atual
        mvprintw(y, x, "H");

        // Atualiza a tela
        refresh();

        // Captura a entrada do usuário
        int ch = getch();

        // Move o helicóptero com base na entrada do usuário
        switch(ch) {
            case KEY_UP:
                y--;
                break;
            case KEY_DOWN:
                y++;
                break;
            case KEY_LEFT:
                x--;
                break;
            case KEY_RIGHT:
                x++;
                break;
        }

        // Condições de explosão
        if (y <= 0 || y >= LINES - 1) {
            mvprintw(LINES/2, COLS/2 - 5, "BOOM!"); // Exibe "BOOM!" no centro da tela
            refresh();
            getch(); // Aguarda o usuário pressionar uma tecla
            break; // Sai do loop
        }
    }

    endwin(); // Encerra o modo ncurses

    return 0;
}