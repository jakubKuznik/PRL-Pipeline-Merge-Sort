# PRL-Pipeline-Merge-Sor
## Faculty: BUT FIT, Course: FLP t


## Compilation 
mpic++ --prefix /usr/local/share/OpenMPI -o pms pms.cpp

## Execution 
`mpirun --prefix /usr/local/share/OpenMPI  -np $proc pms`


## Input 
Hardcoded File `numbers` which contains 1B (0-255) numbers separed by space. 

## Output 
Program writes on stdout. On one line the numbers that were on input. and then sorted nums separated by newline. Errors are on stderr.   
```
54 53 70 25 
25
53
54
70
```

