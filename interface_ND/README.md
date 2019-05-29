# How To Export the Pre-Decomposed Problems
  - This document explains how to decompose a problem such that it can be solved by BCompose.
## Decomposition Scheme
  - Please refer to ***The Benders Dual Decomposition Method***
## Important Guidelines  
  - To ensure BCompose works properly, the user needs to follow few specific conventions in naming the variables and constraints:
    - Please refer to the WARNINGS highlighted in the example interface.
  - Do not forget to impose integrality on the copies master variables in the subproblems' formulation.
## How to Use BCompose for Continuous Problems
  - The user needs to impose the integrality on the copied master variables but limit the algorithm to the first phase.
## How to Run
  - The user is required to properly update the path to the following directories as set in the python script:
    - path_to_BCompose
    - models_dir
    - opt_model_dir
