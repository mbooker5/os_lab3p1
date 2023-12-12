// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use one pipe for each direction
    int fd1[2];  // Used to store two ends of pipe from P1 to P2
    int fd2[2];  // Used to store two ends of pipe from P2 to P1
  
    char fixed_str_p1[] = "howard.edu";
    char fixed_str_p2[] = "gobison.org";
    char input_str[100];
    
    pid_t p1, p2;  // Process IDs for P1 and P2
  
    if (pipe(fd1) == -1 || pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    
    printf("Enter a string for P1 to concatenate:");
    scanf("%s", input_str); 
    
    p1 = fork(); 
    
    if (p1 < 0) 
    { 
        fprintf(stderr, "fork P1 Failed" ); 
        return 1; 
    } 
    else if (p1 > 0) 
    { 
        // P1 (Parent) process
        
        close(fd1[0]);  // Close reading end of P1 to P2 pipe
        close(fd2[1]);  // Close writing end of P2 to P1 pipe
  
        // Write input string from P1 and close writing end of P1 to P2 pipe.
        write(fd1[1], input_str, strlen(input_str) + 1); 
        
        // Wait for P2 to send a string
        wait(NULL); 
  
        // Read concatenated string from P2
        char concat_str_p2[100];
        read(fd2[0], concat_str_p2, 100); 
  
        // // Concatenate a fixed string with it
        // int k = strlen(concat_str_p2); 
        // int i; 
        // for (i = 0; i < strlen(fixed_str_p2); i++) 
        //     concat_str_p2[k++] = fixed_str_p2[i]; 
  
        concat_str_p2[-1] = '\0';   // String ends with '\0' 
  
        printf("Concatenated string from P2: %s\n", concat_str_p2);
        
        // Close both ends of pipes
        close(fd1[1]);
        close(fd2[0]); 
    } 
    else
    { 
        // P2 (Child) process
        
        close(fd1[1]);  // Close writing end of P1 to P2 pipe
        close(fd2[0]);  // Close reading end of P2 to P1 pipe
      
        // Read a string from P1
        char concat_str_p1[100]; 
        read(fd1[0], concat_str_p1, 100); 
  
        // Concatenate a fixed string with it
        int k = strlen(concat_str_p1); 
        int i; 
        for (i = 0; i < strlen(fixed_str_p1); i++) 
            concat_str_p1[k++] = fixed_str_p1[i]; 
  
        concat_str_p1[k] = '\0';   // String ends with '\0' 
  
        printf("Concatenated string from P1: %s\n", concat_str_p1);
        
        // Prompt user for a second input
        printf("Enter a string for P2 to concatenate:");
        scanf("%s", input_str);
        
        strcat(concat_str_p1, input_str);
        // Write the input string from P2 and close writing end of P2 to P1 pipe.
        write(fd2[1], concat_str_p1, strlen(concat_str_p1) + 1); 
        
        // Close both ends of pipes
        close(fd1[0]);
        close(fd2[1]); 
        
        exit(0); 
    } 
}