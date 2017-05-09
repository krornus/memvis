#include <stdio.h>

void secretFunction(){
    printf("congrats!\n");
}
void echo(){
    char buffer[30];
    printf("%s",buffer);
    printf("you entered %s\n");
}
int main(){
    echo();
    return 0;
}
