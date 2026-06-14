#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <chrono>
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

    auto start_time = std::chrono::high_resolution_clock::now();
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

    
    if (rank == 0) {
        float totalArea = partialArea;
        float receivedPart;
        MPI_Status status;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&receivedPart, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            totalArea += receivedPart;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        cout << "Result (v2): " << totalArea << endl;
        cout << "Time: " << duration.count() << " microseconds" << endl;
    } else {
        MPI_Send(&partialArea, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
