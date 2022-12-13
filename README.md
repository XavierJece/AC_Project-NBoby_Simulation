# project1-ca

Project for the course of Advanced Computing from Instituto Politecnico de Braganca, this project applies parallel computing using pthreads to a serial code for N-Body simulation

1- Install SDL BIG
- Download .deb for Ubuntu from [https://sdl-bgi.sourceforge.io/](https://sdl-bgi.sourceforge.io/)
- Open file with Other Applications (Software Install)

2 - Install Valgrind
- Run command `sudo apt install valgrind`

3 - Install kcachegrind, graphviz
- Run command `sudo apt-get install -y kcachegrind && sudo apt-get install graphviz`

4 - Run serial code with kcachegrind
- To compile code run command `make`
- Run command `valgrind --tool=callgrind ./nbody-serial.exe`

5 - See hotspots
- Open program Kcachegrind
- Open file callgrind.out. ...

6 - Run test
 - Run command `cd ac-nbody-resources/`
 - Run command `./compare.sh -h` 

