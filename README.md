# How To Use
## Minimum Requirements
### Mac
    - g++ 8.2 or Apple LLVM version 10.0.1 (clang-1001.0.46.4)
    - IloCplex 12.8.0.0
    - Boost libraries
    - Python 2.6.6
    - Mac OS (e.g., Mojave)
    - Processor type x86_64
## How It Works
  - BCompose is to solve pre-decomposed MILP problems. This means that the user needs to export the master and subproblem(s) following the provided guidelines.
    - This gives the user a full flexibility in decomposing the problem and exploiting its special structures.
  - The 'models' folder contains a pre-compiled example of Stochastic Network Design Problems, i.e., r05-9-0.8-16. 

## How To Run
  - After exporting the proper formulations for the master (MP) and subproblem (SP) into the "models/" directory, run following command:
    ```
    python run_me.py
    ```
  - or:
    ```
     ./main  --model_dir=? --current_dir=?
    ```
## Tune
  - These options are currently unavailable.

## Notice
  - This is a precompiled version of BCompose;
  - The code will require 8 cores;
  - The pre-compiled example is for Stochastic Network Design Problems;

#TBC
