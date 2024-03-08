#ifndef kernel
#define kernel

// defing the uint8_t and uint16_t
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

// making the bool data type and it values
typedef uint8_t bool;
#define true 1
#define false 0

// max chars in the input
#define MAX_INPUT_LENGTH 256

// defing the size_t
#define size_t unsigned int

// prototypes
void printc(char chr);
void prints(char* str);
void clear();
void color_bg(uint8_t color);
char* input(char* prompt);
int strcmp(const char *s1, const char *s2);
void shutdown();
void check(char* str);


#endif /* kernel */
