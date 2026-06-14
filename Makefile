CC = mpic++
CFLAGS = -O3 -Wall
OBJ = func.o

all: integrate mpi_integrate_v1 mpi_integrate_v2 mpi_integrate_v3

integrate: integrate.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

mpi_integrate_v1: mpi_integrate_v1.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

mpi_integrate_v2: mpi_integrate_v2.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

mpi_integrate_v3: mpi_integrate_v3.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o integrate mpi_integrate_v1 mpi_integrate_v2 mpi_integrate_v3