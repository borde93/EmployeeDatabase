#include "common.h"
#include "file.h"
#include "parse.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



void programInstruction(char* argv[]);
void freeMemory(struct dbheader_t **header, struct employee_t **employee);

int main(int argc, char* argv[]){

    int opt = 0;
    char* fileOptions = "na:f:";  
    char* filepath = NULL;
    char* addStr = NULL;
    bool newfile = false;
    bool addEmployee = false;
    
    int dbfd = -1;
    struct dbheader_t *header = NULL;
    struct employee_t *employees = NULL;


    while((opt = getopt(argc, argv, fileOptions)) != -1){


        switch(opt){
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case 'a':
                addEmployee = true;
                if(optarg == NULL){
                    printf("Missing new employee details\n");
                    programInstruction(argv);
                    return -1;
                }
                addStr = optarg;   
                break;
            case '?':
                printf("Invalid option: %c\n", opt);
                programInstruction(argv);
                return -1;
            default:
                return -1;
        }
    }

    if(filepath == NULL){
        printf("Filepath is a required argument.\n");
        programInstruction(argv);
        return 0;
    }

    if(newfile == true){
        dbfd = create_db_file(filepath);
        if(dbfd == STATUS_ERROR){
            printf("Unable to create database file\n");
            return -1;
        }

        if((create_db_header(&header)) == STATUS_ERROR){
            printf("Failed to create database header\n");
            return -1;
        }
    }

    else{
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR){
            printf("Failed to open db file.\n");
            return -1;
        }

        if(validate_db_header(dbfd, &header) == STATUS_ERROR){
             printf("Failed to validate the header.\n");
            freeMemory(&header, &employees);
            close(dbfd);
            return -1;
        };
    }

    if(read_employees(dbfd, header, &employees) == STATUS_ERROR){
        printf("Couldn't read employees. \n");
        close(dbfd);
        freeMemory(&header, &employees);
        return -1;
    }

    if(addEmployee){
        if(add_employee(dbfd, header, &employees, addStr) == STATUS_ERROR){
            printf("Couldn't add employees. \n");
            freeMemory(&header, &employees);
            close(dbfd);
            return -1; 
        }
    }


    if(output_file(dbfd, header, employees) == STATUS_ERROR){
        printf("Didn't write to the file. An error occurred\n");
        freeMemory(&header, &employees);
        close(dbfd);
        return -1;
    }
    printf("Newfile: %d\n", newfile);
    printf("Filepath: %s\n", filepath);

    return 0;
}



void programInstruction(char* argv[]){
    printf("Instruction for %s, use these flags:\n", argv[0]);
    printf("\t -f </file_path <--- Mandatory!!\n");
    printf("\t -n if you want to start a new file \n");
    printf("\t -a \"< First Name> <SurnameInitial.>, <Address>, <Weekly Hours>\" if you want to add a new employee \n");
    printf("\t Add employee example: %s -f ./employeesDB.db -a \"John S., 123 smith ln, 38\n\"", argv[0]);
}

void freeMemory(struct dbheader_t **header, struct employee_t **employees){
    free(*header);
    free(*employees);
    *header = NULL;
    *employees = NULL;
    return;
}


