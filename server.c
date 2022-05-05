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
#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>
#include "json.hpp"
#include <iomanip>
#include<string>
#include<time.h>

using namespace std;
using json = nlohmann::json;

string message;
string from;
string delivered_at;

string clientes[32][2];
int usuarios = 0;
string id[32][2];

struct client_t
{
    int id;
    char nickname[32];
};

vector<client_t> clients;

bool removeClient(int id)
{
    for (auto it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it).id == id)
        {
            clients.erase(it);
            return true;
        }
    }

    return false;
}

bool existClient(int id)
{
    for (auto &c : clients)
    {
        if (c.id == id)
            return true;
    }
    return false;
}

string getAllClients(int FD)
{
    string ret = "-----Lista de clientes-----\nID\tUsuario\n";
    for (auto &c : clients)
    {
        ret += to_string(c.id) + '\t';
        ret += string(c.nickname);
        if (c.id == FD)
        {
            ret += "\t(uno mismo)";
        }
        ret += '\n';
    }
    ret += "-----Lista de clientes-----\n";
    return ret;
}

void sendMsg2AllClients(const char *content)
{
    for (auto &c : clients)
    {
        send(c.id, content, strlen(content), 0);
    }
}

void sendMsg2ClientsExcept(const char *content, int exceptFD)
{
    for (auto &c : clients)
    {
        if (c.id == exceptFD)
            continue;
        send(c.id, content, strlen(content), 0);
    }
}


void *recvsocket(void *arg) 
{
    client_t *client = (client_t *)arg;
    int st = client->id;

    char s[1024];
    while (1)
    {
        memset(s, 0, sizeof(s));
        int rc = recv(st, s, sizeof(s), 0);

        if (rc <= 0)
            break;

            json request;

            request = json::parse(s);

            string reques = request["request"];


            if(reques == "INIT_CONEX"){
                string connect_time = request["body"][0];
                string user_id = request["body"][1];
                id[usuarios][0] = to_string(rc);
                id[usuarios][1] = user_id;
            
                clientes[usuarios][0] = user_id;
                clientes[usuarios][1] = "0";
                usuarios++;
                cout<<usuarios<<" Usuario nuevo "<<endl;
                json response;
                response["response"] = "INIT_CONEX";
                response["code"] = "200";
                //cout<<connect_time<<endl;
                //cout<<user_id<<endl;

                //fprintf(stderr, "%s", connect_time);
                //fprintf(stderr, "%s", user_id);
                string senviar = response.dump();
                strcpy(s, senviar.c_str());
                send(st, s, senviar.size()+1, 0);
                    
        } 


            if(reques == "GET_CHAT"){
                string all = request["body"];
                //crear un usuario con el user ID
                //clients.push_back(*client);

                json response;
                response["response"] = "GET_CHAT";
                response["code"] = "200";
                response["body"] = {{message, from, delivered_at}};

                //fprintf(stderr, "%s", connect_time);
                //fprintf(stderr, "%s", user_id);
                string senviar = response.dump();
                strcpy(s, senviar.c_str());
                send(st, s, senviar.size()+1, 0);
                    
        }     
 

            if(reques == "POST_CHAT"){
                message = request["body"][0];
                from = request["body"][1];
                delivered_at = request["body"][2];
                string to = request["body"][3];

                //crear un usuario con el user ID
                //clients.push_back(*client);

                json response;
                response["response"] = "POST_CHAT";
                response["code"] = "200";

                //fprintf(stderr, "%s", connect_time);
                //fprintf(stderr, "%s", user_id);
                string senviar = response.dump();
                strcpy(s, senviar.c_str());
                send(st, s, senviar.size()+1, 0);
                    
        } 
        if(reques == "GET_USER" && request["body"] == "all"){
        string all = request["body"];
        //crear un usuario con el user ID
        //clients.push_back(*client);

        json response;
        response["response"] = "GET_USER";
        response["code"] = "200";
        response["body"] = clientes;
        //fprintf(stderr, "%s", connect_time);
        //fprintf(stderr, "%s", user_id);
        string senviar = response.dump();
        strcpy(s, senviar.c_str());
        send(st, s, senviar.size()+1, 0);

        } 
        if(reques == "GET_USER"){
        cout<<"ENTRO";
        string username = request["body"];
        

        json response;
        response["response"] = "GET_CHAT";
        response["code"] = "200";
        for(int i=0; i<32; i++)
            {
            if(username==clientes[i][0]){
                response["body"] = {"127.0.0.1",clientes[i][1]};
            }

            cout<<"\n";
            }

        //fprintf(stderr, "%s", connect_time);
        //fprintf(stderr, "%s", user_id);
        string senviar = response.dump();
        strcpy(s, senviar.c_str());
        send(st, s, senviar.size()+1, 0);
                    
        } 
        if(reques == "PUT_STATUS"){
                string status_obtenido = request["body"];

                json response;
                response["response"] = "PUT_STATUS";
                response["code"] = "200";
                cout<<"ENTRO 1";

                for(int i=0; i<32; i++)
                    {
                    cout<<"RC"<<to_string(rc)<<"\n";
                    cout<<"ID"<<id[i][0]<<"\n";
                    if(to_string(rc)==id[i][0]){
                        cout<<"ENTRO 2";
                        if(id[i][1] == clientes[i][0]){
                            cout<<"ENTRO 3";
                            clientes[i][1] = status_obtenido;
                        }
                        response["body"] = {"127.0.0.1",clientes[i][1]};
                    }

                    cout<<"\n";
                    }

                string senviar = response.dump();
                strcpy(s, senviar.c_str());
                send(st, s, senviar.size()+1, 0);

        } 
 
        string receivedStr = string(s);

        if (receivedStr == "-a\n")
        {
            string list = getAllClients(st);
            send(st, list.c_str(), list.size(), 0);
            continue;
        }

        if (receivedStr == "-q\n")
        {
            break;
        }

        if (receivedStr.substr(0, 3) == "-nn")
        {
            string newName = receivedStr.substr(4, receivedStr.size() - 5);
            sprintf(client->nickname, "%s", newName.c_str());
            printf("Client{%d} Cambiar nombre de usuario a%s\n", st, newName.c_str());

            string reply = "Nombre de usuario cambiado! (" + newName + ")\n";
            send(st, reply.c_str(), reply.size(), 0);
            continue;
        }

        if (receivedStr.substr(0, 2) == "-p")
        {
            int toid;
            char msg[512];
            sscanf(receivedStr.substr(2).c_str(), "%d %s", &toid, msg);

            if (toid == st)
            {
                send(st, "Precaucion, no se puede enviar mensajes a uno mismo\n", 41, 0);
            }
            else if (existClient(toid))
            {
                string msg2send;

                msg2send = "***MENSAJE PRIVADO DEL CLIENTE{";
                msg2send += string(client->nickname) + "}***\n" + string(msg);
                msg2send += "\n***MENSAJE PRIVADO***\n";
                send(toid, msg2send.c_str(), msg2send.size(), 0);
            }
            else
            {
                send(st, "PRECAUCION ID INVALIDO\n", 21, 0);
            }
            continue;
        }

        char content[1024];
        sprintf(content, "CLIENTE{%d}: %s", st, s);
        printf("%s", content);
        sendMsg2ClientsExcept(content, st);
    }

    removeClient(st);
    close(st);
    pthread_cancel(pthread_self());
}

void *sendsocket(void *arg) 
{
    char s[1024];
    while (1)
    {
        memset(s, 0, sizeof(s));
        read(STDIN_FILENO, s, sizeof(s));

        string content = "SERVER: " + string(s);
        sendMsg2AllClients(content.c_str());
    }
}

int main(int arg, char *args[])
{
    int port = 6666;
    if (arg >= 2)
    {
        port = atoi(args[1]);
    }

    int st = socket(AF_INET, SOCK_STREAM, 0); 
    int on = 1;

    
    if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        printf("setsockopt failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;               
    addr.sin_port = htons(port);              
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    
    if (bind(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("bind failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    
    if (listen(st, 20) == -1)
    {
        printf("listen failed %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t thread_send;
    pthread_create(&thread_send, NULL, sendsocket, NULL);

    int client_st = 0;
    struct sockaddr_in client_addr;

    while (1)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);

        client_st = accept(st, (struct sockaddr *)&client_addr, &len);

        if (client_st == -1)
        {
            printf("Client connection failed %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        printf("New Client from: %s\n", inet_ntoa(client_addr.sin_addr));

        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->id = client_st;
        sprintf(client->nickname, "%d", client_st);

        clients.push_back(*client);

        pthread_t thrd1;
        pthread_create(&thrd1, NULL, recvsocket, client);

    }

    close(st);
    return EXIT_SUCCESS;
}
