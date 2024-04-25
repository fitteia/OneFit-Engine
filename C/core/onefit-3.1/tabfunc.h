#define LINPAR 0
#define NOLINPAR 1

#define NEW_ENTRY(a,b,c,d) \
({ \
    double (b)();\
    foptions[Nfuncoes].name = (a); \
    foptions[Nfuncoes].ptr  = (b); \
    foptions[Nfuncoes].flag = (c); \
    foptions[Nfuncoes].comment = (d); \
    Nfuncoes++; \
    foptions = (Foption *) Realloc(foptions,(unsigned) (Nfuncoes+1)*sizeof(Foption)); \
    fprintf(pointers_file,"%s %p %d # %s\n",(a), (b), (c), (d)); \
    if(list_flag) printf("%-15s %p\t%d\t# %s\n",(a), (b), (c), (d)); \
})   
