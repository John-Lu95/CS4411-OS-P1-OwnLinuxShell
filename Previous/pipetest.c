#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
  
int main()  
{  
    int pfds[2];  
  
    if ( pipe(pfds) == 0 ) {  
  
    if ( fork() == 0 ) {  
        printf("ls\n");
        close(1);  
        dup2( pfds[1], 1 );  
        close( pfds[0] );
        
        execlp( "ls", "ls", "-l", NULL );  
  
    } else {  
        printf("wc\n");
        close(0);  
        dup2( pfds[0], 0 );  
        close( pfds[1] );  
        
        execlp( "wc", "wc", "-l", NULL );  
  
    }
    }  
  
    return 0;  
}  