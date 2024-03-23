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

#define FILENUMS "numbers"
#define UP 1
#define DOWN 2 

using namespace std;

/**
 * This is the first process that will read the 
 * random numbers from the file FILENUMS. The process 
 * will then put the value alternately to two output fronts 
 */
void first_proces(){
    
    // send Buffer that will be used to send number to the p2 
    uint8_t sBuff;
    
    // get my rank 
    int rank;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::cout << "I am FIRST " << std::endl;

    // Open the file     
    std::ifstream file;
    file.open(FILENUMS);

    if (!file.is_open()) {
        std::cerr << "Error opening file numbers" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // parse the input numbers and send then to the second process 
    char c;
    for (uint8_t i = 0; file.get(c); i++){
        sBuff = static_cast<uint8_t>(c);
        if (i % 2 == 0){
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
        }
        else{
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
        }
    }

    file.close();
    
    // after everything sent speciall signal to shut down everyone


    return;    
}

void nth_proces() {
    
    std::queue<uint8_t> q_up; 
    std::queue<uint8_t> q_down; 

    uint8_t rcBuff; 
    uint8_t sBuff; 

    int up_taken     = 0;
    int down_taken   = 0; 

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Condition for the UPPER front to start the process 
    uint8_t condition  = 2^(rank-1);
    bool condition_met = false;
    
    MPI_Status status;

    cout << "I am " << rank << endl;

    for (uint8_t i = 0; condition_met; i++){
        if (i % 2 == 0){
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank, UP, MPI_COMM_WORLD, &status);
            q_up.push(rcBuff);
        }
        else{
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank, DOWN, MPI_COMM_WORLD, &status);
            q_down.push(rcBuff);
        }

        if (!q_down.empty() && (q_up.front() == condition)){
            condition_met = true; 
        }
    }

    int count = 0;
    for (uint8_t i = 0; true; i++){
        // SEND 
        if (count < condition){
            // choose the bigger and remeber which front it was from 
            if (q_up.front() > q_down.front()){
                up_taken++;
                sBuff = q_up.front();
                q_up.pop();
            }
            else{
                down_taken++;
                sBuff = q_down.front();
                q_down.pop();
            }
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);

        }
        else if (count < (condition * 2)){
            if ((up_taken == down_taken) || (up_taken < down_taken)){
                sBuff = q_up.front();
                q_up.pop();
                up_taken++;
            }
            else {
                sBuff = q_down.front();
                q_down.pop();
                down_taken++;
            }
            
            MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
        }
        else {
            up_taken    = 0;
            down_taken  = 0;
            count = 0;
            continue;
        } 

        // RECIEVE 
        if (i % 2 == 0){
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank, UP, MPI_COMM_WORLD, &status);
            q_up.push(rcBuff);
        }
        else {
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank, DOWN, MPI_COMM_WORLD, &status);
            q_down.push(rcBuff);
        }
        count++;

    }

    
}

void last_proces() {
    cout << "I am LAST" << endl;

}


int main(int argc, char *argv[]) {

    int rank, size;
    
    MPI_Init(&argc, &argv);

    // get the number of process 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // get current procces id 
    // MPI_COMM_WORLD - prediefined constant to match all the processes  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 1){
        first_proces();
    }
    else if (rank == (size-1)){
        last_proces();
    }
    else {
        nth_proces();
    }

    cout << "I am " << rank << " of " << size << endl;

    MPI_Finalize();

    return 0;
}

