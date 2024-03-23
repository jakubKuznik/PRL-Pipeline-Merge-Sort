// Faculty: BUT FIT 
// Course: PRL 
// Project Name: Pipeline Merge Sort  
// Name: Jakub Kuznik
// Login: xkuzni04
// Year: 2024

/* Pipline Merge sort process scheme, where each process has 
input and output fronts 
         ------   -----  ...  ------
---- P1        P2        ...      Pn -----
         ------   -----  ...  ------
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
    // input queue 
    std::queue<uint8_t>* q1 = new std::queue<uint8_t>();
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
    
    // parse the input numbers into the queue 
    char c;
    while (file.get(c)) {
        uint8_t val = static_cast<uint8_t>(c);
        q1->push(val);
        cout << static_cast<int>(val) << endl;
    }

    file.close();

    // p1 brain while there is number send it to the p2 
    while (){
        // tag 1 q1 
        // tag 2 q2 
        MPI_Send(^)
    }

    // after everything sent speciall signal to shut down everyone

    /**
    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), UP, MPI_COMM_WORLD);
    MPI_Send(&sBuff, 1, MPI_BYTE, (rank+1), DOWN, MPI_COMM_WORLD);
    
    MPI_Status status;
    MPI_Recv(&rcBuff, 1, MPI_BYTE, rank, UP, MPI_COMM_WORLD, &status);
    */

    delete q1;
    return;    
}

void nth_proces() {
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    cout << "I am " << rank << endl;

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

