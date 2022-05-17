#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <mutex>
#include "new_stack.hpp"
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h> 

//this lock may be modified or released using any of these file descriptors//
struct flock lock_;
int fd;

void file_de(){
    fd = open("example.txt", O_WRONLY | O_CREAT);
}

void memory_to_shared(struct stack* stack) {
    char *start_ptr = (char*)mmap(NULL, sizeof(node)*30, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    stack->addres_ = start_ptr;
    cout << "The initial memory address is: "<< start_ptr << endl; 
    //return initialptr;
}

void * my_malloc(stack* stack){
    stack->addres_ +=sizeof(struct node);
    return stack->addres_;
}

void my_free(node *item_to_free,stack *stack){
    stack->addres_ -=sizeof(struct node);
}
#include <string.h>
void PUSH(char* data, stack *stack){
    //////locking///////
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    /////pointer to continue the shared memory///
    node *n = (node*)my_malloc(stack);
    //// print the location for debug///
    cout << "the string push at:" << stack->addres_ << endl;
    //printf("push at: %p\n", stack->addres_);
    ///// the new node/////
    strcpy(n->data , data);
    node *temp = stack->head; 
    stack->head = n;
    n->next = temp;
    stack->top_data = data;
    stack->size++;
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);

}

string POP(struct stack* stack){
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    string data;
    if (stack->size == 0 )
    {
        data = "stack is empty";
    }
    else 
    {
    struct node *temp = stack->head;
    stack->head = temp->next;
    cout << "the node poped from:" << stack->addres_ << endl;
    data = temp->data;
    stack->top_data = stack->head->data;
    cout << "the top_data now is:" << stack->top_data << endl;
    my_free(temp, stack);
    stack->size--;
    }
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);
    return data;
}

string TOP(struct stack stack){
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    const char* top ="stack is empty";
    if (stack.size == 0)
    {
      cout << "stack is empty";  
      //top ="stack is empty";
    }   
    else{
    printf("the node top from: %p\n", stack.addres_);
   // cout << "data: "<<stack.top_data << endl;
    top = stack.head->data;
    }
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);
    return top;
}


// node::node(){

//  }
// int main(){
//     stack st;
//     string s = "ilan";
//     st.PUSH(s);
//     string ilan = st.POP();
//      std::cout<<ilan<<std::endl;
//     string s1 = "ilan2";
//     st.PUSH(s1);
//     string ilan2 = st.POP();
//     std::cout<<ilan2<<std::endl;
//     string s2 = "ilan3";
//      st.PUSH(s2);
//     string ilan3 = st.POP();
//      std::cout<<ilan3<<std::endl;
//     st.PUSH(s);
//     st.PUSH(s1);
//     st.PUSH(s2);
//     string test_top = st.TOP();
//      string ilan4 = st.POP();
//      std::cout<<test_top<<std::endl;
// }