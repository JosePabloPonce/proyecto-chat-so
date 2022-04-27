#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

bool inicio = true;
char *usuario;
char *ip;
char *status = "ACTIVO";
char buffer[1024];
int entrada,entryStatus;

void menu(int clientSocket)
{
    if (inicio == true)
    {
        printf( "\n");
        printf( "---------------------------------------------------\n");
        printf( "Bienvenido a Chat SISTOS %s\n",usuario);
        printf( "Status Actual: %s \n",status);
        printf( "Ingrese el numero de la opcion que deasea ejecutar\n");
        printf( "1. Mensaje directo\n");
        printf( "2. Broadcasting (Chat general) \n");
        printf( "3. Cambio de status\n");
        printf( "4. Listado de usuarios\n");
        printf( "5. Informacion de un usuario\n");
        printf( "6. Salir \n");
        printf( "---------------------------------------------------\n");
        printf( "\n");
    }

    else if (inicio == false)
    {
        printf( "---------------------------------------------------\n");
        printf( "Usuario: %s",usuario);
        printf( "ESTATUS ACTUAL: %s \n", status);
        printf( "\n");
        printf( "Ingrese el numero de la opcion que deasea ejecutar:\n");
        printf( "1. Mensaje directo\n");
        printf( "2. Broadcasting (Chat general) \n");
        printf( "3. Cambio de status\n");
        printf( "4. Listado de usuarios\n");
        printf( "5. Informacion de un usuario\n");
        printf( "6. Salir \n");
        printf( "---------------------------------------------------\n");
        printf( "\n");
    }

    inicio = false;
    printf( "Introduzca la opcion que desea ejecutar (1-6): \n");
    bzero(buffer, 1024);
    scanf("%d", &entrada);
    printf( "---------------------------------------------------\n");
    printf( "\n");

    while (entrada < 1 || entrada > 6)
        ;

    switch (entrada)
    {
    //Mensaje directo
    case 1:
        break;

    //Broadcast
    case 2:
    	while(true){
            printf("Client: \t");
		    scanf("%s", &buffer[0]);
		    send(clientSocket, buffer, strlen(buffer), 0);
		    if(strcmp(buffer, ":exit") == 0){
			    //close(clientSocket);
			    printf("SALIENDO...\n");
                exit(1);
                
		    }   

		    if(recv(clientSocket, buffer, 1024, 0) < 0){
			    printf("[-]Error in receiving data.\n");
		    }else{
			    printf("Server: \t%s\n", buffer);
		    }
	    }   


    //Cambio de status
    case 3:
        printf( "Cambio de estatus\n");
        printf( "Ingrese el numero de la opcion que desea:\n");
        printf( "1. Activo\n");
        printf( "2. Ocupado\n");
        printf( "3. Inactivo\n");
        printf( "\n");
        printf( "Introduzca la opcion que desea ejecutar (1-3): \n");
        scanf("%d", &entryStatus);
        printf( "\n");

        while (entryStatus != 0)
        {
            if (entryStatus == 1)
            {
                status = "Activo";
                //CambioStatus(IdGlobal, status, sockfd, buffer);
                menu(clientSocket);
            }
            else if (entryStatus == 2)
            {
                status = "Ocupado";
                //CambioStatus(IdGlobal, status, sockfd, buffer);
                menu(clientSocket);
            }
            else if (entryStatus == 3)
            {
                status = "Inactivo";
                //CambioStatus(IdGlobal, status, sockfd, buffer);
                menu(clientSocket);
            }
            else
            {
                printf( "ERROR! Ingrese un opcion valida\n");
                menu(clientSocket);
            }
        }

    //Listado de usuarios
    case 4:
        //obtenerInfoAllUsers(sockfd, buffer);
        break;

    //Informacion de usuario
    case 5:
        printf( "Ingrese el usuario que desea ver: \n");
        printf( "\n");
        //obtenerInfoUsuario(IdGlobal, usuarioInf, sockfd, buffer);
        break;

    //salir
    case 6:
        break;
        while (entrada != 6)
            ;
    }
}

int main(int argc, char *argv[]){

	int clientSocket, ret, portno;
	struct sockaddr_in serverAddr;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

    usuario = argv[2];
    ip = argv[3];
    portno = atoi(argv[4]);

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portno);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
    menu(clientSocket);
    printf("DESCONECTANDO...\n");
    close(clientSocket);

	return 0;
}
