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
    for (uint8_t i = 0; file.get(c); i++){
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

    int up_taken     = 0;
    int down_taken   = 0; 

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
    cout << "I am " << rank << " TOTAL NUMS ARE: "<< total_nums << endl;


    for (uint8_t i = 0; condition_met == false; i++){
        if (i % 2 == 0){
            cout << "I am " << rank << " AND I AM WAITING ON UP" << endl;
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
            q_up.push(rcBuff);
            accepted_nums++;
            cout << "I am " << rank << " And if got UP num" << endl;
        }
        else{
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
            q_down.push(rcBuff);
            accepted_nums++;
            cout << "I am " << rank << " And if got down num" << endl;
        }

        if (!q_down.empty() && (q_up.front() >= condition)){
            condition_met = true;
            cout << "I am " << rank << " And i'v MET A CONDITION" << endl;
        }
    }
    
    std::cout.flush();
    cout << "I am " << rank << " I v accepted:" << static_cast<int>(accepted_nums) << endl;

    int count = 0;
    for (uint8_t i = 0; true; i++){
        // last process is prinitng the output 
        
        // SEND 
        if (count < condition){
            cout << "I am " << rank << " i am CHOOSING SMALLER "  << "count: "
            << count << " condi: " << static_cast<int>(condition) << endl;
            // choose the bigger and remeber which front it was from 
            if (q_up.front() < q_down.front()){
                up_taken++;
                sBuff = q_up.front();
                q_up.pop();
            }
            else{
                down_taken++;
                sBuff = q_down.front();
                q_down.pop();
            }
            cout << "I am " << rank << " and i am sending " << static_cast<int>(sBuff) << " UP to: " << rank+1 << endl;

            // IF I AM THE LAST PROCESS     
            if (rank == size-1){
                cout << "I AM LAST, THIS IS RESULT: " << endl;
                cout << static_cast<int>(sBuff) << endl;
            }
            else{ // else send to next process 
                MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
            }

        }
        else if (count < (condition * 2)){
            cout << "I am " << rank << " i am FILLING REST "  << "count: "
            << count << " condi: " << static_cast<int>(condition) << endl;
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
            // IF I AM THE LAST PROCESS     
            if (rank == size-1){
                cout << "I AM LAST, THIS IS RESULT: " << endl;
                cout << static_cast<int>(sBuff) << endl;
            }
            else{ // else send to next process 
                cout << "I am " << rank << " and i am sending " << static_cast<int>(sBuff) << " DOWN to: " << rank+1 << endl;
                MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
            }
        }
        else {
            up_taken    = 0;
            down_taken  = 0;
            count = 0;
            continue;
        } 


        // RECIEVE 
        if (i % 2 == 0){
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, UP, MPI_COMM_WORLD, &status);
            accepted_nums++;
            cout << "I am " << rank << " AND I AM WAITING ON UP" << endl;
            q_up.push(rcBuff);
        }
        else {
            MPI_Recv(&rcBuff, 1, MPI_BYTE, rank-1, DOWN, MPI_COMM_WORLD, &status);
            accepted_nums++;
            cout << "I am " << rank << " And if got down num" << endl;
            q_down.push(rcBuff);
        }
        count++;

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

