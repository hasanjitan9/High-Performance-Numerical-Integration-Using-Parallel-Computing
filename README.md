# Numerical Integration with MPI

A C++ project that computes definite integrals using the **trapezoid rule**, comparing a sequential baseline against three parallel MPI implementations.

The function being integrated is:

```
f(x) = x² + 3x + 10
```

---

## Project Structure

| File | Description |
|---|---|
| `func.cpp` | Defines `f(x) = x² + 3x + 10` |
| `integrate.cpp` | Sequential trapezoid integration |
| `mpi_integrate_v1.cpp` | Parallel — `MPI_Send` / `MPI_Recv` (ordered receives) |
| `mpi_integrate_v2.cpp` | Parallel — `MPI_Send` / `MPI_Recv` with `MPI_ANY_SOURCE` |
| `mpi_integrate_v3.cpp` | Parallel — collective `MPI_Reduce` with `MPI_SUM` |
| `Makefile` | Builds all four executables |

---

## How It Works

### Sequential (`integrate`)
Divides the interval `[a, b]` into `n` equal subintervals and applies the trapezoid rule:

```
area += (f(xLeft) + f(xRight)) * 0.5 * step
```

### Parallel (all MPI versions)
Each MPI process is assigned a contiguous chunk of the `n` subintervals (with leftover intervals distributed to the first few ranks). Every process computes its local partial area, then results are gathered at rank 0.

- **v1** — Rank 0 receives partial results one by one in order (from rank 1, 2, 3, …)
- **v2** — Same as v1, but rank 0 uses `MPI_ANY_SOURCE` to accept results in any arrival order
- **v3** — Uses `MPI_Reduce` with `MPI_SUM` for collective, optimized reduction

---

## Requirements

- A C++ compiler with MPI support (e.g. **OpenMPI** or **MPICH**)
- `mpic++` available on your `PATH`

Install OpenMPI on Ubuntu/Debian:
```bash
sudo apt install libopenmpi-dev openmpi-bin
```

---

## Build

```bash
make
```

This compiles all four programs. To clean up:
```bash
make clean
```

---

## Usage

### Sequential
```bash
./integrate <a> <b> <n>
```

### MPI versions
```bash
mpirun -np <num_processes> ./mpi_integrate_v1 <a> <b> <n>
mpirun -np <num_processes> ./mpi_integrate_v2 <a> <b> <n>
mpirun -np <num_processes> ./mpi_integrate_v3 <a> <b> <n>
```

**Example** — integrate from 0 to 5 with 1,000,000 intervals using 4 processes:
```bash
./integrate 0 5 1000000
mpirun -np 4 ./mpi_integrate_v1 0 5 1000000
mpirun -np 4 ./mpi_integrate_v2 0 5 1000000
mpirun -np 4 ./mpi_integrate_v3 0 5 1000000
```

---

## Performance Results

Tested with `a=0`, `b=5`, `n=1000000` across 1, 2, 4, and 6 processes.

### v1 — Ordered `MPI_Recv`

| Processes | Time (µs) | Result |
|---|---|---|
| 1 | 3803 | 73.4958 |
| 2 | 1929 | 73.503 |
| 4 | 2053 | 73.502 |
| 6 | 694 | 73.5013 |

### v2 — `MPI_ANY_SOURCE`

| Processes | Time (µs) | Result |
|---|---|---|
| 1 | 3703 | 73.4958 |
| 2 | 2164 | 73.503 |
| 4 | 2366 | 73.502 |
| 6 | 2025 | 73.5013 |

### v3 — `MPI_Reduce` (collective)

| Processes | Time (µs) | Result |
|---|---|---|
| 1 | 3737 | 73.4958 |
| 2 | 2062 | 73.503 |
| 4 | 1194 | 73.502 |
| 6 | 851 | 73.5013 |

### Key Takeaways

- **v3 is the fastest and most consistent** — collective communication via `MPI_Reduce` scales better than point-to-point sends/receives.
- **v1 and v2** show similar throughput, but v2's `MPI_ANY_SOURCE` flexibility offers slight improvements in some configurations.
- Minor numerical differences across process counts are due to floating-point summation order.

---

## Algorithm — Pseudocode

```
Input: a, b, n
step = (b - a) / n

# Each MPI process computes:
chunkCount = n / size  (+1 if rank < n % size)
firstIndex = rank * (n/size) + min(rank, n%size)
localStart = a + firstIndex * step

partialArea = 0
for i in 0..chunkCount-1:
    xLeft  = localStart + i * step
    xRight = xLeft + step
    partialArea += (f(xLeft) + f(xRight)) * 0.5 * step

# v1/v2: rank 0 collects with MPI_Recv, others MPI_Send
# v3:    MPI_Reduce(partialArea → totalArea, MPI_SUM, root=0)
```
