// Faculty: BUT FIT 
// Course: PRL 
// Project Name: Pipeline Merge Sort  
// Name: Jakub Kuznik
// Login: xkuzni04
// Year: 2024

/* 
Pipline Merge sort process scheme, where each process has 
input and output fronts 
          ------    -----  ...  ------
---- |P1|        |P2|      ...      |Pn| -----
          ------    -----  ...  ------
*/

#include <iostream>
#include <fstream>
#include <queue>
#include "mpi.h"
#include <cmath>

#define FILENUMS "numbers"
#define UP 1
#define DOWN 2 

#define LAST_UP 0 
#define LAST_DOWN 1 
#define LAST_NON 2

using namespace std;

/**
 * This is the first process that will read the 
 * random numbers from the file FILENUMS. The process 
 * will then put the value alternately to two output fronts 
 * 
 * Also first process prints the input number array separated by spaces 
 */
void first_proces(){
    
    // send Buffer that will be used to send number to the p2 
    uint8_t sBuff;
    
    // get my rank 
    int rank;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::cout << "I am ZERO " << rank << std::endl;

    int total_nums = 0; 

    // Open the file     
    std::ifstream file;
    file.open(FILENUMS);

    if (!file.is_open()) {
        std::cerr << "Error opening file numbers" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // parse the input numbers and send then to the second process 
    char c;
    for (int i = 0; file.get(c); i++){
        sBuff = static_cast<uint8_t>(c);
        total_nums++;
        // results 
        // cout << static_cast<int>(sBuff) << " " << endl;
        if (i % 2 == 0){
            cout << "I am FIRST and i am sending " << static_cast<int>(sBuff) << " UP to " << rank+1 << std::endl;
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
        }
        else{
            cout << "I am FIRST and i am sending " << static_cast<int>(sBuff) << " DOWN to " << rank+1 << std::endl;
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
        }
    }
    int event = 0; // Event to be broadcasted
    // Broadcast the event from process 0 to all other processes
    MPI_Bcast(&total_nums, 1,MPI_INT, 0, MPI_COMM_WORLD);

    file.close();
    
    // after everything sent speciall signal to shut down everyone
}

/**
 * This contains logic of the nth process. 
 * LAST PROCESS IS printing the output  
 */
void nth_proces() {
    
    std::queue<uint8_t> q_up; 
    std::queue<uint8_t> q_down; 

    uint8_t rcBuff; 
    uint8_t sBuff; 

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    // Condition for the UPPER front to start the process 
    int condition  = pow(2, (rank-1));
    bool condition_met = false;

    // how many numbers are there in total  
    int total_nums      = 0; 
    int accepted_nums   = 0;
    

    MPI_Status status;

    cout << "I am " << rank << " My condition is: " << condition << endl;
    
    MPI_Bcast(&total_nums, 1, MPI_INT, 0, MPI_COMM_WORLD);


    for (int i = 0; condition_met == false; i++){
        if (i % 2 == 0){
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
            q_up.push(rcBuff);
            accepted_nums++;
        }
        else{
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
            q_down.push(rcBuff);
            accepted_nums++;
        }

        if (!q_down.empty() && (q_up.front() >= condition)){
            condition_met = true;
            cout << "I am " << rank << " And i'v MET A CONDITION" << endl;
        }
    }
    
    int send = 0;
    int ups = pow (2, (rank-1));
    char last = LAST_UP;
    int up_taken = 0;
    int can_take_up = false;
    int can_take_down = false; 
    int help = 0;
    bool oneEmpty = false; 
    for (int i = 0; true; i++){
        // last process is prinitng the output 
        if (accepted_nums == total_nums){
            cout << "I am " << rank << " ITERATION 2/2 ALL NUMS " << endl;
            if (q_up.empty() && q_down.empty()){
                cout << "I am " << rank << " ENDING " << endl;
                break;
            }
        }
        else { // RECIEVE 
            if (accepted_nums < total_nums){
                if (i % 2 == 0){
                    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
                    accepted_nums++; q_up.push(rcBuff);
                }
                else {
                    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
                    accepted_nums++; q_down.push(rcBuff);
                }
            }
        }

        if (send < ups){
            // choose the smaller and remeber which front it was from
            if (q_up.empty()){
                sBuff = q_down.front(); q_down.pop(); 
                up_taken--;
            }
            else if (q_down.empty()){
                sBuff = q_up.front(); q_up.pop(); 
                up_taken++;
            }
            else if (q_up.front() < q_down.front()){
                sBuff = q_up.front(); q_up.pop(); 
                up_taken++;
            }
            else{
                sBuff = q_down.front(); q_down.pop(); 
                up_taken--;
            }

            send++;
            // IF I AM THE LAST PROCESS     
            if (rank == size-1){
                cout << "I AM LAST, THIS IS RESULT: " << endl;
                cout << static_cast<int>(sBuff) << endl;
            }
            else{ // else send to next process 
                if (last == LAST_UP){
                    cout << "I am " << rank << " and i am sending " 
                    << static_cast<int>(sBuff) << " UP to: " << rank+1 << endl;
                    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
                }
                if (last == LAST_DOWN){
                    cout << "I am " << rank << " and i am sending " 
                    << static_cast<int>(sBuff) << " DOWN to: " << rank+1 << endl;
                    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
                }
            }
        }
        else if (send < (ups * 2)){
            cout << "I am " << rank << " i am FILLING REST "  << "count: "
            << send << " condi: " << static_cast<int>(ups*2) << endl;

            help = abs(up_taken);

            cout << "!!! I AM " << rank << " UP TAKEN: " << up_taken << " UPS*2: " 
            << ups*2 << " send: " << send << endl;

            if (up_taken == 0){
                can_take_down = true; can_take_up = true; 
            }
            // if more are taken from upper front 
            else if (up_taken > 0){
                if (((ups*2) - send - help) > 0){
                    can_take_up = true; can_take_down = true;
                    cout << "!!! I AM " << rank << " I CAN TAKE BOTH " << endl;
                }
                else{
                    can_take_down = true; can_take_up = false; 
                    cout << "!!! I AM " << rank << " I CAN TAKE DOWN" << endl;
                }
            }
            else {
                if (((ups*2) - send - help) > 0){   
                    can_take_down = true; can_take_up = true; 
                    cout << "!!! I AM " << rank << " I CAN TAKE BOTH " << endl;
                }
                else{
                    can_take_down = false; can_take_up = true; 
                    cout << "!!! I AM " << rank << " I CAN TAKE UP" << endl;
                }
            }
            
            oneEmpty = false; 
            
            if (can_take_down && can_take_up){
                if (q_up.front() < q_down.front()){
                    sBuff = q_up.front(); q_up.pop();
                    up_taken++;
                }
                else{
                    sBuff = q_down.front(); q_down.pop();
                    up_taken--;
                }
            }
            else if (can_take_down){
                if (q_down.empty()){
                    oneEmpty = true;
                    cout << "CAN TAKE DOWN BUT DOWN EMPTY .... up taken:" << 
                    up_taken << " send: " <<  send << " Possible " << ups*2 << endl;
                }else{
                    sBuff = q_down.front(); q_down.pop();;
                    up_taken--;
                }
            }
            else{
                if (q_up.empty()){
                    oneEmpty = true;
                    cout << "CAN TAKE UP BUT DOWN EMPTY .... up taken:" << 
                    up_taken << " send: " <<  send << " Possible: " << ups*2 << endl;
                }
                else {
                    sBuff = q_up.front(); q_up.pop();
                    up_taken++;
                }
            }

            if (oneEmpty == false){
                // IF I AM THE LAST PROCESS     
                send++;
                if (rank == size-1){
                    cout << "I AM LAST, THIS IS RESULT: " << endl;
                    cout << static_cast<int>(sBuff) << endl;
                }
                else{ // else send to next process 
                    if (last == LAST_UP){
                        cout << "I am " << rank << " and i am sending " 
                        << static_cast<int>(sBuff) << " UP to: " << rank+1 << endl;
                        MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
                    }
                    if (last == LAST_DOWN){
                        cout << "I am " << rank << " and i am sending " 
                        << static_cast<int>(sBuff) << " DOWN to: " << rank+1 << endl;
                        MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
                    }
                }
            }
        }
        else {
            if (last == LAST_UP){
                last = LAST_DOWN;
            }
            else{
                last = LAST_UP;
            }
            send = 0;
            up_taken = 0;
            continue;
        } 
    }
}


int main(int argc, char *argv[]) {

    int rank, size;
    
    MPI_Init(&argc, &argv);

    // get the number of process 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    cout <<  "THERE ARE: " << size << " Process " << endl;

    // get current procces id 
    // MPI_COMM_WORLD - prediefined constant to match all the processes  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        first_proces();
    }
    else {
        nth_proces();
    }


    MPI_Finalize();

    return 0;
}

