#include <stdlib.h>
typedef struct {
  long long *arr;
  int top;
  int max;
} D_Stack;
D_Stack d_init_stack(int size) {
  D_Stack new_stack;
  new_stack.max = size;
  new_stack.top = 0;
  new_stack.arr = (long long *)malloc(size * sizeof(long long));
  return new_stack;
}
long long d_stack_state(D_Stack *stack) {
  if (stack->top == 0) { // stack is empty (underflow)
    return 0;
  }
  if (stack->top == stack->max) { // stack is full (oveflow)
    return 1;
  }
  return 2;
}
void d_push(D_Stack *stack, long long new_data) {
  if (d_stack_state(stack) != 1) {
    stack->arr[stack->top] = new_data;
    stack->top++;
  }
}
long long d_pop(D_Stack *stack) {
  if (d_stack_state(stack) == 0) {
    return -1;
  }
  stack->top--;
  return stack->arr[stack->top];
}
long long d_peek(D_Stack *stack) {
  if (d_stack_state(stack) == 0) {
    return -1;
  }
  return stack->arr[stack->top - 1];
}