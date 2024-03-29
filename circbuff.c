#include<circbuff.h>

void circbuff_push(char k){
  if(!circbuff_full()){
    *circbuffptr_last = k;
    circbuffptr_last = (circbuffptr_last+1);
    if (circbuffptr_last >= &keybuffer[CIRCBUFF_SIZE]) circbuffptr_last=&keybuffer[0];
    ++circbuffcounter;
  }
}

char circbuff_front(){
  return *circbuffptr_first;
}

void circbuff_pop(){
  if(!circbuff_empty()){
  circbuffptr_first = circbuffptr_first+1;
  if (circbuffptr_first >= &keybuffer[CIRCBUFF_SIZE]) circbuffptr_first=&keybuffer[0];
  --circbuffcounter;
  }
}

char circbuff_empty(){
  return (circbuffcounter==0);
}
char circbuff_full(){
  return (circbuffcounter==sizeof(keybuffer)/sizeof(keybuffer[0]));
}
int circbuff_count(){
  return circbuffcounter;
}
