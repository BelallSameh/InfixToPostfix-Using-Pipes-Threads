#include "c_stack.h"
#include "d_stack.h"
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1
sem_t s;

int fd1[2], fd2[2], fd3[2], fd4[2],fd5[2];

struct arg_struct {
  long long arg1;
  long long arg2;
};

void *add(void *arg) {
  long long sum;
  sem_wait(&s);
  /** Enter CRITICAL SECTION **/
  struct arg_struct *args = arg;
  printf("Thread 1 adding: \n");
  sum = (args->arg1) + (args->arg2);
  write(fd1[WRITE_END], &sum, sizeof(long long));
  /** Exit CRITICAL SECTION **/
  printf("----------------------------------------\n");
  sem_post(&s);
  return NULL;
}

void *sub(void *arg) {
  long long diff;
  sem_wait(&s);
  /** Enter CRITICAL SECTION **/
  struct arg_struct *args = arg;
  printf("Thread 2 subtracting: \n");
  diff = (args->arg2) - (args->arg1);
  write(fd2[WRITE_END], &diff, sizeof(long long));
  /** Exit CRITICAL SECTION **/
  printf("----------------------------------------\n");
  sem_post(&s);
  return NULL;
}

void *mul(void *arg) {
  long long mult;
  sem_wait(&s);
  /** Enter CRITICAL SECTION **/
  struct arg_struct *args = arg;
  printf("Thread 3 multiplying:\n");
  mult = (args->arg1) * (args->arg2);
  write(fd3[WRITE_END], &mult, sizeof(long long));
  /** Exit CRITICAL SECTION **/
  printf("----------------------------------------\n");
  sem_post(&s);
  return NULL;
}

void *divd(void *arg) {
  long long div;
  sem_wait(&s);
  /** Enter CRITICAL SECTION **/
  struct arg_struct *args = arg;
  printf("Thread 4 dividing:\n");
  div = (args->arg2) / (args->arg1);
  write(fd4[WRITE_END], &div, sizeof(long long));
  /** Exit CRITICAL SECTION **/
  printf("----------------------------------------\n");
  sem_post(&s);
  return NULL;
}

int priority(char in) {
  switch (in) {
  case '*':
    return 2;
  case '/':
    return 2;
  case '+':
    return 1;
  case '-':
    return 1;
  }
  return 0;
}

int main() {
  /* Initialize semaphore s */
  sem_init(&s, 0, 1);

  if (pipe(fd1) < 0) {
    perror("pipe 1 error");
    exit(1);
  }
  if (pipe(fd2) < 0) {
    perror("pipe 2 error");
    exit(1);
  }
  if (pipe(fd3) < 0) {
    perror("pipe 3 error");
    exit(1);
  }
  if (pipe(fd4) < 0) {
    perror("pipe 4 error");
    exit(1);
  }
  if (pipe(fd5) < 0) {
    perror("pipe 4 error");
    exit(1);
  }
  // string for the input expression (infix)
  char infix[30];
  // string for the output expression (postfix)
  char postfix[30] = {'\0'};
  // scan the infix from the user
  printf("Enter the equation you want to calculate : ");
  scanf("%s@", infix);
  printf("----------------------------------------\n");
  //================================== Conversion
  // create a stack for the operators and (
  C_Stack stack = c_init_stack(30);
  //=========================
  int i, j = 0;
  // loop over the infix (char by char)
  for (i = 0; i < strlen(infix); i++) {
    // skip the space in the infix
    if (infix[i] == ' ')
      continue; // skip the current iteration
    if (infix[i] == '(') {
      c_push(&stack, infix[i]);
    } else if (infix[i] == ')') {
      while (c_stack_state(&stack) > 0 && // stack not empty
             c_peek(&stack) !=
                 '(' // top operator has >= priority than the current operator
      ) {
        postfix[j++] = c_pop(&stack);
        postfix[j++] = ' ';
      }
      c_pop(&stack); // pop and discard the '('
    } else if (infix[i] == '^') {
      c_push(&stack, infix[i]);
    } else if (priority(infix[i]) > 0) {
      while (c_stack_state(&stack) > 0 && // stack not empty
             priority(c_peek(&stack)) >=
                 priority(infix[i]) // top operator has >= priority than the
                                    // current operator
      ) {
        // while stack is not empty & the top operator priority >= from the
        // current add the top element to the postfix
        postfix[j++] = c_pop(&stack);
        postfix[j++] = ' ';
      }
      // push the current operator
      c_push(&stack, infix[i]);
    } else { // if operand
      postfix[j++] = infix[i];
      postfix[j++] = ' ';
    }
  }
  // pop the remaining items from the stack & add to the postfix
  while (c_stack_state(&stack) > 0) {
    postfix[j++] = c_pop(&stack);
    postfix[j++] = ' ';
  }
  printf("Postfix:\t%s\n", postfix);
  printf("----------------------------------------\n");
  printf("----------------------------------------\n");
  //================================== Evaluation

  D_Stack stack2 = d_init_stack(30);
  for (i = 0; i < strlen(postfix); i++) {
    // ignore the spaces
    if (postfix[i] == ' ')
      continue;
    // ignore the spaces
    if (priority(postfix[i]) > 0) {
      // if operator
      long long a = d_pop(&stack2);
      long long b = d_pop(&stack2);
      long long z = 0;
      if (postfix[i] == '+' || postfix[i] == '-') {
        pid_t child1 = fork();
        if (child1 == 0) {
          printf("I'm 1st child and calculating + and -\n");

          /* Main thread for addition and subtraction */
          printf("Main add & sub Thread: \n");
          printf("----------------------------------------\n");
          printf("----------------------------------------\n");
          /* Child creates two threads */

          struct arg_struct args;
          args.arg1 = a;
          args.arg2 = b;
          if (postfix[i] == '+') {
            pthread_t t1;
            pthread_create(&t1, NULL, add, (void *)&args);
            read(fd1[READ_END], &z, sizeof(long long));
            write(fd5[WRITE_END], &z, sizeof(long long));
            pthread_join(t1, NULL);
          } else if(postfix[i] == '-') {
            pthread_t t2;
            pthread_create(&t2, NULL, sub, (void *)&args);
            read(fd2[READ_END], &z, sizeof(long long));
            write(fd5[WRITE_END], &z, sizeof(long long));
            pthread_join(t2, NULL);
          }
          exit(0);
        }
        else {
          long long got;
          read(fd5[READ_END], &got, sizeof(long long));
          d_push(&stack2,got);
          wait(NULL);
        }

      } else {
        pid_t child2 = fork();
        if (child2 == 0) {
          printf("I'm 2nd child and calculating * and /\n");

          /* Main thread for multiplication and division */
          printf("Main mul & div Thread:\n");
          printf("----------------------------------------\n");
          printf("----------------------------------------\n");

          struct arg_struct args;
          args.arg1 = a;
          args.arg2 = b;

          if (postfix[i] == '*') {
            pthread_t t3;
            pthread_create(&t3, NULL, mul, (void *)&args);
            read(fd3[READ_END], &z, sizeof(long long));
            write(fd5[WRITE_END], &z, sizeof(long long));
            pthread_join(t3, NULL);
          } else if(postfix[i] == '/'){
            pthread_t t4;
            pthread_create(&t4, NULL, divd, (void *)&args);
            read(fd4[READ_END], &z, sizeof(long long));
            write(fd5[WRITE_END], &z, sizeof(long long));
            pthread_join(t4, NULL);
          }
          exit(0);
        } else {
          long long got;
          read(fd5[READ_END], &got, sizeof(long long));
          d_push(&stack2,got);
          wait(NULL);
        }
      }
    } else {
      // if operand convert to double and add to the stack
      long long x = postfix[i] - '0';
      d_push(&stack2, x);
    }
  }
  long long value = d_pop(&stack2);
  printf("Result: %lld\n", value);
  return 0;
}