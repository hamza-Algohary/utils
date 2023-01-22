#undef try_in_order(A , B) try{A;} catch(...){B;}
#undef try_return_in_order(A , B) try{return A;} catch(...){return B;}
#undef no_throw(A) try{A;}catch(...){}