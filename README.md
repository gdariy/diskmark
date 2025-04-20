# Diskmark 1.0

Diskmark is a disk benchmarking tool with a simple terminal interface

Benchmarks both sequential and random read/write performance

## Prerequisites

* ncurses

## Installation

1. Clone the repository:
```bash
git clone https://github.com/myx0m00000p/diskmark.git
cd diskmark
```
2. Compile:
```bash
gcc benchmark.c main.c -o diskmark -lncurses
```
3. Run:
```bash
./diskmark
```

## Usage

![caption](https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExaDExeWtoNGE3bmtzbmV4ZXZ2ODN5bWp1Z3h5ZXM5YnV3Y282ZXd2NyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/o8Vt9b4aMtiP9zhtd1/giphy.gif)

You can test your flashdrive, SSD or HDD by entering directory path:

For example "/media/user/flashdrive/" or "/home/user/"

[WARNING] Do not enter paths to the block devices from /dev/

Select the size of the testfile and the number of tests via interface
