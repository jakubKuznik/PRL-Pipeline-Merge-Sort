#include <mpi.h>
#include <iostream>

int main(int argc, char *argv[]) {\


    int rank, size;
    // MPI::Init(&argc, &argv);

    // MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_ARE_FATAL);
    // rank = MPI::COMM_WORLD.Get_rank();
    // size = MPI::COMM_WORLD.Get_size();

    std::cout << "I am" << rank << "of" << size << std::endl;

    // MPI::Finalize();


    return 0;
}