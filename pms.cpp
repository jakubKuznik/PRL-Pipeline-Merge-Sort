
#include <iostream>
#include <fstream>
#include <queue>
#include "mpi.h"

#define FILENUMS "numbers"

using namespace std;


/* Pipline Merge sort process scheme, where each process has 
input and output fronts 
            ------   ---    ...   ---
-------- P1        P2       ...    Pn --------- 
            ------   ---    ...   ---
*/

/**
 * @brief This is the first process that will read the 
 * random numbers from the file FILENUMS. The process 
 * will then put the value alternately to two output fronts 
 */
void first_proces(){
    
    std::queue<uint8_t>* first_queue = new std::queue<uint8_t>();

    std::cout << "I am FIRST " << std::endl;

    // Open the file     
    std::ifstream file;
    file.open(FILENUMS);

    if (!file.is_open()) {
        std::cerr << "Error opening file numbers" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char c;
    while (file.get(c)) {
        uint8_t val = static_cast<uint8_t>(c);
        cout << static_cast<int>(val) << endl;
    }
    
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

