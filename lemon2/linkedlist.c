#define linkedlist(TYPE) \
\
TYPE* TYPE##listremove(TYPE * first, TYPE * item)\
{\
    if(item==first)\
	return first->next;\
    else\
    {\
	TYPE * next = first;\
	while(next)\
	{\
	    if(next->next==item)\
	    {\
	        next->next=item->next;\
	        return first;\
	    }\
	    next=next->next;\
	}\
    }\
    return 0;\
}

