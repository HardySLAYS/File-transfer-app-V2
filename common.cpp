#include "common.h"

int check(int val,const char* msg){
    if(val >= 0)return 0;
    cerr << val << endl;
    cerr << strerror(errno) << endl;
    cerr << msg;
    exit(EXIT_FAILURE);
}