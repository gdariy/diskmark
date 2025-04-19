# Diskmark 1.0

Diskmark is a disk benchmarking program with a simple terminal interface

Benchmarks both sequential and random read/write performance

## Installation

Use your packet manager to install gcc and ncurses-dev package

Debian 12:
```bash
sudo apt install libncurses5-dev
```

```bash
sudo apt install gcc
```
Then compile a program with -lncurses flag:
```bash
gcc main.c benchmark.c -o diskmark -lncurses
```
## Usage

You may test your flashdrive, SSD or HDD by entering directory path:

For example "/media/user/flashdrive/" or "/home/user/"

[WARNING] Do not enter a block device paths from /dev

Select the size of the testfile and the number of tests via interface
