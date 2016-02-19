#ifdef __arm__	/* Arduino DUE */


#define PSTR(str) (str)
#define strcpy_P(dest, src) strcpy((dest), (src))	
#define strncpy_P(dest, src, n) strncpy((dest), (src), (n))
#define strcmp_P(a, b) strcmp((a), (b))
#define strncmp_P(a, b, n) strncmp((a), (b), (n))
// #define strlen_P(a) strlen((a))

#else




#endif