#include <stdlib.h>
typedef struct {
  char *arr;
  int top;
  int max;
} C_Stack;
C_Stack c_init_stack(int size) {
  C_Stack new_stack;
  new_stack.max = size;
  new_stack.top = 0;
  new_stack.arr = (char *)malloc(size * sizeof(char));
  return new_stack;
}
int c_stack_state(C_Stack *stack) {
  if (stack->top == 0) { // stack is empty (underflow)
    return 0;
  }
  if (stack->top == stack->max) { // stack is full (oveflow)
    return 1;
  }
  return 2;
}
void c_push(C_Stack *stack, char new_data) {
  if (c_stack_state(stack) != 1) {
    stack->arr[stack->top] = new_data;
    stack->top++;
  }
}
char c_pop(C_Stack *stack) {
  if (c_stack_state(stack) == 0) {
    return '\0';
  }
  stack->top--;
  return stack->arr[stack->top];
}
char c_peek(C_Stack *stack) {
  if (c_stack_state(stack) == 0) {
    return '\0';
  }
  return stack->arr[stack->top - 1];
}