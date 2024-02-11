/* 
 * tcpserver.c - A multithreaded TCP echo server 
 * usage: tcpserver <port>
 * 
 * Testing : 
 * nc localhost <port> < input.txt
 */
 #define TCP_HEADER
#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;


class Server{
    public:
        int PORT;
        string IP;
        Server(int port){
            PORT = port;
            sockaddr_in server_addr;
            server_addr.sin_port = htons(PORT);
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;

            sockid = socket(AF_INET, SOCK_STREAM, 0);
            if(bind(sockid, (sockaddr*)&server_addr, sizeof(server_addr)) < 0){
                cerr << "Port is alrady been use" << endl;
                exit(0); 
            }

            if(listen(sockid, 5) < 0){
                cerr<< "Error Listening" << endl;
                exit(0);
            }
            
            Add_request();
            close(sockid);
        }

        int Add_request(){
            int conn;
            char buffer[buffsize];
            char res[buffsize];
            while(1){
                conn = accept(sockid, nullptr, nullptr); 
                if(conn < 0){
                    cerr << "Error" << endl;
                    exit(0);
                }
                memset(buffer, 0, buffsize);
                memset(res, 0, buffsize);
                read(conn, buffer, buffsize);
                parse_command(buffer);
                exec(res, conn);
                write(conn, "Done\n", 5);
            }
        }

    private:
        unordered_map<string, string> kv_store;
        queue<string> requests;
        void parse_command(char* buffer){
            char* p = buffer;
            int mode = 0;
            int i=0;
            while(*p && i<buffsize){
                string temp = "";
                while(*p != '\n'){
                    i++;
                    temp += *p;
                    p++;
                }
                p++;
                i++;
                if(temp == "READ"){
                    mode = 1;
                    temp = "1";
                }
                else if(temp == "WRITE"){
                    mode = 2;
                    temp = "2";
                }
                else if(temp == "COUNT"){
                    mode = 3;
                    temp = "3";
                }
                else if(temp == "DELETE"){
                    mode = 4;
                    temp = "4";
                }
                else if(temp == "END"){
                    mode = 5;
                    temp = "5";
                }
                if(mode == 1 || mode == 4){
                    temp += ' ';
                    while(*p != '\n'){
                        i++;
                        temp += *p;
                        p++;
                    }
                    p++;
                    i++;
                }
                if(mode == 2){
                    temp += ' ';
                    while(*p != '\n'){
                        i++;
                        temp += *p;
                        p++;
                    }
                    p++;
                    i++;
                    while(*p != '\n'){
                        i++;
                        temp += *p;
                        p++;
                    }
                    p++;
                    i++;
                }
                requests.push(temp); 
            }
            buffer[i] = 0;
        }
        void exec(char* res, int sock){
            string temp = "";
            string key;
            string val;
            while(!requests.empty()){
                temp = requests.front();
                requests.pop();
                if(temp[0] == '1'){
                    key = temp.substr(1, temp.size() -1);
                    cout << "read " <<endl;
                    cout<< key << endl;
                    if(kv_store.find(key) == kv_store.end()){
                        write(sock, "NULL\n", 5);
                    }
                    else write(sock, (kv_store[key]+"\n").c_str(), kv_store[key].size()+1);

                }
                else if(temp[0] == '2'){
                    size_t idx = temp.find(':');
                    key = temp.substr(1, idx-1);
                    val = temp.substr(idx+1, temp.size() - idx -1);
                    cout << "write " <<endl;
                    cout<< key << endl;
                    cout<< val << endl;
                    kv_store[key] = val;
                    write(sock, "FIN\n", 4);
                }

                else if(temp[0] == '3'){
                    cout << "count " << endl;
                    write(sock, (to_string(kv_store.size())+"\n").c_str(), (to_string(kv_store.size())).size()+1);
                }
                else if(temp[0] == '4'){
                    key = temp.substr(1, temp.size() - 1);
                    cout << "delete " << "\n" << key << endl;
                    if(kv_store.find(key) == kv_store.end()){
                        write(sock, "NULL\n", 5);
                    }
                    else write(sock, "FIN\n", 4);
                }
                else{
                    cout << "end " << endl;
                    write(sock, "\n", 1);
                    close(sock);
                }

            }
        }
        int sockid;
        int buffsize = 1024;
};

//----------------------




int main(int argc, char ** argv) {
  int portno; /* port to listen on */
  
  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // DONE: Server port number taken as command line argument
  portno = atoi(argv[1]);
  Server server(portno);

}