#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //C POSIX library which provides the API for changing directory, chdir() function. Also, needed for fork() function.
#include <dirent.h> //C POSIX library for directory traversal. Only works on Linux systems.
#include <sys/types.h> //for fork() function.

/**
-----------------------------------------------------------------------------------------------------------------
* Description
* niluSH (nilu shell) is a shell written in C using POSIX C libraries to mock useful commands from the Bash programming language.
* With an extension of new commands which do not exist in Bash scripting.
* No built-in shells are used. Only POSIX libraries for directory manipulation.
* Author: Nilushanth Thiruchelvam.
-----------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief helper function which gets the length of the character pointer.
 * @return length of the character pointer.
 */
int getLength(char * str){
    char * temp = NULL;

    int size = 0;
    for(temp = str; *temp != '\0'; temp++){
        size++;
    }
    return size;
}

/**
 * @brief helper function which concatenates str1 and str2 and returns a new char * str.
 * @param str1 
 * @param str2 
 * @return str concatenation of str1 and str2.
 */
char * concatenate(char * str1, char * str2){
    int lengthStr1 = getLength(str1);
    int lengthStr2 = getLength(str2);
    int concatLength = lengthStr1 + lengthStr2;

    char * concatStr = malloc(sizeof(char *) * concatLength);
    int i; //i variable used for accessing concatStr.
    int j = 0; //j variable used for accessing only str1.
    int k = 0; //k variable used for accessing only str2.
    for(i = 0; i < concatLength; i++){
        if(str1[j] != '\0'){
            concatStr[i] = str1[j];
            j++;
            continue; //immediately skip to the next iteration.
        }
        else if(str2[k] != '\0'){
            concatStr[i] = str2[k];
            k++;
            continue; //immediately skip to the next iteration.
        }
    }
    return concatStr;
}

/**
 * @brief echo function implementation in C. The echo command from the Bash programming language outputs content in the shell.
 * @param parameter which are parameter[0], parameter[1]...parameter[n] are parameter 'tokens'.
 */
void echo(char ** parameter){
    char **temp = NULL;
    char * concat = NULL;
    concat = *parameter; //concat points to the first string in parameter variable.

    //niluSH command "echo -ns" which prints with no space.
    if(concat[0] == '-' && concat[1] == 'n' && concat[2] == 's'){
        //iterating from *(parameter+2) because concat points to *(parameter + 1 ) is -ns option.
        concat = *(parameter+1);
        for(temp = parameter + 2; *temp != NULL; temp++){
            concat = concatenate(concat, *temp);
        }
    }
    else{
    //otherwise print with spaces in between.
        //iterating from *(parameter+1) because concat points to *(parameter).
        for(temp = parameter + 1; *temp != NULL; temp++){
            *temp = concatenate(" ", *temp);
            concat = concatenate(concat, *temp);
        }
    }

    printf("%s\n", concat);
}

/**
 * @brief touch function implementation in C. The touch command from the Bash programming language creates a new file.
 * @param parameter which are parameter[0], parameter[1]...parameter[n] are parameter 'tokens'.
 */
void touch(char ** parameter){
    FILE * filePointer = NULL; //instantiating a file pointer to null.

    /*
     * the second argument is the mode which is used. In this case that mode is r+ (for reading and writing).
     * mode r+ is for reading and writing.
     * mode r is only for reading.
     * mode w is for writing and previous contents in the file get erased.
     * mode a is for appending.
     */
    filePointer = fopen(parameter[0], "r+");
    fclose(filePointer);

    if(filePointer == NULL){
        //this means the file is not created yet. So creating a new file with mode w.
        filePointer = fopen(parameter[0], "w");
        fclose(filePointer);

        if(filePointer == NULL){
            printf("ERROR: cannot write to or open the file %s\n", parameter[0]);
        }

    }
}

/**
 * @brief Helper ls function implementation in C. ls command from the Bash programming language lists all the files and directories in the current working directory.
 */
void ls(){
    
    DIR *dir; //directory pointer to the directory stream.
    struct dirent * sd; //pointer to the structure dirent.

    dir = opendir("."); //the directory pointer is initialized to the current working directory.

    if(dir == NULL){
        printf("ERROR: the working directory is not valid\n");
    }
    else{
        //printing the directories and files in the working directory.
        while( (sd = (readdir(dir))) != NULL ){
            
            //only printing non-hidden directories/files since the ls command does not print hidden files starting with '.'
            if( (sd->d_name)[0] != '.'){
                printf("%s\n", sd->d_name);
            }
        }
    }

    closedir(dir); //close the opened directory.
}

/**
 * @brief ls overloaded function implementation in C. Parameter is a directory path. ls command from the Bash programming language lists all the files and directories in the current working directory.
 * @param parameter which are parameter[0], parameter[1]...parameter[n] are parameter 'tokens'.
 */
void lsOverloaded(char ** parameter){
    
    //checking if directory path given is absolute or relative which will contain the "/" (forward-slash) symbol.
    char *tempPath;

    DIR *dir; //the directory stream pointer.
    struct dirent * sd; //pointer to the structure dirent.

    if(parameter[0] == NULL){
        ls(); //list the working directory since no directory path was given.
    }
    else if(parameter[0][0] != '-'){
        //change directory to the given directory path. No option given since options start with '-'.
        dir = opendir(parameter[0]);

        if(dir == NULL){
            printf("ERROR: the %s directory is not valid\n", parameter[0]);
        }
        else{
            //printing the directories and files in the working directory. The readdir function returns pointer to next directory entry.
            while( (sd=(readdir(dir))) != NULL ){

                //if the directory entry starts with a '.' then it is a hidden file which ls does not print.
                if( (sd->d_name)[0] != '.'){
                    printf("%s\n", sd->d_name);
                }
            }
            closedir(dir); //close the opened directory.
        }
    } 
}

/**
 * @brief pwd function implementation in C. pwd command from the Bash programming language prints the current working directory.
 * @return the path of the current working directory.
 */
char * pwd(){
    char * buffer = NULL;

    buffer = malloc(1000 * sizeof(char));
    getcwd(buffer, 1000);
        
    printf("current working directory: %s\n", buffer);

    return buffer;
}
 
/**
 * @brief Helper cd function implementation in C. cd command from the Bash programming language changes the current working directory.
 */
void cd(){
    /* When cd is given in bash with no directory path the directory is changed to home.
    * getenv() standard library function is used to obtain the environment variable content of HOME.
    */
    chdir(getenv("HOME"));
    char *wd = pwd(); //printing the working directory to let the user know they changed directories.
    free(wd); //freeing the wd pointer because the working directory had memory allocated with malloc.

}

/**
 * @brief overloaded cd function implementation in C. Parameter is a directory path. cd command from the Bash programming language changes the current working directory.
 * @param parameter which are parameter[0], parameter[1]...parameter[n] are parameter 'tokens'.
 */
void cdOverloaded(char ** parameter){
    int success = chdir(parameter[0]);
    if(success == -1 && parameter[0] != NULL){
        printf("ERROR: %s, invalid directory path.\n", parameter[0]);
    }
    else if(parameter[0] == NULL){
        cd();
    }
    else{
        //print the working directory to let the user know they have been redirected.
        char *wd = pwd();
        //freeing the wd pointer since malloc was used to create the working directory.
        free(wd);

    }
}

/**
 * @brief env function implementation in C. env command from the Bash programming language prints all the environment variables.
 */
void env(char **envp){
    char **temp;

    for(temp = envp; *temp != NULL; temp++){
        printf("%s\n", *temp);
    }
}

/**
 * @brief The help command implementation in C.
 */
void help(){
    printf("niluSH by Nilushanth Thiruchelvam\n");
    printf("USAGE: [command] [option] [word]\nor\nUSAGE: [command] [argument]\n");
}

/**
 * @brief mkdir function implementation in C. mkdir command from the Bash programming language creates an empty directory in the current working directory.
 * 
 */
void mkdir(){
    
}


/**
 * @brief pipes function implementation in C. The symbol "|" used for pipe from the Bash programming language used to take output from one process as input to another process.
 */
void pipes(){
    
}

/**
 * @brief Only found in niluSH. Saves all contents in working directory to a different directory called niluSH-backup.
 */
void backup(){
    char * wd = pwd();
    free(wd); //freeing the wd pointer because the working directory had memory allocated with malloc.
    printf("Backup made in %s named \"niluSH-backup\"\n", pwd()); 
}

/**
 * @brief helper function for finding the index at the ith whitespace of the character pointer.
 * @param i which is the index of the ith whitespace.
 * @param pointer
 * @return int 
 */
int indexOfWhitespaceAt(int i, char *pointer){

    char *temp;
    int index;

    int numWhitespace = 0; //temporary position of a whitespace.

    for(temp = pointer; *temp != '\0' ; temp++){
        if(*temp == ' '){
            numWhitespace++;
        }
        if(numWhitespace == i){
            break; //number of desired whitespaces now found. Exit the loop.
        }
        index ++; //incrementing the index.
    }

    //if number of whitespaces is so, then the index returned should be zero since there arent any white spaces.
    if(numWhitespace == 0){
        index = 0;
    }

    return index;
}

/**
 * @brief helper method to find the length of a character pointer after using fgets.
 * @param pointer 
 * @return int size of the buffer.
 */
int sizeOfBufferFgets(char *pointer){

    char *temp;
    int size = 0;

    //fgets appends a newline character. So, until the newline character is found the size is incremented.
    for(temp = pointer; *temp != '\n'; temp++){
        size++;
    }

    return size;
}

//launching shell with parameters.
void shell_launch(char *command, char ** parameters, char **envp){

    pid_t pid = fork(); //executing the fork and storing the process ID.

    if(pid < 0){
        perror("ERROR: fork has failed.");
    }
    //if the pid > 0, the pid is at the parent process. The parent must wait for the child.
    if(pid > 0){
        wait(NULL); //waiting for child to complete.
    }
    //the fork is 0 which means pid is at the child process. At this child process the execution of the command occurs.
    else if(pid == 0){
       execvp(command, parameters); //In local path, executing the command with the parameters of the command (including the command itself).
    }

}

/**
 * @brief the loop for the shell. The child process from the fork executes the commands.
 */
void loop(char **envp){

    while(1){
        
        char buffer[1000]; //assuming a buffer size of 1000. TODO: if for some reason the buffer size is not enough for user, then increase buffer size dynamically.

        printf(">");
        fgets(buffer, 1000, stdin); //getting the command and parameter input from the user and storing it to the buffer.

        int size = sizeOfBufferFgets(buffer); //getting the size of the buffer.
        char(* parameters)[100]; //pointer to an array of 100 char elements. Can have any number or rows but limited to column of size 100. Each parameter should not exceed 100 characters.

        parameters = malloc(sizeof(* parameters));

        buffer[size] = ' '; //appending a space character at the end of buffer needed for splitting the buffer.
        /*
         * filling the *parameters[100] array with the parameters found after the initial command.
         */
        int row = 0;
        int col = 0;
        int j = 0;
        for(j = 0; j <= size; j++){
            if(buffer[j] == ' '){
                row++;
                col = 0;
                //printf("niluSH executed %s command\n", parameters[row-1]);
                continue; //skip immediately to the next iteration.
            }
            parameters[row][col] = buffer[j];
            col++;
        }

        char ** params = malloc(sizeof(char *) * row * 100);
        for(int i = 0; i < row; i++){
            params[i] = parameters[i];
        }

        char * command = parameters[0]; //command only.
        char ** paramArgs = &params[1]; //pointer of pointers parameters (excluding the command).

        /*
        * now "Built-in" niluSH commands. If the command is not found then the shell is launched.
        */
        //1. ls command.
        if(command[0] == 'l' && command[1] == 's'){
            lsOverloaded(paramArgs);
        }
        //2. cd command.
        else if(command[0] == 'c' && command[1] == 'd'){
            cdOverloaded(paramArgs);
        }
        //3. pwd command.
        else if(command[0] == 'p' && command[1] == 'w' && command[2] == 'd'){
            char *wd = pwd();
            free(wd); //freeing the working directory pointer because the working directory had memory allocated with malloc.
        }
        //4. echo command.
        else if(command[0] == 'e' && command[1] == 'c' && command[2] == 'h' && command[3] == 'o'){
            echo(paramArgs);
        }
        //5. touch command.
        else if(command[0] == 't' && command[1] == 'o' && command[2] == 'u' && command[3] == 'c' && command[4] == 'h'){
            touch(paramArgs);
        }
        //6. env command.
        else if(command[0] == 'e' && command[1] == 'n' && command[2] == 'v'){
            env(envp);
        }
        //7. help command.
        else if(command[0] == 'h' && command[1] == 'e' && command[2] == 'l' && command[3] == 'p'){
            help();
        }
        //8. backup command. (niluSH command).
        else if(command[0] == 'b' && command[1] == 'a' && command[2] == 'c' && command[3] == 'k' && command[4] == 'u' && command[5] == 'p'){
            backup();
        }
        else{
            //launching the unix shell with no parameters (only command).
            shell_launch(command,params,envp);
        }
    }
    
}

/**
 * @brief main function of the program.
 * @param argc is the number of command line arguments.
 * @param argv is char** pointer which represents each command line argument.
 * @param envp is the environment variables which are passed to the main.
 * @return 0 for successful completion of the program. 
 */
int main(int argc, char **argv, char **envp){

    printf("\nniluSH\n");
    printf("Type \"help\" for usage and list of available commands.\n");

    //available "built-in" niluSH commands.

    //executing the loop for the shell. Passing all the environment variables in the call.
    loop(envp);
    
    return 0;
}
