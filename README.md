# Burrows-Wheeler Transform

## Build

Change directory to the `command` folder 

```terminal
cd command
```

and run the following command to build the program.

```terminal
g++ -std=c++17 -o main main.cpp
```

## Run

### 1. Run for command 1

Convert the original string to BWT or vice versa.

#### a. Original string s -> BWT(s) -> output.txt

`input.txt` contains the original string s.

```text
hello$
world$
panabanana$
```

```terminal
./main -c input.txt output.txt "--bwt"
```

#### b. BWT(s) -> Original string s -> output.txt

`input.txt` contains the BWT(s).

```text
oh$ell
dlrwo$
annnpbaaaa$
```

```terminal
./main -c input.txt output.txt
```

### 2. Run for command 2

Find the positions of the patterns in the paragraph.

```terminal
./main -p paragraph.txt patterns.txt output.txt
```

## Notes

- This command line program is built using C++17 and g++.
- The program of command 1 reads the input from the file `input.txt` and writes the output to the file `output.txt`.
- The program of command 2 reads the input from the files `paragraph.txt` and `patterns.txt`, and writes the output to the file `output.txt`.
- The program of command 1 can be run with or without the `--bwt` option to convert the original string to BWT or vice versa.
- The command line program is built and tested on MacOS. It also works on Linux. If you use Windows, you can run the program using the `main.exe` file by replacing `./main` with `main.exe` in the commands above. Example:

```terminal
main.exe -c input.txt output.txt "--bwt"
```