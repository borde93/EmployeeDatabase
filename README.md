# EmployeeDatabase
An employee database written in C. Written to improve my C programming skills.

## Functionality

Run the program from cmd line ./dbview <-option1> <-option2> <Option argument>

Available options are:

|Flag | Option           | Example           |Description|
|:---:|------------------|-------------------|-----------|
|--f  |\<filepath\>      |-f database.db     |Filepath to existing or new database|
|--n  |                  |-n                 |Create a new database file (file must not already exist)
|--a  |\<"name,Address,hrs"\>|./mydb.db -a "John S.,123 Marine rd,40"| Add an employee, needs 3 values comma separated:<br> \<name\>,\<Address\>,\<Working hours\>|
|-l   |                 | -l                 | Lists all the employees|


The program reads the the inputs from the cmd line, checks the file header for the magic number (file identifier), ensures that the file size if consistent with the database header information for extra security, can add and check employees information updating the file header as needed.
