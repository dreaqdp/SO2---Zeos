char keybuffer[666];
char *circbuffptr_last; 
char *circbuffptr_first;
int circbuffcounter;


void circbuff_push(char k);

char circbuff_front();

void circbuff_pop();

char circbuff_empty();

char circbuff_full();

int circbuff_count();
