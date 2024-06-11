// EJEMPLO DE APLICACION PARA OBTENER TEXTO DE UN SERVIDOR WEB (http)
// 1) Prepara socket cliente
// 2) Conecta con el servidor http (usar isa.uniovi.es = 156.35.152.5), puerto 80
// 3) Prepara solicitud para la pagina http://isa.uniovi.es/~ialvarez/Descargas/pagina_de_prueba.htm
// 4) Envia la solicitud y recibe el texto html
// 5) Decodifica texto html y busca enlaces a otras paginas
// 6) Si hay algun enlace, permite rehacer una solicitud para la pagina indicada en el mismo y volver al punto 3

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

// (*)
// Solución para la recepción en que los datos del servidor pueden llegar troceados en varios paquetes
// Se esperan datos del servidor durante un tiempo máximo, y se van añadiendo al destino si llegan troceados
int WaitAndReceive(SOCKET sock, char* dest, int tam_dest);


int main()
{
    // ------------------------------------------------------------------------------
    // 1) Preprara socket cliente
    // 1.1) En Windows, es necesario inicializar los sockets para la aplicación en 1er lugar (https://learn.microsoft.com/en-us/windows/win32/api/_winsock/)
    WSADATA wsaData;
    WORD wVersionRequerida = MAKEWORD(2, 2);

    if (WSAStartup(wVersionRequerida, &wsaData) != 0)     // Necesario en Windows para inicializar sockets para la aplicación
    {
        printf("Error inicializando sockets Windows\n");
        return -1;
    }

    // 1.2) Variables para gestionar comunicación de socket cliente
    SOCKET sock;        // Identificador de socket (equivalente a FILE* para archivos)
    struct sockaddr_in local, remoto;    // Direcciones de socket local y remoto
    char txt_host[256],         // Texto con el nombre del host, a incluir en la cabecera GET
        txt_page[256],         // Texto con la pagina a obtener del host, a incluir en la cabecera GET
        buf[256],
        rcv[40],               //Texto que recibe
        snd[40];               //Texto que envia
    char txt_snd[1024], txt_rcv[65536];  // Texto a enviar y recibir
    int n;              // Num de caracteres enviados o recibidos

    // 1.3) Obtener identificador para un socket TCP (stream)
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("No se puede crear socket. Programa terminado\n");
        return -2;
    }

    // 1.4) Preparar estructura con datos de IP/puerto local y llamar a bind() para que enlace el socket local
    local.sin_family = AF_INET;
    local.sin_addr.S_un.S_addr = 0;   // 0 = cualquier dirección IP disponible en el equipo
    local.sin_port = htons(0);        // 0 = cualquier puerto disponible en el equipo
    memset(local.sin_zero, 0, 8 * sizeof(char));
    if (bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_in)) != 0)
    {
        printf("No se puede asignar direccion local. Programa terminado\n");
        return -2;
    }

    // 1.5) Esperamos a que el usuario pulse INTRO antes de lanzar la solicitud de conexión
    printf("Pulse RETURN para conectar con Esp-32: ");
    getchar();

    // ------------------------------------------------------------------------------
    // 2) Conectar con el servidor deseado isa.uniovi.es (dirección ip 156.35.152.5)
    // 2.1) Preparar estructura con datos de IP/puerto remoto y realizar connect()
    remoto.sin_family = AF_INET;
    remoto.sin_addr.S_un.S_addr = inet_addr("192.168.23.1");
    remoto.sin_port = htons(55355);      // htons() -> Convierte a network order (big endian)
    memset(remoto.sin_zero, 0, 8 * sizeof(char));

    if (connect(sock, (struct sockaddr*)&remoto, sizeof(struct sockaddr_in)) != 0)
    {
        printf("No se puede conectar a servidor remoto. Programa terminado\n");
        return -3;
    }

    printf("Conectado con ESP-32\n\n");

    // ------------------------------------------------------------------------------
    // 3) Preparar solicitud GET al servidor HTTP


    // 3.1) Página a visitar y servidor: valores por defecto
    strcpy(txt_page, "http://isa.uniovi.es/~ialvarez/Descargas/pagina_de_prueba.htm");
    strcpy(txt_host, "isa.uniovi.es");
    int cnt = 0;
    int matriz[9];
    int n1 = WaitAndReceive(sock, rcv, 65536); //numero de datos recibidos
    rcv[n1] = 0;
    printf("Recibido: %s\n", rcv);

    char op;        // Opciones de finalizacion: 'S', 'N' ó 'F'
    
    if (strcmp(rcv, "<<<NAME>>>\n") == 0) {
        strcpy(snd, "<<<NAME=PC>>>\n");
        send(sock, snd, strlen(snd), 0);
        cnt = 1;
    }
    

    while (cnt) {
        int n = WaitAndReceive(sock, buf, 65536);
        if (strncmp(buf, "FOTO", 4) == 0) {
            char* pt = strchr(buf,'w');
            int w = atoi(pt + 2);
            pt = strchr(buf, 'h');
            int h = atoi(pt + 2);
            pt = strchr(buf, 'l');
            int l = atoi(pt + 2);
            pt = strchr(buf, '\n')+1;
            for (int i = 0; i < l; i++) {
                printf("%d, ",pt[i]);
            }
            //analizar la foto aqui......
            //send(sock, txt_snd, strlen(txt_snd), 0);
        }
    }

    for (op = 'S'; op != 'F';)   // Repetimos mientras el usuario no desee terminar
    {
        
        printf("Pulse RETURN para enviar solicitud GET %s :", txt_page);
        getchar();

        // 3.2) Preparar texto de solicitud de página web, según la especificación GET del protocolo HTTP (https://reqbin.com/req/nfilsyk5/get-request-example)
        sprintf(txt_snd, "GET %s HTTP/1.1\r\n", txt_page);
        sprintf(txt_snd + strlen(txt_snd), "Host: %s\r\n", txt_host);
        strcat(txt_snd, "Accept: text/html\r\n");
        strcat(txt_snd, "Keep-Alive: timeout=5\r\n");
        strcat(txt_snd, "\r\n");

        // ------------------------------------------------------------------------------
        // 4) Enviar solicitud y esperar respuesta

        // 4.1) Enviar texto de solicitud al remoto
        n = send(sock, txt_snd, strlen(txt_snd), 0);
        if (n != strlen(txt_snd))
        {
            printf("Error en envio\n");
            printf("Fin del programa. Pulse INTRO para terminar ");
            getchar();
            return -4;
        }

        printf("Enviado texto: %s\n\n", txt_snd);

        // 4.2) Esperar respuesta y guardar en txt_rcv

//        n=recv(sock,txt_rcv,65536,0);       // Esta forma de recepción es "incorrecta": supone que toda la lectura se realiza en 1 único recv(), pero
                                            // en sockets tipo stream puede venir troceada. Ver (*) al final para una solución más elegante, que se invocaría con:
        n = WaitAndReceive(sock, txt_rcv, 65536);

        if (n <= 0)
        {
            printf("Error en recepcion\n");
            printf("Fin del programa. Pulse INTRO para terminar ");
            getchar();
            return -5;
        }

        // ------------------------------------------------------------------------------
        // 5) Decodificar respuesta

        txt_rcv[n] = 0;   // Asegura que la cadena recibida termina en el caracter nulo
        printf("Recibido texto: %s\n\n", txt_rcv);

        //// 5.1) Bucle para buscar enlaces html (cadenas en el texto recibido con el formato <a href="direccion_del_enlace">Texto</a>)
        //char* ptStart, * ptEnd;   // ptStart=lugar donde comenzar a buscar, ptEnd=fin de busqueda
        //for (ptStart = txt_rcv; ptStart != NULL;)    // Comenzamos desde el principio de la cadena, mientras encontremos algo
        //{
        //    ptStart = strstr(ptStart, "<a href=");     // Buscamos la parte comun a cualquier enlace
        //    if (ptStart != NULL)      // Hemos encontrado la parte común
        //    {
        //        ptEnd = strchr(ptStart, '>');      // Buscamos el final del tag <a .... >
        //        ptStart += strlen("<a href=");
        //        ptStart++;      // Apunta al siguiente caracter tras las comillas de comienzo de dirección de enlace
        //        ptEnd--;        // Apunta a las comillas de final de dirección de enlace
        //        n = ptEnd - ptStart;
        //        strncpy(txt_page, ptStart, n);        // Modifica la pagina a visitar para la siguiente solicitud
        //        txt_page[n] = 0;

        //        printf("Referencia a: %s\n", txt_page);
        //        printf("Ir a pagina? <S/N/F> (Si/No/Fin): ");

        //        for (op = 'X'; op != 'S' && op != 'N' && op != 'F';)
        //        {
        //            op = toupper(getchar());
        //        }
        //        getchar();  // Elimina \n del buffer de teclado

        //        if (op == 'S' || op == 'F')
        //            break;
        //        if (op == 'N')
        //            ptStart = ptEnd + 1;
        //    }
        //}
    }

    closesocket(sock);
    printf("Fin del programa. Pulse INTRO para terminar ");
    getchar();

    return 0;
}

// (*)
// Solución para la recepción en que los datos del servidor pueden llegar troceados en varios paquetes
// Se esperan datos del servidor durante un tiempo máximo, y se van añadiendo al destino si llegan troceados
int WaitAndReceive(SOCKET sock, char* dest, int tam_dest)
{
    ULONG NonBlock = 1;

    if (ioctlsocket(sock, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
        fflush(stdout);
        return -1;
    }

    int n_recv, total_len_recv;
    for (total_len_recv = 0; total_len_recv < tam_dest;)
    {
        FD_SET rd;
        struct timeval tout = { 00,500000 };     // Time-out: 0 s + 500000 us=500 ms
        FD_ZERO(&rd);
        FD_SET(sock, &rd);
        if (select(1, &rd, NULL, NULL, &tout) > 0)    // Comprueba si en sock se ha recibido algo; termina cuando se recibe o vence el timeout
        {
            n_recv = recv(sock, dest + total_len_recv, tam_dest - total_len_recv, 0);  // Como máximo caben tam_dest-len_recv
            if (n_recv > 0)
                total_len_recv += n_recv;    // Acumulamos
            else
                break;
        }
        else
            break;
    }

    return total_len_recv;
}