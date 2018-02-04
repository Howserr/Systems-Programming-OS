// Implementation of string and memory functions

#include <string.h>
#include <console.h>

int strcmp(const char* str1, const char* str2)
{
    while(*str1 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

char * strcpy(char * destination, const char * source)
{
    char* tmp_ptr = destination;
    do
    {
        *destination++ = *source;
    } while(*source++);

    return tmp_ptr;
}

errno_t strcpy_s(char *destination, size_t numberOfElements, const char *source)
{

    if (destination == NULL)
    {
        return 22;
    }
    if (source == NULL)
    {
        *destination = '\0';
        return 22;
    }
    if (numberOfElements < strlen(source)) 
    {
        *destination = '\0';
        return 34;
    }
    char* tmp_ptr = destination;

    do
    {   
        *destination++ = *source;
    } while(*source++);
    
    *destination = '\0';

    return 0;
}

size_t strlen(const char* source)
{
    size_t length = 0;

    while(*source++)
    {
        length++;
    }

    return length;
}

void * memcpy(void * destination, const void * source, size_t count)
{
    char* d = destination;
    const char* s = source;
    for(size_t i = 0; i < count; i++)
    {
        *d++ = *s++;
    }
    return destination;
}

errno_t memcpy_s(void *destination, size_t destinationSize, const void *source, size_t count)
{
    char* ptr_dest = destination;
    const char* ptr_source = source;
    
    if (ptr_dest == NULL)
    {
        return 22;
    }
    if (ptr_source == NULL)
    {
        *ptr_dest = '\0';
        return 22;
    }
    if (destinationSize < count) 
    {
        *ptr_dest = '\0';
        return 34;
    }

    do
    {   
        *ptr_dest++ = *ptr_source;
    } while(*ptr_source++);
    
    *ptr_dest = '\0';

    return 0;
}

void * memset(void *destination, char val, size_t count)
{
    char* ptr_dest = destination;
    for(size_t i = 0; i < count; i++)
    {
        *ptr_dest++ = val;
    }
    return destination;
}

unsigned short * memsetw(unsigned short * destination, unsigned short val, size_t count)
{
    unsigned short* tmp_dest = destination;
    for(size_t i = 0; i < count; i++)
    {
        *destination++ = val;
    }
    return tmp_dest;
}