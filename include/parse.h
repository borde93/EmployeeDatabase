#ifndef PARSE_H
#define PARSE_H

// Header identifier is used to validate the filetype ensuring correct parsing.
// the value has been chosen by me and only me.
#define HEADER_MAGIC 0x7269636F
#define HEADER_VERSION 0x1

struct dbheader_t{
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct employee_t{
    char name[256];
    char address[256];
    unsigned int hours;
};

int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int read_employees(int fd, struct dbheader_t **headerOut, struct employee_t **employeesOut);
int output_file(int fd, struct dbheader_t *dbhdr);
#endif
