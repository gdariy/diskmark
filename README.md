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

![caption](https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExejU3MHR4amZnZ2F1ZHJ1M3Ria3ZhanR4eTJlZjJueGx0OWUxMWRqYiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/GToBTKIqq46fZYyfWs/giphy.gif)

You may test your flashdrive, SSD or HDD by entering directory path:

For example "/media/user/flashdrive/" or "/home/user/"

[WARNING] Do not enter a block device paths from /dev

Select the size of the testfile and the number of tests via interface
