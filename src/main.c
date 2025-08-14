#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[])
{
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n  create new database file\n");
  printf("\t -f  (required) path to database file\n");
}

int main(int argc, char *argv[])
{
  bool new_file = false;
  char *filepath = NULL;
  int c = 0;
  int dbfd;

  while((c = getopt(argc, argv, "nf:")) != -1)
  {
    switch (c)
    {
      case 'n':
        new_file = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case '?':
        print_usage(argv);
        return -1;
      default:
        return -1;
    }
  }

  if(filepath == NULL)
  {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return -1;
  }

  if(new_file)
  {
    dbfd = create_db_file(filepath);
    if(dbfd == -1)
    {
      printf("Unable to create database\n");
      return -1;
    }
  }
  else
  {
    dbfd = open_db_file(filepath);
    if(dbfd == -1)
    {
      printf("Unable to open database\n");
      return -1;
    }
  }

  printf("new_file: %d\n", new_file);
  printf("filepath: %s\n", filepath);

  return 0;
}
