//
//  main.c
//  rpn
//
//  Created by William McCarthy on 192//20.
//  Copyright © 2020 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100
#define MATH '1'
#define VARIABLE '2'

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

double variables[26];

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }
void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

int getop(char* s) {
  int i, c;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';

  if (isalpha(c)) {  // get digits before '.'
    i = 0;
    while (isalpha(s[++i] = c = getch_())) { }
    if (c != EOF) { ungetch_(c); }
    if(i <= 1) {return s[0];}
    s[i] = '\0';
    return MATH;
  }

  if (c == '=' || c == '?')
  {
    s[1] = c = getch_();
    s[2] = '\0';
    return VARIABLE;
  }

  if (!isdigit(c) && c != '.') { return c; }  // if not a digit, return

  i = 0;
  if (isdigit(c)) {  // get digits before '.'
    while (isdigit(s[++i] = c = getch_())) { }
  }
  if (c == '.') {    // get digits after decimal (if any)
    while (isdigit(s[++i] = c = getch_())) { }
  }
  s[i] = '\0';
  if (c != EOF) { ungetch_(c); }
  return NUMBER;      // return type is NUMBER, number stored in s
}

double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

void printtop(void)
{
  if (sp == 0) { fprintf(stderr, "stack underflow\n");}
  double temp;
  temp = pop();
  printf("\t%.8g\n", temp);
  push(temp);
}

void dup(void)
{
  double temp = pop();
  push(temp);
  push(temp);
}

void swap(void)
{
  double temp2 = pop();
  double temp1 = pop();
  push(temp2);
  push(temp1);
}

void clearstack(void)
{
  sp = 0;
}

void math(char *s)
{
  double temp;
  if(strcmp(s, "sin") == 0)
  {
    push(sin(pop()));
  }
  else if(strcmp(s, "exp") == 0)
  {
    push(exp(pop()));
  }
  else if(strcmp(s, "pow") == 0)
  {
    temp = pop();
    push(pow(pop(), temp));
  }
  else
  {
    printf("Error: Unrecognized function");
  }
}

void variable(char* s)
{
  if(s[0] == '=')
  {
    variables[s[1] - 'A'] = pop();
  }
  else if(s[0] == '?')
  {
    push(variables[s[1] - 'A']);
  }
}

void rpn(void) {
  int type;
  int temp1, temp2;
  double op2;
  char s[BUFSIZ];

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case '\n':    printf("\t%.8g\n", pop());  break;
      case MATH:    math(s);                    break;
      case VARIABLE:variable(s);                 break;
      case 'p':     printtop();                 break;
      case 'c':     clearstack();               break;
      case 'd':     dup();                      break;
      case 's':     swap();                     break;
      case NUMBER:  push(atof(s));              break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case '%':
        temp2 = pop();
        temp1 = pop();
        push(temp1 % temp2); break;
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;
      default:      fprintf(stderr, "unknown command %s\n", s);  break;
    }
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
