#include "common.h"

//Variables
int client_socket;

void send_file(fstream& f){
    char data[BUFFER_SIZE] = {0};

    while(!f.eof()){
        f.read(data, sizeof(data));
        int sz = f.gcount();
        cout << sz << endl;
        check(send(client_socket, &sz, sizeof(sz), 0), "Error in Transferring file");

        cout << data << endl;
        check(send(client_socket, &data, sizeof(data), 0), "Error in Transferring file");

        memset(data, 0, sizeof(BUFFER_SIZE));
    }
    
    int sz = 0;
    cout << sz << endl;
    check(send(client_socket, &sz, sizeof(sz), 0), "Error in Transferring file");
    // check(send(client_socket, data, sizeof(data), 0), "Error in Transferring file");
}


int main(int argc, char *argv[]){
    check(argc-2, "No IP address provided");

    //Create and open a client socket

    check(client_socket = socket(AF_INET, SOCK_STREAM, 0), "Error in opening socket for client");

    //Connect to Server socket
    SA_IN server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, (char *)argv[1], &(server_addr.sin_addr));

    if(connect(client_socket, (SA*)&server_addr, sizeof(server_addr)) != 0){
        cerr << "Error connecting to the server";
        exit(EXIT_FAILURE);
    }

    char req[BUFFER_SIZE], res[BUFFER_SIZE];
    string file;


    //File transfer requests by client
    do{
        memset(req, ' ', sizeof(req));
        cout << endl;
        system("clear");
        
        cout << "**************************************************\n";
        cout << "                HELLO DEAR USER\n";
        cout << "**************************************************\n";
        cout << "\n    ENTER THE ABSOLUTE OR RELATIVE FILE PATH YOU WANT TO UPLOAD:    ";
        cin >> file;

        fstream f;
        f.open(file, ios::in | ios::out);
        if(!f)
        {
            cout << "\n    THE FILE PATH YOU ENTERED IS INCORRECT.";
        }
        else{
            cout << "\n    ENTER THE FILE NAME YOU WANT TO SEND TO SERVER:    ";
            cin >> file;

            req[0] = '1';
            send(client_socket, &req, sizeof(req), 0);
            strcpy(req, file.c_str());
            send(client_socket, &req, sizeof(req), 0);

            cout << "\n\n    WAITING FOR SERVER SIDE TO ACCEPT THE REQUEST...";

            recv(client_socket, &res, sizeof(res), 0);
            if(res[0] == '0'){
                cout << "\n\n    SERVER DECLINED THE TRANSFER REQUEST.";
            }
            else{
                //FILE TRANSFER
                cout << "\nFILE TRANSFER STARTED...\n";
                int cnfrm;
                char d;

                send_file(f);

                // memset(req, 0, sizeof(req));
                
                // while (f.read(req, sizeof(req)))
                // {
                //     send(client_socket, req, sizeof(req), 0);
                // }
                
                // send(client, &cnfrm, sizeof(cnfrm), 0);

                f.close();
                cout << "FILE SUCCESSFULLY SENT.\n";
            }
        }
        cout << "\n\n  DO YOU WANT TO CONTINUE? (0/1)\n";
        cin >> req[0];

        if (req[0] == '1') {
            cout << "CONNTINUING...";
        }
        else{
            cout << "THANK YOU FOR USING ME. CLOSING THE CLIENT PROGRAM.\n";
        }
    }while(req[0] == '1');
}