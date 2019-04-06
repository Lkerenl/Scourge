#include <pch.h>


size_t KMP_search(char * str, char * substr, size_t memlen,size_t submem_len);
bool search_marked(uint8_t * ptr, mi_t ** marked, size_t len_of_ptr);
