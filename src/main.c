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
  printf("\t -a  add a new employee <name,address,hours>\n");
  printf("\t -l  list all employees\n");
  printf("\t -h  change first occurence of employee's hours <name,hours>\n");
  printf("\t -r  remove first occurence of employee <name>\n");
}

int main(int argc, char *argv[])
{
  bool new_file = false;
  bool list = false;
  char *filepath = NULL;
  char *add_string = NULL;
  char *hours_string = NULL;
  char *remove_string = NULL;
  int c = 0;
  int dbfd;

  while((c = getopt(argc, argv, "nf:a:lh:r:")) != -1)
  {
    switch (c)
    {
      case 'n':
        new_file = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case 'a':
        add_string = optarg;
        break;
      case 'l':
        list = true;
        break;
      case 'h':
        hours_string = optarg;
        break;
      case 'r':
        remove_string = optarg;
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

  struct employee_t *employees = NULL;
  if(read_employees(dbfd, header, &employees) == STATUS_ERROR)
  {
    printf("Error reading employees\n");
    close_db_file(dbfd);
    free(header);
    return STATUS_ERROR;
  }

  if(add_string != NULL)
  {
    if(add_employee(header, &employees, add_string) == STATUS_ERROR)
    {
      printf("Error adding employee: %s\n", add_string);
      return STATUS_ERROR;
    }
  }

  if(hours_string != NULL)
  {
    if(change_employee_hours(header, employees, hours_string) == STATUS_ERROR)
    {
      printf("Error changing hours from employee: %s\n", hours_string);
      return STATUS_ERROR;
    }
  }

  if(remove_string != NULL)
  {
    if(remove_employee(header, &employees, remove_string) == STATUS_ERROR)
    {
      printf("Error removing employee: %s\n", remove_string);
      return STATUS_ERROR;
    }
  }

  if(list)
  {
    list_employees(header, employees);
  }

  if(output_file(dbfd, header, employees) == STATUS_ERROR)
  {
    printf("Could not output file\n");
    return STATUS_ERROR;
  }

  return STATUS_SUCCESS;
}
