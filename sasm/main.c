#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, const char * argv[]) {
    
    char* path = "/Users/jhladik/Documents/Development/sasm/program.sasm";
    
    // open file
    FILE* fp = fopen(path, "r");
    if (!fp) exit(EXIT_FAILURE);
    
    // figure out the size of the file
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // load file in the memory and close file
    char *file = malloc(fsize + 1);
    fread(file, fsize, 1, fp);
    fclose(fp);
    
    // null terminate the file
    file[fsize] = '\0';
    
    // ********************************
    // start process file
    // ********************************
    
    // process file line by line
    char* line = file;
    while (line)
    {
        // chop line with a null terminator
        char* next_line = strchr(line, '\n');
        if (next_line) *next_line = '\0';
        
        // ********************************
        // start process line
        // ********************************
        printf("%s\n", line);
        //char* token = strtok(line, " ,\t\n\v\f");
        //printf("    %s", token);
        // ********************************
        // end process line
        // ********************************
        
        
        // update line pointer
        line = next_line ? next_line + 1 : NULL;
    }
    
    // ********************************
    // end process file
    // ********************************
    
    free(file);
    exit(EXIT_SUCCESS);
}
