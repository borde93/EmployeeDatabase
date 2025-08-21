#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"



int create_db_header(int fd, struct dbheader_t **headerOut){
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if(header == NULL){
        printf("Calloc failed to create db header\n");
        return STATUS_ERROR;
    }

    header->version = HEADER_VERSION;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);
    
    *headerOut = header;
    return STATUS_GOOD;
}

int validate_db_header(int fd, struct dbheader_t **headerOut){

    if(fd < 0){
        printf("Got a bad FD from the user while validating header\n");
        return STATUS_ERROR;
    }
    
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL){
        printf("Calloc failed to create a db header\n");
        return STATUS_ERROR;
    }
    
    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){
        perror("read");
        free(header);
        return STATUS_ERROR;
    }
    
    //Convert to the host endian.
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);
    header->magic = ntohl(header->magic);

    if(header->magic != HEADER_MAGIC){
        printf("Improper identifier!\n");
        free(header);
        return STATUS_ERROR;
    }

    if(header->version != HEADER_VERSION){
        printf("Wrong header version.\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    if(fstat(fd, &dbstat) == -1){
        printf("fstat error\n");
        perror("fstat");
        free(header);
        return STATUS_ERROR;
    }

    if(dbstat.st_size != header->filesize){
        printf("The file is corrupted! \n");
        free(header);
        return(STATUS_ERROR);
    }
    *headerOut = header;

   return STATUS_GOOD; 
};



int output_file(int fd, struct dbheader_t *dbhdr){
    
    if(fd < 0){
        printf("Got a bad FD from the user while ouputting to file\n");
        return STATUS_ERROR;
    }
    
    int nEmployees = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->count = htons(dbhdr->count + (sizeof(struct employee_t) * nEmployees));
    dbhdr->filesize = htonl(dbhdr->filesize);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET); //setting cursor to the beginning of the file to write the header

    if((write(fd, dbhdr, sizeof(struct dbheader_t))) == -1){
       printf("Cannot write to file. \n");
        perror("write");
        return STATUS_ERROR;
    }
    
    return STATUS_GOOD;
}



int read_employees(int fd, struct dbheader_t **headerOut, struct employee_t **employeesOut);


