#include <iostream>
#include <mpi.h>
#include <cstdlib>
float func(float x);


using namespace std;



int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 4) {
        if (rank == 0)
            cerr << "Usage: " << argv[0] << " <a> <b> <n>" << endl;
        MPI_Finalize();
        return 1;
    }


    
    float a = atof(argv[1]);
    float b = atof(argv[2]);
    int n = atoi(argv[3]);
    if (n <= 0) {
        if (rank == 0) cerr << "Usage: " << argv[0] << " <a> <b> <n>" << endl;
        MPI_Finalize();
        return 1;
    }

    float step = (b - a) / n;
    int baseChunk = n / size;
    int leftover = n % size;
    int chunkCount = baseChunk + (rank < leftover ? 1 : 0);
    int firstIndex = rank * baseChunk + (rank < leftover ? rank : leftover);
    float localStart = a + firstIndex * step;

    float partialArea = 0.0f;
    for (int i = 0; i < chunkCount; i++) {
        float xLeft = localStart + i * step;
        float xRight = xLeft + step;
        partialArea += (func(xLeft) + func(xRight)) * 0.5f * step;
    }

    float totalArea = 0.0f;
    MPI_Reduce(&partialArea, &totalArea, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Result (v3): " << totalArea << endl;
    }

    MPI_Finalize();
    return 0;
}
