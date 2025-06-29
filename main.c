#include <ncurses.h>
#include "benchmark.h"

#define MENU_ITEMS 5
#define MAX_MENU_LENGTH 6
#define MAX_PATH 100

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
}

int select_filesize(WINDOW *window)
{
  fill_window(window);
  int max_y, max_x;
  getmaxyx(window, max_y, max_x);
  int start_y = (max_y - MENU_ITEMS) / 2;
  int start_x = (max_x - MAX_MENU_LENGTH) / 2;

  keypad(window, TRUE);

  char filesize_menu[MENU_ITEMS][MAX_MENU_LENGTH] = {
      "16MB", "128MB", "512MB", "1GB", "Exit"};
  int input_key = 0;
  int highlight = 0;
  int done = 0;
  while (!done)
  {
    for (int i = 0; i < MENU_ITEMS; i++)
    {
      if (i == highlight)
      {
        wattron(window, A_REVERSE);
      }
      mvwprintw(window, start_y + i, start_x, "%s", filesize_menu[i]);
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
      if (highlight < MENU_ITEMS - 1)
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

int select_iterations(WINDOW *window)
{
  fill_window(window);
  int max_y, max_x;
  getmaxyx(window, max_y, max_x);
  int start_y = (max_y - MENU_ITEMS) / 2;
  int start_x = max_x / 2 - MAX_MENU_LENGTH / 2;
  char filesize_menu[MENU_ITEMS][MAX_MENU_LENGTH] = {
      "1", "3", "5", "10", "Exit"};
  int input_key = 0;
  int highlight = 0;
  int done = 0;

  keypad(window, TRUE);

  while (!done)
  {
    for (int i = 0; i < MENU_ITEMS; i++)
    {
      if (i == highlight)
      {
        wattron(window, A_REVERSE);
      }
      mvwprintw(window, start_y + i, start_x, "%s", filesize_menu[i]);
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
      if (highlight < MENU_ITEMS - 1)
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

char *select_path(WINDOW *window)
{
  fill_window(window);
  const char *msg = "Enter path to a disk(empty is valid):";
  char *path = malloc(MAX_PATH);
  int max_y, max_x;
  getmaxyx(window, max_y, max_x);

  echo();
  curs_set(1);
  keypad(window, FALSE);

  print_msg(window, msg);

  wmove(window, max_y / 2 + 1, (max_x - strlen(msg)) / 2);
  wgetnstr(window, path, MAX_PATH);

  noecho();
  curs_set(0);
  keypad(window, TRUE);

  wclear(window);
  wrefresh(window);
  return path;
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

  char *path = NULL;
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
    wgetch(win);
    goto cleanup;
  }

  path = select_path(win);
  strcat(path, "test.dat");

  int filesize_choice = select_filesize(win);
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

  int iterations_choice = select_iterations(win);
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
      wgetch(win);
      goto cleanup;
    }
    total_sw += result_sw;

    double result_sr = seqread_mark(filesize, path);
    if (result_sr == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      wgetch(win);
      goto cleanup;
    }
    total_sr += result_sr;

    double result_rr = rndread_mark(filesize, path);
    if (result_rr == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      wgetch(win);
      goto cleanup;
    }
    total_rr += result_rr;

    double result_rw = rndwrite_mark(filesize, path);
    if (result_rw == -1)
    {
      print_msg(win, "Error! Probably the wrong path");
      wgetch(win);
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
  free(path);
  endwin();
  return 0;

cleanup:
  if (win)
    delwin(win);
  free(path);
  endwin();
  return 1;
}
