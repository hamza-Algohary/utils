#define try_in_order(A , B) try{A;} catch(...){B;}
#define try_return_in_order(A , B) try{return A;} catch(...){return B;}
#define no_throw(A) try{A;}catch(...){}