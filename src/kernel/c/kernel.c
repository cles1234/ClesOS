#include "kernel.h"

// main in the kernel
void main() {
    // clear screen
    clear();
    // change color of back ground
    color_bg(1);
    // printing string welcome to clesos
    prints("welcome to clesos\r\n");

    // the kerel loop
    while (true) {
        // take input from the user
        char* str = input(">");

        // print new line
        prints("\r\n");

        // check the command
        check(str);
    }
}

// print charectart
void printc(char chr){
    // ah = 0x0E 
    // al = chr
    // int 10h
    __asm__ volatile(
        "movb $0x0E, %%ah\n"
        "movb %0, %%al\n"    
        "int $0x10\n"       
        :
        : "r" (chr)
        : "ah", "al"
    ); 
}

// print string
void prints(char* str){
    // print every char in the string until it end
    for(int i = 0; str[i] != '\0'; i++)
        printc(str[i]);
}

// clear the screen
void clear(){
    // ah = 0x00
    // al = 0x03
    // int 0x10
    __asm__ volatile(
    "mov $0x00,%%ah\n"
    "mov $0x03,%%al\n"
    "int $0x10"
    :
    :
    :"al","ah"
    );
}

// change color of back ground
void color_bg(uint8_t color) {
    // ah = 0x0b
    // bh = 0
    // bl = color
    // int 0x10
    __asm__ volatile (
        "mov $0x0b,%%ah\n"
        "mov $0,%%bh\n"
        "mov %0,%%bl\n"
        "int $0x10\n"
        :
        :"r"(color)
        :"ah","bh","bl"
    );
}

// take input
char* input(char* prompt) {
    // Print the prompt to the screen
    prints(prompt);

    // Initialize a buffer to store user input
    char* str = (char*)0x8000; 
    str[0] = '\0'; // Ensure the buffer is empty initially
    int index = 0; // Initialize an index to keep track of the current position in the buffer

    char c; // Variable to store each character read from the user

    // Continue reading input until the Enter key is pressed
    while (1) {
        // Read a character from the keyboard
        __asm__ volatile (
            "mov $0x00, %%ah\n"  // AH = 0x00 (function to read a character)
            "int $0x16\n"        // BIOS interrupt 0x16 for keyboard input
            : "=a" (c)           // Output: store the read character in 'c'
            : "a" (0x0000)       // Input: AH = 0x00 (function to read a character)
            : "cc"               // Clobbered registers
        );

        // Check if the Enter key (ASCII 13) was pressed
        if (c == 13) {
            break; // Exit the loop if Enter was pressed
        }

        // Handle backspace (ASCII 8)
        if (c == 8) {
            if (index > 0) {
                index--;         // Decrement index to remove the last character
                str[index] = '\0'; // Null-terminate the string at the new end
                printc(c);        // Print the backspace character to erase the character on the screen
            }
        } else {
            // If not backspace, add the character to the buffer
            if (index < MAX_INPUT_LENGTH - 1) {
                str[index++] = c; // Add the character to the buffer and increment index
                str[index] = '\0'; // Null-terminate the string
                printc(c);         // Print the character to the screen
            }
        }
    }

    // Return the input string
    return str;
}

// compare a str1 to str2 if equal return 0 
int strcmp(const char *s1, const char *s2) {
    // do this until the s1 end adn s2 and they are equal
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    // return
    return *s1 - *s2;
}

// make a outw function to indcate the outw in assembly
static inline void outw(uint16_t port, uint16_t data) {
    // outw data,port
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a" (data), "Nd" (port)
    );
}

// shutdown
void shutdown() {
    // shutdown ports 
    uint16_t ports[] = {0x604, 0xB004,0x400, 0x402, 0x404, 0x406, 0x408, 0x40A, 0x40C, 0x40E, 0x410, 0x412,0x414, 0x416, 0x418};
    // number of ports is size of ports / size of uint16_t
    int num_ports = sizeof(ports) / sizeof(uint16_t);
    // for loop if i is lesser than number_ports
    for (int i = 0; i < num_ports; i++) {
        // give signal to the current port 0x2000
        outw(ports[i], 0x2000); 
    }
    // if the port is not here
    prints("ERROR: could not shutdown\r\n");
}

// reboot or restart
void reboot() {
    // reboot ports
    uint16_t ports[] = {0x64, 0x400, 0x402, 0x406, 0x408, 0x40A, 0x40C, 0x40E, 0x410, 0x412, 0x414, 0x416, 0x418, 0x604};
    // number of ports is size of ports / size of uint16_t
    int num_ports = sizeof(ports) / sizeof(uint16_t);
    // for loop if i is lesser than number_ports
    for (int i = 0; i < num_ports; i++) {
        // give signal to the current port 0x2000
        outw(ports[i], 0x2000); 
    }
    // if the port is not here
    prints("ERROR: could not reboot\r\n");
}

// compare string but start from start end in end
bool scmp(int start, int end, const char* str1, const char* str2) {
    // Loop through each character in the specified range
    for (int i = start, j = 0; i <= end; i++, j++) {
        // Check if characters at the current positions are different
        if (str1[i] != str2[j]) {
            return false; // If different, return false
        }
        // Check if either of the strings has reached its end
        if (str1[i] == '\0' || str2[j] == '\0') {
            return false; // If either has reached its end, return false
        }
    }
    return true; // If all characters in the range match and both strings have not reached their ends, return true
}

// return the len of string
size_t strlen(const char *s) {
    // p is equal to s
    const char *p = s;
    // while p is not equal \0 the end add
    while (*p != '\0') {
        // add p by one
        p++;
    }
    // p - s the len
    return p - s;
}
// function that give matching chars
size_t strspn(const char *str, const char *accept) {
    // Initialize count to store the number of characters matching those in accept
    size_t count = 0;
    const char *p;

    // Iterate through each character in the str string
    while (*str) {
        // Iterate through each character in the accept string
        for (p = accept; *p; p++) {
            // Check if the current character in str matches any character in accept
            if (*str == *p) {
                count++; // If there's a match, increment count
                break;   // Exit the inner loop
            }
        }
        // If the inner loop exits without finding a match, break the outer loop
        if (*p == '\0') {
            break;
        }
        str++; // Move to the next character in str
    }

    // Return the count of matching characters
    return count;
}

// check the command
void check(char* str) {
    // if it is shutdown then shutdwon
    if (strcmp(str, "shutdown") == 0 || strcmp(str, "Shutdown") == 0) {
        // call the shutdown function
        shutdown();
    }
    // if it is reboot or restart
    else if (strcmp(str, "reboot") == 0 || strcmp(str, "Reboot") == 0 || strcmp(str, "restart") == 0 || strcmp(str, "Restart") == 0) {
        // call the reboot function
        reboot();
    }
    // if it is echo 
    else if (scmp(0,4,str,"echo "))
    {
        // print the after echo
        prints(str + 5);
        // print newline
        prints("\r\n");
    }
    // if it is clear
    else if (strcmp(str,"clear") == 0)
    {
        // clear
        clear();
        // change back ground color to blue
        color_bg(0x01);
    }
    // if it is help
    else if (strcmp(str,"help") == 0)
    {
        // print the shutdown and info
        prints("shutdown: to shutdown\r\n");
        // print the reboot or restart and info
        prints("reboot or restart: to reboot\r\n");
        // echo the shutdown and info
        prints("echo : print the words that is after it\r\n");
        // print the clear and info
        prints("clear: to clear the screen\r\n");
    }
    // if it is space or nothing 
    else if (strspn(str, " ") == strlen(str)) {
        return;
    }
    // else print 'str' is not command
    else {
        // print `
        printc('`');
        // print str
        prints(str);
        // print `
        printc('`');
        // print it is not command
        prints(" is not a command!\r\n");
    }
}
