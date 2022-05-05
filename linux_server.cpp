#include "common.h"

void write_file(char* req, int client){
    string actualPath = UPLOAD_DESTINATION + (string)"/" + (string)req;

    int sz;
    char data[BUFFER_SIZE];
    memset(data, 0, sizeof(data));

    int n;

    fstream f;
    f.open(actualPath, ios::out);
    if(!f){
        cerr << "Unable to open the file";
        return;
    }
    else{
        cout << "\n\n*****************************************\n";
        cout << " FILE TRANSFER STARTED" << endl;
        cout << "*****************************************\n";

        while(true){
            check(n = recv(client, &sz, sizeof(sz), 0), "Here");
            if(n <= 0){
                cout << "Error in transferring file....aborting.";
                f.close();
                return;
            }
            if(sz <= 0){
                cout << " FILE TRANSFER COMPLETED" << endl;
                cout << "*****************************************\n";
                return;
            }
            cout << sz << " ";
            cout << "Remaining data to transfer..." << endl;

            check(n = recv(client, &data, sizeof(data), 0), "Here");
            cout << data << endl;

            if(n <= 0){
                cout << "Error in transferring file....aborting.";
                f.close();
                return;
            }
            else{
                f.write(data, sz);
                memset(data, 0, sizeof(BUFFER_SIZE));
            }
        }
    }

}

//Variables
SA_IN server_addr, client_addr;
vector<int> client_socket;


int server_setup(int port, int backlog){
    //Create variables
    int server_socket;
    
    int opt = true;

    //Open a server socket
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0), "Error in opening Socket");

    //Add socket opts - To reuse the port and address even if taken
    check(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)), "Error in assigning socket options");

    //Initialize the address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);


    //Bind the socket to Port
    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)) < 0, "Failed to bind the socket to the port");


    //Listen for connections on the port
    cout << "Listening on the Port: " << port << endl;
    check(listen(server_socket, backlog) < 0, "Error: Cannot listen for new connections");

    //Return the FD of created server socket
    return server_socket;
}

int acceptConnections(int server_socket){
    int addr_size = sizeof(server_addr);
    int client_socket;

    check((client_socket = 
        accept(server_socket, (SA*)&server_addr, (socklen_t*)&addr_size)), "Failed in accepting client's connection request");

    return client_socket;
}

int handleRequest(int cur_client){
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    int addrlen = sizeof(client_addr);

    if(recv(cur_client, &request, sizeof(request), 0) == 0){
        getpeername(cur_client, (SA*)&client_addr, (socklen_t*)&addrlen);

        cout << "\n\n\n******************************\n";
        cout << " CLIENT DISCONNECTED:\n";
        cout << "******************************\n";
        cout << "\tIP: " << inet_ntoa(client_addr.sin_addr) << endl;
        cout << "\tPORT: " << ntohs(client_addr.sin_port) << endl;

        close(cur_client);
        return 0;
    }
    else{
        getpeername(cur_client, (SA*)&client_addr, (socklen_t*)&addrlen);

        cout << "\n\n\n*****************************************\n";
        cout << " CLIENT HAS SENT A FILE TRANSFER REQUEST:\n";
        cout << "*****************************************\n";
        cout << endl;
        cout << "\tCLIENT IP: " << inet_ntoa(client_addr.sin_addr) << endl;
        cout << "\tPORT: " << ntohs(client_addr.sin_port) << endl;
        memset(request, 0, sizeof(request));
        recv(cur_client, &request, sizeof(request), 0);
        cout << "\tFILE NAME: " << request << endl << endl;

        cout << "*****************************************\n";
        cout << "\tDO YOU WANT TO ACCEPT THE REQUEST (0/1)?\t";
        cin >> response[0];
        send(cur_client, &response, sizeof(response), 0);

        if(response[0] == '0')
            cout << "\tDECLINED THE REQUEST.\n";
        else
        {
            //FILE TRANSFER
            write_file(request, cur_client);
        }
    }

    return 1;
}



int main(int argc, char const *argv[]){
    //Some variables
    int cur_client_sock;

    int activity_ptr;

    int fd, max_fd;

    fd_set client_fds;

    //Setup the server
    int server_socket = server_setup(PORT, SERVER_BACKLOG);

    //Run the server infinitely
    cout << "Waiting for connection requests from clients..." << endl;
    while(true){
        //Setup socket set for select()
        FD_ZERO(&client_fds);
        FD_SET(server_socket, &client_fds);

        max_fd = server_socket;

        //Get the maximum socket currently active
        for(int i = 0; i < client_socket.size(); i++){
            fd = client_socket[i];

            if(fd > 0)FD_SET(fd, &client_fds);

            if(fd > max_fd)max_fd = fd;
        }

        //Call select(): It will wait for any of the sockets from the "client_fds" socket set to become active for I/O
        check(activity_ptr = select(max_fd + 1, &client_fds, NULL, NULL, NULL) + (errno == EINTR), "Error: select statement failed");

    
        //Accept any incoming connection
        if(FD_ISSET(server_socket, &client_fds)){
            cur_client_sock = acceptConnections(server_socket);
        
            cout << "\n\n\n******************************\n";
            cout <<  " CLIENT CONNECTED:\n";
            cout << "******************************\n";
            cout << "\tSOCKET FD: " << cur_client_sock << endl;
            cout << "\tIP: " << inet_ntoa(client_addr.sin_addr) << endl;
            cout << "\tPORT: " << ntohs(client_addr.sin_port) << endl;

            client_socket.push_back(cur_client_sock);
            cout << "Client added to the socket set";
        }

        //Do the process with the accepted connection
        for(int i = 0; i < client_socket.size(); i++){

            if(FD_ISSET(client_socket[i], &client_fds)){
                if(handleRequest(client_socket[i]) == 0){
                    client_socket.erase(client_socket.begin() + i);
                    i--;
                };
            }
        }
    }
    return 0;
}
