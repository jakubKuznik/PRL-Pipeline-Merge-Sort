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
    int rank, size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // get the number of process 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int total_nums = 0; 

    // Open the file     
    std::ifstream file;
    file.open(FILENUMS);

    if (!file.is_open()) {
        std::cerr << "Error opening file numbers" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char c;
    // If there is only one number 
    if (rank == size-1){
        file.get(c);
        sBuff = static_cast<uint8_t>(c);
        cout << static_cast<int>(sBuff) << " " << endl;
        cout << static_cast<int>(sBuff) << endl;
        return;
    }
    
    // parse the input numbers and send then to the second process 
    for (int i = 0; file.get(c); i++){
        sBuff = static_cast<uint8_t>(c);
        total_nums++;
        // results 
        cout << static_cast<int>(sBuff) << " ";
        if (i % 2 == 0){
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
        }
        else{
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
        }
    }
    cout << endl; 
    
    // Broadcast the event from process 0 to all other processes
    MPI_Bcast(&total_nums, 1,MPI_INT, 0, MPI_COMM_WORLD);
    file.close();
}

/**
 * This contains logic of the nth process. 
 * LAST PROCESS IS printing the output  
 */
void nth_proces() {
    
    // last process does not use them  
    std::queue<uint8_t> q_up; 
    std::queue<uint8_t> q_down; 

    uint8_t rcBuff; 
    uint8_t sBuff; 

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Condition for the UPPER front to start the process 
    int condition  = pow(2, (rank-1));

    // how many numbers are there in total  
    int total_nums      = 0;
    // count how many numbers did this procces accept  
    int accepted_nums   = 0;

    MPI_Status status;
    // wait till you know total numbers 
    MPI_Bcast(&total_nums, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // met the input condition 
    for (int i = 0; i < condition; i++){
        MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
        q_up.push(rcBuff);accepted_nums++;
    }
    
    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
    q_down.push(rcBuff); accepted_nums++;
    
    int send = 0;
    int ups = pow (2, (rank-1));

    // remeber if you are doing UP UP or DOWN DOWN 
    char last = LAST_UP;
    int up_taken = 0;
    int can_take_up = false;
    int can_take_down = false; 
    int help = 0;
    bool oneEmpty = false;

    // Each iteration accept number and send number 
    for (int i = 0; true; i++){
        // last process is prinitng the output 
        if (accepted_nums == total_nums){
            if (q_up.empty() && q_down.empty()){
                break;
            }
        }
        // RECIEVE numbers -- 
        //  first proces UP DOWN UP DONW 
        //  second UP UP DOWN DOWN ...  
        else { 
            if (accepted_nums < total_nums){
                if (((accepted_nums / condition) % 2) == 0){ // take n up then n down 
                    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
                    accepted_nums++; q_up.push(rcBuff);
                }
                else {
                    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
                    accepted_nums++; q_down.push(rcBuff);
                }
            }
        }
        
        // First ups number takes from whenewer 
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
                cout << static_cast<int>(sBuff) << endl;
            }
            else{ // else send to next process 
                if (last == LAST_UP){
                    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
                }
                if (last == LAST_DOWN){
                    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
                }
            }
        }
        // second ups number you have to fill so it will ad up to taking same from
        //  both queue 
        else if (send < (ups * 2)){
            help = abs(up_taken);

            if (up_taken == 0){
                can_take_down = true; can_take_up = true; 
            }
            // if more are taken from upper front 
            else if (up_taken > 0){
                if (((ups*2) - send - help) > 0){
                    can_take_up = true; can_take_down = true;
                }
                else{
                    can_take_down = true; can_take_up = false; 
                }
            }
            else {
                if (((ups*2) - send - help) > 0){   
                    can_take_down = true; can_take_up = true; 
                }
                else{
                    can_take_down = false; can_take_up = true; 
                }
            }
            
            oneEmpty = false; 
            
            if ((can_take_down == true) && (can_take_up == true)){
                if (q_up.empty()){
                    sBuff = q_down.front(); q_down.pop();
                    up_taken--;
                }
                else if (q_down.empty()){
                    sBuff = q_up.front(); q_up.pop();
                    up_taken++;
                }
                else if (q_up.front() <= q_down.front()){
                    sBuff = q_up.front(); q_up.pop();
                    up_taken++;
                }
                else{
                    sBuff = q_down.front(); q_down.pop();
                    up_taken--;
                }
            }
            else if (can_take_down == true){
                if (q_down.empty()){
                    oneEmpty = true;
                }else{
                    sBuff = q_down.front(); q_down.pop();;
                    up_taken--;
                }
            }
            else{
                if (q_up.empty()){
                    oneEmpty = true;
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
                    cout << static_cast<int>(sBuff) << endl;
                }
                else{ // else send to next process 
                    if (last == LAST_UP){
                        MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
                    }
                    if (last == LAST_DOWN){
                        MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
                    }
                }
            }
        }
        // reset the counterss etc 
        else {
            if (last == LAST_UP){
                last = LAST_DOWN;
            }
            else{
                last = LAST_UP;
            }
            send = 0;
            up_taken = 0;
        } 
    }
}

int main(int argc, char *argv[]) {

    int rank, size;
    
    MPI_Init(&argc, &argv);

    // get the number of process 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

