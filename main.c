#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //C POSIX library which provides the API for changing directory, chdir() function.
#include <dirent.h> //C POSIX library for directory traversal. Only works on Linux systems.

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
 * @brief echo function implementation in C. The echo command from the Bash programming language outputs content in the shell.
 */
void echo(char *string){
    printf("%s\n", string);
}

/**
 * @brief touch function implementation in C. The touch command from the Bash programming language creates a new file.
 */
void touch(char *fileName){
    FILE * filePointer = NULL; //instantiating a file pointer to null.

    /*
     * the second argument is the mode which is used. In this case that mode is r+ (for reading and writing).
     * mode r+ is for reading and writing.
     * mode r is only for reading.
     * mode w is for writing and previous contents in the file get erased.
     * mode a is for appending.
     */
    filePointer = fopen(fileName, "r+");
    fclose(filePointer);

    if(filePointer == NULL){
        //this means the file is not created yet. So creating a new file with mode w.
        filePointer = fopen(fileName, "w");
        fclose(filePointer);

        if(filePointer == NULL){
            printf("ERROR: cannot write to or open the file %s\n", fileName);
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
 */
void lsOverloaded(char *dPath){
    
    //checking if directory path given is absolute or relative which will contain the "/" (forward-slash) symbol.
    char *tempPath;

    DIR *dir; //the directory stream pointer.
    struct dirent * sd; //pointer to the structure dirent.

    if(dPath == NULL){
        ls(); //list the working directory since no directory path was given.
    }
    else{
        //change directory to the given directory path.
        dir = opendir(dPath);

        if(dir == NULL){
            printf("ERROR: the %s directory is not valid\n", dPath);
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

            free(dPath);
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
 * @param dPath which is the directory path to change directory to.
 */
void cdOverloaded(char *dPath){
    int success = chdir(dPath);
    if(success == -1 && dPath != NULL){
        printf("ERROR: %s, invalid directory path.\n", dPath);
    }
    else if(dPath == NULL){
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
 * @brief finding the index at the ith whitespace of the character pointer.
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

/**
 * @brief the loop for the shell.
 */
void loop(char **envp){

    char buffer[1000]; //assuming a buffer size of 1000. TODO: if for some reason the buffer size is not enough for user, then increase buffer size dynamically.

    printf(">");
    fgets(buffer, 100, stdin);

    int size = sizeOfBufferFgets(buffer); //getting the size of the buffer.
    buffer[size] = '\0'; //appending a null character which was originally a newline character by fgets.

    int indexArg2 = indexOfWhitespaceAt(1, buffer); //index at the first whitespace in the buffer.
    char * argument2 = &buffer[indexArg2 + 1]; //argument 2 pointer which is right after the first whitespace 

    if(indexArg2 == 0){
        //setting argument 2 to be null.
        argument2 = NULL;
    }
    /*
     * The cases for the different useful re-implmentation of bash commands and niluSH commands.
     */
    //1. ls command.
    if(buffer[0] == 'l' && buffer[1] == 's'){
        lsOverloaded(argument2);
    }
    //2. cd command.
    else if(buffer[0] == 'c' && buffer[1] == 'd'){
        cdOverloaded(argument2);
    }
    //3. pwd command.
    else if(buffer[0] == 'p' && buffer[1] == 'w' && buffer[2] == 'd'){
        char *wd = pwd();
        free(wd); //freeing the working directory pointer because the working directory had memory allocated with malloc.
    }
    //4. echo command.
    else if(buffer[0] == 'e' && buffer[1] == 'c' && buffer[2] == 'h' && buffer[3] == 'o'){
        echo(argument2);
    }
    //5. touch command.
    else if(buffer[0] == 't' && buffer[1] == 'o' && buffer[2] == 'u' && buffer[3] == 'c' && buffer[4] == 'h'){
        touch(argument2);
    }
    //6. env command.
    else if(buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'v'){
        env(envp);
    }
    //7. help command.
    else if(buffer[0] == 'h' && buffer[1] == 'e' && buffer[2] == 'l' && buffer[3] == 'p'){
        help();
    }
    //8. backup command. (niluSH command)
    else if(buffer[0] == 'b' && buffer[1] == 'a' && buffer[2] == 'c' && buffer[3] == 'k' && buffer[4] == 'u' && buffer[5] == 'p'){
        backup();
    }

    loop(envp); //loop the program again.
    
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
    
    //executing the loop for the shell. Passing all the environment variables in the call.
    loop(envp);
    
    return 0;
}
