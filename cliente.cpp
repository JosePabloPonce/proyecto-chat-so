#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <stdbool.h>
#include <iostream>
#include "json.hpp"
#include <iomanip>
#include<string>
#include<time.h>


using namespace std;
using json = nlohmann::json;
char *usuario;
string status;

void *recvsocket(void *arg) 
{
    int st = *(int *)arg;
    json request;
    json response;
    char buffer[1024];
    request["request"] = "GET_CHAT";
    request["body"] = "all";

    string parsed_request = request.dump();

    strcpy(buffer, parsed_request.c_str());
    send(st, buffer, parsed_request.size()+1, 0);

        while(1){

        int rc = recv(st, buffer, 1024, 0);
       // if (rc <= 0) 
         //           break;
        response = json::parse(buffer);
        string respons = response["response"];
        string code = response["code"];
        string message = response["body"][0][0];
        string from = response["body"][0][1];
        string delivered_at = response["body"][0][2];

        if(code == "200"){
            printf(" Mensajes Obtenidos\n");
        }
    }
    
}


void *sendsocket(void *arg) 
{
    int st = *(int *)arg;

    // char client_username[100];
    // sprintf(client_username, "NEW_CLIENT_USERNAME %s", "Amy");
    // send(st, client_username, strlen(client_username), 0);

    char s[1024];
    while(1){
        json request;
        json response;
        char buffer[1024];
        request["request"] = "POST_CHAT";

        memset(s, 0, sizeof(s));
        read(STDIN_FILENO, s, sizeof(s));
        //cin>>message;
        string from;
        from = usuario;
        time_t delivered_at = time(0);
        string toUser = "all";
        cout<<s<<from<<ctime(&delivered_at)<<toUser<<endl;
        request["body"] = {s, from, ctime(&delivered_at), toUser};
        string parsed_request = request.dump();

        strcpy(buffer, parsed_request.c_str());
        send(st, buffer, parsed_request.size()+1, 0);
        
        
        
        memset(buffer, 0, sizeof(buffer));
        cout<<"LLEGO"<<endl;
        int rc = recv(st, buffer, sizeof(buffer), 0);
        if(rc<0){
            break;
        }
        cout<<"LLEGO2"<<endl;
        response = json::parse(buffer);
        string respons = response["response"];
        string code = response["code"];
        cout<<"RESPUESTA SERVER: "<<respons<<code<<endl;

        if(code == "200"){
            printf(" Mensaje Enviado");
            } 
    }
    
        
}

string changeStatus(string opcion){
    if(opcion == "0"){
        status = "ACTIVO";

    }
    if(opcion == "1"){
        status = "OCUPADO";

    }
    if(opcion == "2"){
        status = "INACTIVO";
    }
    return status;
}

int main(int arg, char *args[])
{
    string ip = "0.0.0.0";
    int port = 6666;
    int entrada;
    changeStatus("0");

    if (arg >= 3)
    {
        ip = string(args[2]);
        port = atoi(args[3]);
        usuario = args[1];
    }

    int st = socket(AF_INET, SOCK_STREAM, 0); 

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("connect failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    json request;
    json response;
    time_t hora = time(0);

    char buffer[1024];
    request["request"] = "INIT_CONEX";
    request["body"] = {ctime(&hora), usuario};

    string parsed_request = request.dump();

    strcpy(buffer, parsed_request.c_str());
    send(st, buffer, parsed_request.size()+1, 0);

    recv(st, buffer, 1024, 0);

    response = json::parse(buffer);
    int code = response["code"];
    string respons = response["response"];


    if(code == 200){
        printf("Conexion Exitosa\n");
    } 

    pthread_t thrd1, thrd2, thrd3;
    bool bandera = true;

    while(bandera)
    {
        printf( "---------------------------------------------------\n");
        printf( "Usuario: %s \n",usuario);
        cout<<"ESTATUS ACTUAL: "<<status<<endl;
        printf( "\n");
        printf( "Ingrese el numero de la opcion que deasea ejecutar:\n");
        printf( "1. Chatear con todos los usuarios (broadcasting)\n");
        printf( "2. Enviar y recibir mensajes directos, privados, aparte del chat general\n");
        printf( "3. Cambio de status\n");
        printf( "4. Listar los usuarios conectados al sistema de chat\n");
        printf( "5. Desplegar información de un usuario en particular\n");
        printf( "6. Ayuda \n");
        printf( "7. Salir \n");
        printf( "---------------------------------------------------\n");
        printf( "\n");
    

    
    printf( "Introduzca la opcion que desea ejecutar (1-7): \n");
    scanf("%d", &entrada);
    printf( "---------------------------------------------------\n");
    printf( "\n");

 
    //Mensaje directo
    if(entrada == 1){
    	printf("=== BIENVENIDO AL CHAT GRUPAL ===\n");
        pthread_create(&thrd2, NULL, sendsocket, &st);
        pthread_create(&thrd1, NULL, recvsocket, &st);
        pthread_join(thrd1, NULL);
        /*while(1){
        char s[1024];
        json request;
        json response;
        char buffer[1024];
        request["request"] = "POST_CHAT";
        string message;
        cin>>message;
        string from;
        from += usuario;
        time_t delivered_at = time(0);
        string to = "all";
        request["body"] = {message, from, ctime(&delivered_at), to};
        string parsed_request = request.dump();
        strcpy(buffer, parsed_request.c_str());
        send(st, buffer, parsed_request.size()+1, 0);
        int rc = recv(st, buffer, 1024, 0);
        //if (rc <= 0) 
          //          break;
        response = json::parse(buffer);
        string respons = response["response"];
        string code = response["code"];
        if(code == "200"){
            printf(" Mensaje Enviado");
            } 
        }*/
        //pthread_create(&thrd1, NULL, recvsocket, &st);
        //pthread_create(&thrd2, NULL, sendsocket, &st);
        //pthread_join(thrd1, NULL);

    
    }

    //Broadcast
    if(entrada == 2){
        printf("=== BIENVENIDO A LA SALA PRIVADA ===\n");
        break;
    }
 


    //Cambio de status
    if(entrada == 3){
        json request;
        json response;
        status = ""; 

        char buffer[1024];
        request["request"] = "PUT_STATUS";
        printf( "Cambio de estatus\n");
        printf( "Ingrese el numero de la opcion que desea:\n");
        printf( "0. Activo\n");
        printf( "1. Ocupado\n");
        printf( "2. Inactivo\n");
        printf( "\n");
        printf( "Introduzca la opcion que desea ejecutar (0-2): \n");
        printf( "\n");
        cin>>status;
        request["body"] = status;
        changeStatus(status);

        string parsed_request = request.dump();

        strcpy(buffer, parsed_request.c_str());
        send(st, buffer, parsed_request.size()+1, 0);

        int rc = recv(st, buffer, 1024, 0);
       // if (rc <= 0) 
         //           break;
        response = json::parse(buffer);
        string respons = response["response"];
        int code = response["code"];

        if(code == 200){
            printf(" Cambio el estatus\n");
        }

    }

    //Listado de usuarios
    if(entrada == 4){
        json request;
        json response;
        char buffer[1024];
        request["request"] = "GET_USER";
        request["body"] = "all";

        string parsed_request = request.dump();

        strcpy(buffer, parsed_request.c_str());
        send(st, buffer, parsed_request.size()+1, 0);

        int rc = recv(st, buffer, 1024, 0);
       // if (rc <= 0) 
         //           break;
        response = json::parse(buffer);
        string respons = response["response"];
        int code = response["code"];

        if(code == 200){
            printf(" Usuarios Obtenidos\n");
            for(int i=0; i<32; i++)
                {
                for(int j=0; j<2; j++)
                {
                    cout<<" "<<response["body"][i][j]<<" ";
                }
                cout<<"\n";
        }
        } 
        //obtenerInfoAllUsers(sockfd, buffer);
    }

    //Informacion de usuario
    if(entrada == 5){
        json request;
        json response;
        //time_t hora = time(0);

        char buffer[1024];
        request["request"] = "GET_USER";
        string usuario;
        cout<<"Ingrese el usuario:\n"<<endl;
        cin>>usuario;
        request["body"] = usuario;

        string parsed_request = request.dump();

        strcpy(buffer, parsed_request.c_str());
        send(st, buffer, parsed_request.size()+1, 0);

        recv(st, buffer, 1024, 0);

        response = json::parse(buffer);
        int code = response["code"];
        string respons = response["response"];
        string ip = response["body"][0];
        string protocol = response["body"][1];


        if(code == 200){
            printf("Usuario Obtenido\n");
            cout<<"IP del usuario: "<<ip<<"\n";
            cout<<"Estado del usuario: "<<protocol<<"\n";
            //printf("%d", respons);


        } 
        //obtenerInfoUsuario(IdGlobal, usuarioInf, sockfd, buffer);
    }

    if(entrada == 6){
        printf("PARA CHAT PERSONAL UTILIZAR EL SIGUIENTE FORMATO <usuario> <mensaje>\n");
        printf("PARA BROADCASTING UTILIZAR EL SIGUIENTE FORMATO <mensaje>\n");
        printf("PARA CAMBIAR DE ESTADO INGRESAR <ESTADO> = 0, 1, 2\n");
        printf("PARA DESPLEGAR INFORMACION DE UN USUARIO EN PARTICULAR <ID>\n");
        printf("PARA LISTAR LOS USUARIOS CONECTADOS INGRESAR LA OPCION 4\n");           }
    //salir

    if(entrada == 7){
        printf("SALIENDO...\n");
        bandera = false;
        break;
    
    }
    }

    close(st);
    return EXIT_SUCCESS;
}
