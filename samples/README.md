Je eine Beispielanwendung für alle 4 derzeit in **xmlclean** vordefinierten worker-callbacks:
```c
void worker_nop   (int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
void worker_csv   (int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
void worker_clean (int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
void worker_pretty(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
```
Jeweils liegt ein 32Bit-Compilat für Win32 und Linux86 bei.
