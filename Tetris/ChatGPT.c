#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define CLEAR "cls"
#else
#include <unistd.h>
#include <termios.h>
#define CLEAR "clear"
#endif

#define FILAS 20
#define COLUMNAS 10

int tablero[FILAS][COLUMNAS] = {0};

// Piezas (solo 3 para simplificar)
int piezas[3][4][4] = {
    // I
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // O
    {
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

typedef struct {
    int forma[4][4];
    int x, y;
} Pieza;

Pieza actual;

void copiarPieza(int destino[4][4], int origen[4][4]) {
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            destino[i][j] = origen[i][j];
}

void nuevaPieza() {
    int tipo = rand() % 3;
    copiarPieza(actual.forma, piezas[tipo]);
    actual.x = 0;
    actual.y = COLUMNAS / 2 - 2;
}

int colision(int nx, int ny, int forma[4][4]) {
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(forma[i][j]){
                int x = nx + i;
                int y = ny + j;

                if(x >= FILAS || y < 0 || y >= COLUMNAS)
                    return 1;
                if(tablero[x][y])
                    return 1;
            }
        }
    }
    return 0;
}

void fijarPieza() {
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(actual.forma[i][j]){
                tablero[actual.x + i][actual.y + j] = 1;
            }
        }
    }
}

void rotar() {
    int temp[4][4];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[j][3-i] = actual.forma[i][j];

    if(!colision(actual.x, actual.y, temp))
        copiarPieza(actual.forma, temp);
}

void limpiarLineas() {
    for(int i=FILAS-1;i>=0;i--){
        int llena = 1;
        for(int j=0;j<COLUMNAS;j++){
            if(!tablero[i][j]){
                llena = 0;
                break;
            }
        }

        if(llena){
            for(int k=i;k>0;k--)
                for(int j=0;j<COLUMNAS;j++)
                    tablero[k][j] = tablero[k-1][j];

            for(int j=0;j<COLUMNAS;j++)
                tablero[0][j] = 0;

            i++;
        }
    }
}

void dibujar() {
    system(CLEAR);

    for(int i=0;i<FILAS;i++){
        printf("|");
        for(int j=0;j<COLUMNAS;j++){
            int ocupado = tablero[i][j];

            // dibujar pieza actual
            for(int pi=0;pi<4;pi++){
                for(int pj=0;pj<4;pj++){
                    if(actual.forma[pi][pj]){
                        int x = actual.x + pi;
                        int y = actual.y + pj;
                        if(x == i && y == j)
                            ocupado = 1;
                    }
                }
            }

            printf(ocupado ? "#" : " ");
        }
        printf("|\n");
    }

    for(int i=0;i<COLUMNAS+2;i++)
        printf("-");
    printf("\n");
}

int inputDisponible() {
#ifdef _WIN32
    return _kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF){
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
#endif
}

int leerInput() {
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

int main() {
    srand(time(NULL));
    nuevaPieza();

    while(1){
        dibujar();

        // input
        if(inputDisponible()){
            char c = leerInput();

            if(c == 'q') break;

            if(c == 'a' && !colision(actual.x, actual.y - 1, actual.forma))
                actual.y--;

            if(c == 'd' && !colision(actual.x, actual.y + 1, actual.forma))
                actual.y++;

            if(c == 's' && !colision(actual.x + 1, actual.y, actual.forma))
                actual.x++;

            if(c == 'w')
                rotar();
        }

        // caída automática
        if(!colision(actual.x + 1, actual.y, actual.forma)){
            actual.x++;
        } else {
            fijarPieza();
            limpiarLineas();
            nuevaPieza();

            if(colision(actual.x, actual.y, actual.forma)){
                printf("GAME OVER\n");
                break;
            }
        }

#ifdef _WIN32
        Sleep(300);
#else
        usleep(300000);
#endif
    }

    return 0;
}