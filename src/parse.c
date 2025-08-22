#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees){
    
    if(dbhdr == NULL){
        printf("dbhdr is a null pointer.\n");
        return;
    }
    if(employees == NULL){
        printf("employees is a NULL pointer\n");
        return;
    }

    int i = 0;
    printf("\n\n");
    printf("Total number of employees: %d\n\n", dbhdr->count);
    for(i = 0; i < dbhdr->count; i++){
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n", employees[i].hours);
    }
}

int create_db_header(struct dbheader_t **headerOut){
    if(headerOut == NULL){
        printf("NULL header is not a valid input for create_db_header!\n");
        return STATUS_ERROR;
    }
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
    if(headerOut == NULL){
        printf("NULL header is not a valid input for validate_db_header!\n");
        return STATUS_ERROR;
    }
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
}



int output_file(int fd, struct dbheader_t *dbhdr, struct  employee_t *employees){
    if(dbhdr == NULL){
        printf("NULL header reveived in output file!\n");
        return STATUS_ERROR;
    }

    if(fd < 0){
        printf("Got a bad FD from the user while ouputting to file\n");
        return STATUS_ERROR;
    }
    int nEmployees = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * nEmployees);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET); //setting cursor to the beginning of the file to write the header

    if((write(fd, dbhdr, sizeof(struct dbheader_t))) == -1){
       printf("Cannot write header to file. \n");
        perror("write");
        return STATUS_ERROR;
    }
    printf("Header written succesfully\n");
    int i = 0;
    for(i = 0; i < nEmployees; i++){
      employees[i].hours = htonl(employees[i].hours);
    if((write(fd, &employees[i], sizeof(struct employee_t))) == -1){
            printf("Cannot write employee to file. \n");
            perror("write");
            return STATUS_ERROR;
        }
    }

        return STATUS_GOOD;
}



int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut){
    if(dbhdr == NULL){
        return STATUS_ERROR;
    }
     if(fd < 0){
        printf("Got a bad FD from the user while reading employees\n");
        return STATUS_ERROR;
    }

   int count = dbhdr->count;

   struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if(employees == NULL){
        printf("Calloc failed in read_employees\n");
        return STATUS_ERROR;
    }
    
    if((read(fd, employees, count * sizeof(struct employee_t))) == -1){
        printf("Failed to read employees on file. \n");
        perror("read");
        free(employees);
        employees = NULL;
        return STATUS_ERROR;
    }
    
    int i = 0;
    for (; i < count; i++){
            employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_GOOD;

}


int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char* addstring){
    if(employees == NULL){
        printf("Empty *employee[] in add_employee\n");
        return STATUS_ERROR;
    }
    if(dbhdr == NULL) return STATUS_ERROR;
    if(addstring == NULL) return STATUS_ERROR;
    int newCount = 0;
    struct employee_t *newEmployees = NULL;

    char *name = strtok(addstring, ",");
    char *address = strtok(NULL, ",");
    if(address == NULL){
        printf("Missing address!!\n");
            return STATUS_ERROR;
    }
    if(*address == ' ') address++;
    
    char *hours = strtok(NULL, ",");
    if(hours == NULL){
        printf("Missing weekly hours data!!\n");
        return STATUS_ERROR;
    }
    if(*hours == ' ') hours++;
    
    
    newCount = dbhdr->count + 1;
    newEmployees = realloc(*employees, sizeof(struct employee_t) * newCount);
    if(newEmployees == NULL){
        printf("Failed to realloc adding new employee. Try again!!\n");
        return STATUS_ERROR;
    }
    
    
    strncpy(newEmployees[newCount - 1].name ,name, sizeof(newEmployees[newCount - 1].name));
    strncpy(newEmployees[newCount - 1].address , address, sizeof(newEmployees[newCount - 1].address));
    newEmployees[newCount - 1].hours = atoi(hours);
    printf("Name: %s\nAddress: %s\nHours: %s\n", name, address, hours);
    
    *employees = newEmployees;
    dbhdr->count = newCount;


    return STATUS_GOOD;
}
