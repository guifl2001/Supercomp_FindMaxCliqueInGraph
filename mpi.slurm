#!/bin/bash
#SBATCH --nodes=2               # Número de nós
#SBATCH --ntasks-per-node=3     # Número de tarefas por nó (CPUs por nó)
#SBATCH --cpus-per-task=1       # Número de CPUs por tarefa
#SBATCH --partition=normal

mpirun --allow-run-as-root -np 2 ./mpi
