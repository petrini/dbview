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
        return STATUS_ERROR;
      default:
        return STATUS_ERROR;
    }
  }

  if(filepath == NULL)
  {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return STATUS_ERROR;
  }

  struct dbheader_t *header = NULL;
  if(new_file)
  {
    dbfd = create_db_file(filepath);
    if(dbfd == STATUS_ERROR)
    {
      printf("Unable to create database\n");
      return STATUS_ERROR;
    }

    int dbheader_create = create_db_header(&header);
    if(dbheader_create == STATUS_ERROR)
    {
      printf("Unable to create database header\n");
      close_db_file(dbfd);
      return STATUS_ERROR;
    }
  }
  else
  {
    dbfd = open_db_file(filepath);
    if(dbfd == STATUS_ERROR)
    {
      printf("Unable to open database\n");
      return STATUS_ERROR;
    }

    int dbheader_validate = validate_db_header(dbfd, &header);
    if(dbheader_validate == STATUS_ERROR)
    {
      printf("Unable to validate database header\n");
      close_db_file(dbfd);
      return STATUS_ERROR;
    }
  }

  struct employee_t *employee = NULL;
  int output = output_file(dbfd, header, employee);

  free(header);

  if(output == STATUS_ERROR)
  {
    printf("Could not output file\n");
    close_db_file(dbfd);
    return STATUS_ERROR;
  }

  return STATUS_SUCCESS;
}
