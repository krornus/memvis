#include <stdio.h>
#include <string.h>

void main(){
    char command[50];
    strcpy( command, "ls -l" );
    system(command);
}
