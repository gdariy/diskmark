#include "benchmark.h"
#include "findmounts.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void fill_window(WINDOW *window)
{
  box(window, 0, 0);
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  const char *title = "DISKMARK 1.0";
  int title_x = (max_x - 4 - strlen(title)) / 2;
  mvwprintw(window, 0, title_x, "%s", title);
  mvwhline(window, 1, 1, ACS_HLINE, max_x - 6);
  wrefresh(window);
}

void print_msg(WINDOW *window, const char *msg)
{
  wclear(window);
  fill_window(window);
  int max_y, max_x;
  getmaxyx(window, max_y, max_x);

  mvwprintw(window, max_y / 2, (max_x - strlen(msg)) / 2, "%s", msg);
  wrefresh(window);
  wgetch(window);
  wclear(window);
}

int display_menu(WINDOW *window, char **items, int len_items)
{
  fill_window(window);
  int max_y, max_x;
  getmaxyx(window, max_y, max_x);
  int x, y = (max_y - len_items) / 2;

  keypad(window, TRUE);

  int input_key = 0;
  int highlight = 0;
  int done = 0;
  while (!done)
  {
    for (int i = 0; i < len_items; i++)
    {
      if (i == highlight)
      {
        wattron(window, A_REVERSE);
      }
      x = (max_x - strlen(items[i])) / 2;
      mvwprintw(window, y + i, x, "%s", items[i]);
      wattroff(window, A_REVERSE);
    }

    input_key = wgetch(window);

    switch (input_key)
    {
    case KEY_UP:
      if (highlight > 0)
      {
        highlight--;
      }
      break;
    case KEY_DOWN:
      if (highlight < len_items - 1)
      {
        highlight++;
      }
      break;
    case 10:
    case KEY_ENTER:
      done = 1;
      break;
    default:
      break;
    }
  }

  keypad(window, FALSE);
  wclear(window);
  wrefresh(window);

  return highlight;
}

int main()
{

  if (initscr() == NULL)
  {
    fprintf(stderr, "Error initializing ncurses\n");
    exit(EXIT_FAILURE);
  }

  cbreak();
  noecho();
  curs_set(0);

  char **volumes_str = NULL;
  char path[MAX_PATH_LEN];
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  WINDOW *win = newwin(max_y - 4, max_x - 4, 2, 2);
  if (win == NULL)
  {
    fprintf(stderr, "Error creating window\n");
    goto cleanup;
  }
  getmaxyx(win, max_y, max_x);

  if (!geteuid())
  {
    print_msg(win, "Running as root is not allowed!");
    goto cleanup;
  }

  MountInfo *mounts;
  int count = get_mounts(&mounts);
  if (!count)
  {
    print_msg(win, "Devices not found!");
    goto cleanup;
  }
  volumes_str = malloc(count * sizeof(char *));

  print_msg(win, "Please select disk device for testing:");
  for (int i = 0; i < count; i++)
  {
    volumes_str[i] = malloc(MAX_LINE_LENGTH * sizeof(char));
    sprintf(volumes_str[i], "%s\t%s\t%llu GiB/%llu GiB", mounts[i].device, mounts[i].mountPoint,
            (mounts[i].totalBytes - mounts[i].availBytes) / 1024 / 1024 / 1024, mounts[i].totalBytes / 1024 / 1024 / 1024);
  }
  int path_choice = display_menu(win, volumes_str, count);
  strcpy(path, mounts[path_choice].mountPoint);
  strcat(path, "/test.dat");
  free(mounts);

  char *filesize_menu[] = {
      "16MB", "128MB", "512MB", "1GB", "Exit"};
  int filesize_choice = display_menu(win, filesize_menu, 5);
  long filesize;
  switch (filesize_choice)
  {
  case 0:
    filesize = 16 * 1024 * 1024;
    break;
  case 1:
    filesize = 128 * 1024 * 1024;
    break;
  case 2:
    filesize = 512 * 1024 * 1024;
    break;
  case 3:
    filesize = 1024 * 1024 * 1024;
    break;
  case 4:
  default:
    goto cleanup;
  }

  char *iterations_menu[] = {
      "1", "3", "5", "10", "Exit"};
  int iterations_choice = display_menu(win, iterations_menu, 5);
  int iterations;
  switch (iterations_choice)
  {
  case 0:
    iterations = 1;
    break;
  case 1:
    iterations = 3;
    break;
  case 2:
    iterations = 5;
    break;
  case 3:
    iterations = 10;
    break;
  case 4:
  default:
    goto cleanup;
  }

  fill_window(win);

  mvwprintw(win, (max_y - 4) / 2, (max_x - 12) / 2, "Please wait...");

  srand(time(NULL));

  double total_sw = 0.0;
  double total_sr = 0.0;
  double total_rw = 0.0;
  double total_rr = 0.0;

  for (int i = 0; i < iterations; i++)
  {
    mvwprintw(win, (max_y - 4) / 2 + 1, (max_x - 12) / 2, "Test %d/%d", i + 1, iterations);
    wrefresh(win);

    double result_sw = seqwrite_mark(filesize, path);
    if (result_sw == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      goto cleanup;
    }
    total_sw += result_sw;

    double result_sr = seqread_mark(filesize, path);
    if (result_sr == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      goto cleanup;
    }
    total_sr += result_sr;

    double result_rr = rndread_mark(filesize, path);
    if (result_rr == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      goto cleanup;
    }
    total_rr += result_rr;

    double result_rw = rndwrite_mark(filesize, path);
    if (result_rw == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      goto cleanup;
    }
    total_rw += result_rw;
  }
  wclear(win);
  fill_window(win);

  mvwprintw(win, (max_y - 6) / 2, (max_x - 24) / 2, "SEQREAD 1MB: %0.2fMBps", total_sr / iterations);
  mvwprintw(win, (max_y - 6) / 2 + 2, (max_x - 24) / 2, "SEQWRITE 1MB: %0.2fMBps", total_sw / iterations);
  mvwprintw(win, (max_y - 6) / 2 + 4, (max_x - 24) / 2, "RNDREAD 4KB: %0.2fMBps", total_rr / iterations);
  mvwprintw(win, (max_y - 6) / 2 + 6, (max_x - 24) / 2, "RNDWRITE 4KB: %0.2fMBps", total_rw / iterations);

  wrefresh(win);
  wgetch(win);

  if (win)
    delwin(win);
  free(volumes_str);
  endwin();
  return 0;

cleanup:
  if (win)
    delwin(win);
  free(volumes_str);
  endwin();
  return 1;
}
