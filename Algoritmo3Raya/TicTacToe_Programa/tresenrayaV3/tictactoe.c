//Programa tres en raya
// Autor: Pelayo García Farrás


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Definición de constantes simbólicas
#define CIERTO 1
#define FALSO 0

//Estructura de datos para almacenar la fila-columna de una
//casilla del tablero que puede ser objeto de un movimiento
struct Movimiento
{
    int fila;
    int columna;
};

//Definición de tipo de datos Movimiento a partir de la estructura
typedef struct Movimiento Movimiento;


//Prototipos de funciones
//Inicializa el tablero poniendo todas las casillas a libre (0)
void inicializarTablero(unsigned char tablero[3][3]); 
//Imprime el estado del tablero
void imprimirTablero(unsigned char tablero[3][3]); 
//Funcion a llamar desde el main() que da comienzo al juego
void juegoTresenRaya(unsigned char tablero[3][3]); 
//Comprueba si hay un ganador y en ese caso devuelve el jugador ganador, si no hay ganador devuelve FALSO (0)
char comprobarGanador(unsigned char tablero[3][3]); 
//Comprueba si hay un empate y en ese caso devuelve CIERTO (1), si no hay empate devuelve FALSO (0)
char comprobarEmpate(unsigned char tablero[3][3]); //comprueba si hay un empate
//Función que elige cuál es el jugador humano (jugador 1 o jugador 2)
unsigned char elegirJugadorHumano(); //elige el jugador humano
//Función que elige aleatoriamente el jugador que empieza la partida (1 o 2)
unsigned char elegirTurnoAleatorio(); 
//Función que permite realizar el movimiento del jugador humano
void realizarMovimientoHumano(unsigned char tablero[3][3], char jugador); 
//Funcion que intenta realizar un movimiento ganador para el jugador (ordenador), si no puede, realiza un movimiento defensivo, si no puede, realiza un movimiento aleatorio
void realizarMovimientoOrdenadorEstrategico(unsigned char tablero[3][3], char jugador);
//Función que imprime el ganador de la partida
void imprimirGanador(char jugador); 
//Función que imprime un mensaje si hay empate
void imprimirEmpate(); 
//Función que cambia el turno del jugador
char cambiarTurno(char turnoactual); 
//Funcioón que imprime a qué jugador le toca jugar la siguiente jugada
void imprimirTurno(char turno);
//Funcion que devuelve un array con la lista de casillas vacias del tablero del tres en raya o NULL si no hay ninguna
Movimiento * casillasVacias(unsigned char tablero[3][3], int * numcasillasvacias);
//Funcion para imprimir la lista de casillas vacías
void imprimirCasillasVacias(Movimiento * casillasvacias, int numcasillasvacias);
//Funcion que devuelve un Movimiento en una fila si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnFilaConCasillaVacia(unsigned char tablero[3][3], char jugador);
//Funcion que devuelve un Movimiento en una columna si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnColumnaConCasillaVacia(unsigned char tablero[3][3], char jugador);
//Funcion que devuelve un Moviminto en una diagonal si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnDiagonalConCasillaVacia(unsigned char tablero[3][3], char jugador);
// Funcion para buscar una estrategia ganadora. Devuelve el movimiento ganador si lo hay, NULL en caso contrario
Movimiento * buscarEstrategiaGanadora(unsigned char tablero[3][3], char jugador);
// Funcion para buscar una estrategia defensiva. Devuelve un movimiento defensico si lo hay, NULL en caso contrario
Movimiento * buscarEstrategiaDefensiva(unsigned char tablero[3][3], char jugador);
// Función que simula, mediante un mensaje, la acción de mover el brazo robot a posición inicial
void moverBrazoRobotPosicionInicial();
// Función que simula la visión artificial que permite reconocer el tablero. 
// Devuelve un puntero a una matriz de 3x3 con los valores de las casillas del tablero
unsigned char * reconocerTablero(char jugadorhumano, char jugadorrobot);
// Función que actualiza el tablero con los valores "reconocidos" por la visión artificial
// Los valores reconocidos se encuentran en la matriz gameboard
void actualizarTablero(unsigned char tablero[3][3], unsigned char * gameboard);
// Función que simula colocar, por el brazo del robot, una ficha que representa
// a un jugador en una posición del tablero
void moverBrazoRobot(int fila, int columna, char ficha);


//Variables globales
unsigned char tablero[3][3]; //tablero del tres en raya 3x3
unsigned char jugadorhumano; //almacena cuál es el jugador humano (1 o 2)
unsigned char jugadorrobot; //almacena cuál es el jugador robot (1 o 2)
unsigned char turno; //almacena quién va a jugar la siguiente jugada (1 o 2)
//número de jugada del robot. Necesaria para saber a dónde se debe mover el brazo robot
//para coger una ficha
unsigned char numerojugadarobot=1; 

//Funcion principal
int main(void)
{
    
    Movimiento * casillasvacias; //lista de casillas vacías
    int numcasillasvacias; //número de casillas vacías

    
    inicializarTablero(tablero); //inicializa el tablero
    imprimirTablero(tablero); //imprime el tablero
    moverBrazoRobotPosicionInicial(); //mover brazo robot a posición inicial
    jugadorhumano=elegirJugadorHumano(); //elige la ficha con la que juega el jugador humano
    jugadorrobot=(jugadorhumano==1)?2:1; //almacenamos la ficha con la que juega el ordenador

    turno=elegirTurnoAleatorio(); //elige quién empieza a jugar (1 o 2)
    printf("El jugador humano es: %d\n",jugadorhumano);
    printf("El jugador ordenador es: %d\n",jugadorrobot);
    printf("Empieza jugando el jugador: %d\n",turno);
    juegoTresenRaya(tablero); //juego del tres en raya
    
    return 0;
}

//Funcion que inicializa el tablero
//ponemos las posiciones vacías a 0
void inicializarTablero(unsigned char tablero[3][3])
{
    int i,j;
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            tablero[i][j]=0; 
        }
    }
}

//Funcion que imprime el tablero
void imprimirTablero(unsigned char tablero[3][3])
{
    int i,j;
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(tablero[i][j]==0)
            {
                printf(" ");
            }
            else if(tablero[i][j]==1)
            {
                printf("O");  //Circulos
            }
            else if(tablero[i][j]==2)
            {
                printf("X");  //Equis
            }
            if(j<2)
            {
                printf("|");
            }
        }
        printf("\n");
        if(i<2)
        {
            printf("-----\n");
        }
    }
}

//Función para elegir el jugador humano
unsigned char elegirJugadorHumano()
{
    int jugador=0;
    do
    {
      printf("Elija ficha del jugador humano (1-circulo o 2-equis): ");
      scanf("%d",&jugador);
    }while(jugador!=1 && jugador!=2);
    return (unsigned char) jugador;
}

//Función para elegir el turno aleatorio
unsigned char elegirTurnoAleatorio()
{
    unsigned char turno;

    //Incizalizamos el generador de números aleatorios con una semilla
    //que viene dada por el número de segundos transcurridos desde 
    //la media noche del 1 de enero de 1970
    srand(time(NULL));  
    //lanzamos el dado al aire y obtenemos un número aleatorio entre 1 y 2
    turno=rand()%2+1;
    printf("El turno aleatorio es: %d\n",turno);
    return turno;
}

//Función para imprimir el turno actual
void imprimirTurno(char turno)
{
    if (turno==jugadorhumano)
    {
        printf("Turno del jugador humano\n");
    }
    else
    {
        printf("Turno del jugador robot\n");
    }
}

//Función que explora el tablero y devuelve la lista de casillas vacías
Movimiento * casillasVacias(unsigned char tablero[3][3], int * numcasillasvacias)
{
    int i,j;
    Movimiento * casillasvacias=NULL;

    //Inicializamos el número de casillas vacías a 0
    //Este es un parámetro que se pasa por referencia y que
    //al retornar devolverá el número de casillas del tablero
    //en las que hay un 0 (no hay ficha)
    *numcasillasvacias=0;
    
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(tablero[i][j]==0)
            {
                //si hay una casilla vacía
                //incrementamos el contador de casillas vacías
                (*numcasillasvacias)++;
                //redimensionamos el array de casillas vacías ya que
                //hemos encontrado una casilla vacía más
                casillasvacias=(Movimiento *)realloc(casillasvacias,(*numcasillasvacias)*sizeof(Movimiento));
                //almacenamos la fila y la columna de la casilla vacía en la
                //estructura Movimiento que se añade a la última posición del array
                casillasvacias[(*numcasillasvacias)-1].fila=i;
                casillasvacias[(*numcasillasvacias)-1].columna=j;
            }
        }
    }
    //retornamos un puntero a la dirección de comienzo del bloque de 
    //memoria que contiene la lista de casillas vacías
    return casillasvacias;
}

void imprimirCasillasVacias(Movimiento * casillasvacias, int numcasillasvacias)
{
    int i;
    
    if (casillasvacias!=NULL)
    {
        //Recorremos el array de casillas vacías  apuntado por casillasvacias
        //e imprimimos la fila y la columna de cada casilla vacía
        for(i=0;i<numcasillasvacias;i++)
        {
            printf("Fila: %d, Columna: %d\n",casillasvacias[i].fila,casillasvacias[i].columna);
        }
    }
    else
    {
        printf("No hay casillas vacías\n");
    }
}

//Función que cambia el turno del jugador en función del turno actual
char cambiarTurno(char turnoactual)
{
    return (turnoactual==1)?2:1;
}

//Función que pide la fila y la columna en la que se realizará la siguiente
//jugada del jugador humano
void realizarMovimientoHumano(unsigned char tablero[3][3], char jugador)
{
    int fila,columna;

    do{
      do
      {
        printf("Introduzca la fila (0,1,2): ");
        scanf("%d",&fila);
      }while(fila<0 || fila>2);
      do
      {
        printf("Introduzca la columna (0,1,2): ");
        scanf("%d",&columna);
      }while(columna<0 || columna>2);
      //Comprobamos si la casilla está ocupada
      if (tablero[fila][columna]!=0)
      {
        printf("Error: Casilla ocupada\n");
      }
    }while(tablero[fila][columna]!=0);   
    tablero[fila][columna]=jugador;
    
}

//Funcion que devuelve el ganador si lo hay o 0 si no lo hay
char comprobarGanador(unsigned char tablero[3][3])
{
    int i;
    char ganador=0;
    //Comprueba las filas
    for(i=0;i<3;i++)
    {
        if(tablero[i][0]==tablero[i][1] && tablero[i][1]==tablero[i][2] && tablero[i][0]!=0)
        {
            ganador=tablero[i][0];
        }
    }
    //Comprueba las columnas
    for(i=0;i<3;i++)
    {
        if(tablero[0][i]==tablero[1][i] && tablero[1][i]==tablero[2][i] && tablero[0][i]!=0)
        {
            ganador=tablero[0][i];
        }
    }
    //Comprueba la diagonal de izquierda a derecha
    if(tablero[0][0]==tablero[1][1] && tablero[1][1]==tablero[2][2] && tablero[0][0]!=0)
    {
        ganador=tablero[0][0];
    }
    //Comprueba la diagonal de derecha a izquierda
    if(tablero[0][2]==tablero[1][1] && tablero[1][1]==tablero[2][0] && tablero[0][2]!=0)
    {
        ganador=tablero[0][2];
    }
    return ganador;
}

//Función que imprime el ganador
void imprimirGanador(char jugador)
{
    if (jugador==jugadorhumano)
    {
        printf("El ganador es: Humano\n");
    }
    else
    {
        printf("El ganador es: Robot\n");
    }
}

//Función que imprime el empate
void imprimirEmpate()
{
    printf("Empate\n");
}

//Función que devuelve cierto si hay un empate, falso en caso contrario
char comprobarEmpate(unsigned char tablero[3][3])
{
    int i,j;
    char empate=CIERTO;

    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(tablero[i][j]==0)
            {
                empate=FALSO;
            }
        }
    }
    //Si no hay un ganador y no hay ninguna casilla vacía hay un empate 
    if(comprobarGanador(tablero)==FALSO && empate==CIERTO)
    {
        return CIERTO;
    }
    else
    {
        return FALSO;
    }
}

//Función para jugar al tres en raya
void juegoTresenRaya(unsigned char tablero[3][3])
{
    char ganador=FALSO;
    char empate=FALSO;
    
    do
    {
        imprimirTurno(turno); //imprime el turno
        if(turno==jugadorhumano)
        {
            //realiza el movimiento del jugador humano
            realizarMovimientoHumano(tablero,jugadorhumano); 
        }
        else
        {     
            //realiza la jugada del ordenador
            realizarMovimientoOrdenadorEstrategico(tablero,jugadorrobot); 
        }
        //imprime el estado del tablero
        imprimirTablero(tablero); 
        ganador=comprobarGanador(tablero); //devuelve si hay ganador
        empate=comprobarEmpate(tablero); //devuelve si hay empate
        turno=cambiarTurno(turno); //cambia el turno
    }
    while(ganador==FALSO && empate==FALSO);
    if(ganador!=FALSO)
    {
            imprimirGanador(ganador); //imprime el ganador
    }
    else
    {
       if(empate==CIERTO)
       {
            imprimirEmpate(); //imprime el empate
       }
    }

}
//Funcion que devuelve un Movimiento en una fila si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnFilaConCasillaVacia(unsigned char tablero[3][3], char jugador)
{
    int i,j;
    Movimiento * movimiento=NULL;
    for(i=0;i<3;i++)
    {
        if(tablero[i][0]==jugador && tablero[i][1]==jugador && tablero[i][2]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=i;
            movimiento->columna=2;
        }
        else if(tablero[i][0]==jugador && tablero[i][2]==jugador && tablero[i][1]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=i;
            movimiento->columna=1;
        }
        else if(tablero[i][1]==jugador && tablero[i][2]==jugador && tablero[i][0]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=i;
            movimiento->columna=0;
        }
    }
    return movimiento;
}
//Funcion que devuelve un Movimiento en una columna si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnColumnaConCasillaVacia(unsigned char tablero[3][3], char jugador)
{
    int i,j;
    Movimiento * movimiento=NULL;
    for(i=0;i<3;i++)
    {
        if(tablero[0][i]==jugador && tablero[1][i]==jugador && tablero[2][i]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=2;
            movimiento->columna=i;
        }
        else if(tablero[0][i]==jugador && tablero[2][i]==jugador && tablero[1][i]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=1;
            movimiento->columna=i;
        }
        else if(tablero[1][i]==jugador && tablero[2][i]==jugador && tablero[0][i]==0)
        {
            //Reservamos memoria para una estructura de tipo Movimiento
            movimiento=(Movimiento *)malloc(sizeof(Movimiento));
            //Almacenamos la fila y la columna de la casilla vacía en la estructura
            movimiento->fila=0;
            movimiento->columna=i;
        }
    }
    return movimiento;
}

//Funcion que devuelve un Movieinto en una diagonal si es ganador para un jugador, NULL en caso contrario
Movimiento * dosEnLineaEnDiagonalConCasillaVacia(unsigned char tablero[3][3], char jugador)
{
    Movimiento * movimiento=NULL;

    if(tablero[0][0]==jugador && tablero[1][1]==jugador && tablero[2][2]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=2;
        movimiento->columna=2;
    }
    else if(tablero[0][0]==jugador && tablero[2][2]==jugador && tablero[1][1]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=1;
        movimiento->columna=1;
    }
    else if(tablero[1][1]==jugador && tablero[2][2]==jugador && tablero[0][0]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=0;
        movimiento->columna=0;
    }
    else if(tablero[0][2]==jugador && tablero[1][1]==jugador && tablero[2][0]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=2;
        movimiento->columna=0;
    }
    else if(tablero[0][2]==jugador && tablero[2][0]==jugador && tablero[1][1]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=1;
        movimiento->columna=1;
    }
    else if(tablero[1][1]==jugador && tablero[2][0]==jugador && tablero[0][2]==0)
    {
        //Reservamos memoria para una estructura de tipo Movimiento
        movimiento=(Movimiento *)malloc(sizeof(Movimiento));
        //Almacenamos la fila y la columna de la casilla vacía en la estructura
        movimiento->fila=0;
        movimiento->columna=2;
    }
    return movimiento;
}

// Funcion para buscar una estrategia ganadora. Devuelve el movimiento ganador si lo hay, NULL en caso contrario
Movimiento * buscarEstrategiaGanadora(unsigned char tablero[3][3], char jugador)
{
    Movimiento * movimiento=NULL;


    movimiento=dosEnLineaEnFilaConCasillaVacia(tablero,jugador);
    if(movimiento==NULL)
    {
        movimiento=dosEnLineaEnColumnaConCasillaVacia(tablero,jugador);
        if(movimiento==NULL)
        {
            movimiento=dosEnLineaEnDiagonalConCasillaVacia(tablero,jugador);
        }
    }
    return movimiento;
}

// Funcion para buscar una estrategia defensiva. Devuelve un movimiento defensivo, si lo hay, NULL en caso contrario
Movimiento * buscarEstrategiaDefensiva(unsigned char tablero[3][3], char jugador)
{
    Movimiento * movimiento=NULL;

    movimiento=dosEnLineaEnFilaConCasillaVacia(tablero,jugador);
    if(movimiento==NULL)
    {
        movimiento=dosEnLineaEnColumnaConCasillaVacia(tablero,jugador);
        if(movimiento==NULL)
        {
            movimiento=dosEnLineaEnDiagonalConCasillaVacia(tablero,jugador);
        }
    }
    return movimiento;
}


//Funcion que intenta realizar un movimiento ganador para el jugador, si no puede, realiza un movimiento defensivo, si no puede, realiza un movimiento aleatorio
void realizarMovimientoOrdenadorEstrategico(unsigned char tablero[3][3], char jugador)
{
    Movimiento * movimiento=NULL;
    char contrincante=(jugador==1)?2:1;  // si el jugador es el 1 el contrincante es el 2 y viceversa
    unsigned char * gameboard;
    int pos;
    
    //El primer parametro es con el que juega el humano y el segundo con el que juega el ordenador
    //devuelve un puntero a una matriz de 3x3 con los valores de las casillas del tablero
    gameboard=reconocerTablero(contrincante,jugador); 
    //actualizar el tablero con los valores reconocidos
    actualizarTablero(tablero,gameboard); 
    if((movimiento=buscarEstrategiaGanadora(tablero,jugador))==NULL) //si no hay movimiento ganador
    {
        if((movimiento=buscarEstrategiaDefensiva(tablero,contrincante))==NULL) //si no estamos amenazados
        {
            Movimiento * casillasvacias=NULL; //lista de casillas vacías
            int numcasillasvacias=0; //número de casillas vacías
            casillasvacias=casillasVacias(tablero,&numcasillasvacias); //lista de casillas vacías
            if (casillasvacias!=NULL)  // si hay casillas vacías
            {
                //obtenemos un numero aleatorio entre 0 y el número de casillas vacías menos 1
                pos=rand()%numcasillasvacias; 
                //actualizamos la matriz que almacena el estado de las casiilas del tablero
                tablero[casillasvacias[pos].fila][casillasvacias[pos].columna]=jugador;
                //simulamos el movimiento del brazo robot a la posición de la casilla del tablero
                moverBrazoRobot(casillasvacias[pos].fila,casillasvacias[pos].columna,jugador); 
                //liberamos el bloque de memoria que almacena la lista de casillas vacías
                free(casillasvacias); //libera la memoria de la lista de casillas vacías
            }
        }
        else // si estamos amenazados realizamos movimiento para bloquear
        {
            //bloqueamos la jugada ganadora del contrincante colocando la "ficha" del jugador ordenador
            tablero[movimiento->fila][movimiento->columna]=jugador;
            //simulamos el movimiento del brazo robot a la posición de la casilla del tablero
            moverBrazoRobot(movimiento->fila,movimiento->columna,jugador); //mover brazo robot a la posición de la casilla del tablero
            //liberamos el bloque de memoria que almacena la posición del tablero donde se ha realizado el movimiento
            free(movimiento);
        }
    }
    else  // realizamos movimiento ganador
    {
        //colocamos la ficha del jugador ordenador en la casilla que le da la victoria
        tablero[movimiento->fila][movimiento->columna]=jugador;
        //simulamos el movimiento del brazo robot a la posición de la casilla del tablero
        moverBrazoRobot(movimiento->fila,movimiento->columna,jugador); 
        //liberamos el bloque de memoria que almacena la posición del tablero donde se ha realizado el movimiento
        free(movimiento);
    }
    numerojugadarobot++; //incrementamos el número de jugadas realizadas por el robot
    moverBrazoRobotPosicionInicial(); //mover brazo robot a posición inicial
    
}

// Mueve el brazo del robot a la posición inicial
void moverBrazoRobotPosicionInicial()
{
    printf("Moviendo brazo robot a posición inicial\n");
}

unsigned char * reconocerTablero(char jugadorhumano, char jugadorrobot)
{
    static unsigned char gameboard[3][3];
    
    //Código para reconocer el tablero
    printf("Aplicando visión artificial para reconocer el tablero\n");
    // Incorporar aquí el código de la visión artificial para reconocer el tablero
    // Por ahora simulamos el reconocimiento copiando el contenido de la variable
    // global tablero en la matriz gameboard
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            gameboard[i][j] = tablero[i][j];
        }
    }
    // Devuelve un puntero a una matriz de 3x3 con los valores de las casillas del tablero
    return (unsigned char *) gameboard;

}

// Mueve el brazo del robot a la posición de la casilla del tablero. 
// El parámetro ficha indica la ficha con la que juega el robot
void moverBrazoRobot(int fila, int columna, char ficha)
{
    // Debemos tener en cuenta el numero de jugada del robot para 
    // desplazarlo hasta donde deba coger la ficha y luego moverlo a
    // la posicion donde debe soltarla
    printf("Moviendo brazo robot a la casilla (%d,%d) para colocar la ficha %c\n", fila, columna, ficha==1?'O':'X');
}

void actualizarTablero(unsigned char tablero[3][3], unsigned char * gameboard)
{
    // Actualiza el tablero con los valores reconocidos
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            tablero[i][j] = gameboard[i*3+j];
        }
    }
}