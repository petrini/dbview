#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
  for(int i = 0; i < dbhdr->count; i++)
  {
    printf("Employee %d\n", i);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
  }
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employeesOut, char *addstring) {
  if(employeesOut == NULL)
  {
    printf("Invalid employeesOutPointer\n");
    return STATUS_ERROR;
  }

  if(dbhdr == NULL)
  {
    printf("Invalid header\n");
    return STATUS_ERROR;
  }

  if(addstring == NULL)
  {
    printf("Invalid addstring\n");
    return STATUS_ERROR;
  }

	char *name = strtok(addstring, ",");
  if(name == NULL)
  {
    printf("Invalid name\n");
    return STATUS_ERROR;
  }
	char *addr = strtok(NULL, ",");
  if(addr == NULL)
  {
    printf("Invalid address\n");
    return STATUS_ERROR;
  }
	char *hours = strtok(NULL, ",");
  if(hours == NULL)
  {
    printf("Invalid hours\n");
    return STATUS_ERROR;
  }
	
  dbhdr->count++;
  struct employee_t *employees = realloc(*employeesOut, sizeof(struct employee_t) * dbhdr->count);
  if(employees == NULL)
  {
    printf("Error realoccing database in memory\n");
    return STATUS_ERROR;
  }
	strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
	strncpy(employees[dbhdr->count-1].address, addr, sizeof(employees[dbhdr->count-1].address));

	employees[dbhdr->count-1].hours = atoi(hours);

  *employeesOut = employees;

  printf("Added Employee %d: %s\n", dbhdr->count - 1, name);

	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	if (fd < 0)
  {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

  if(employeesOut == NULL)
  {
    printf("Illegal employees pointer\n");
    return STATUS_ERROR;
  }

	int count = dbhdr->count;

	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == NULL) {
		printf("Malloc failed\n");
		return STATUS_ERROR;
	}

	read(fd, employees, count*sizeof(struct employee_t));

	int i = 0;
	for (; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;

	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	lseek(fd, 0, SEEK_SET);

	write(fd, dbhdr, sizeof(struct dbheader_t));

	for (int i = 0; i < realcount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed create a db header\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("Impromper header magic\n");
		free(header);
		return STATUS_ERROR;
	}

	if (header->version != 1) {
		printf("Impromper header version\n");
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database\n");
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;

  return STATUS_SUCCESS;
}

int create_db_header(struct dbheader_t **headerOut) {
  if(headerOut == NULL)
  {
    printf("Illegal header pointer\n");
    return STATUS_ERROR;
  }

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;

	return STATUS_SUCCESS;
}

