/*
 * ICT1002 Assignment 2, 2018-19 Trimester 2.
 *
 * This file contains the definitions and function prototypes for all of
 * features of the ICT1002 chatbot.
 */
 
#ifndef _CHAT1002_H
#define _CHAT1002_H

#include <stdio.h>

/* the maximum number of characters we expect in a line of input (including the terminating null)  */
#define MAX_INPUT    256

/* the maximum number of characters allowed in the name of an intent (including the terminating null)  */
#define MAX_INTENT   32

/* the maximum number of characters allowed in the name of an entity (including the terminating null)  */
#define MAX_ENTITY   64

/* the maximum number of characters allowed in a response (including the terminating null) */
#define MAX_RESPONSE 256

/* return codes for knowledge_get() and knowledge_put() */
#define KB_FOUND     0
#define KB_NOTFOUND -1
#define KB_INVALID  -2
#define KB_NOMEM    -3



struct intent {
	char entity[MAX_ENTITY];
	char response[MAX_RESPONSE];
	struct intent *next; // Pointer to the next node in the linked list.
};

typedef struct intent INTENT;

INTENT *what_ptr;
INTENT *where_ptr;
INTENT *who_ptr;

INTENT *head_what_ptr;
INTENT *head_where_ptr;
INTENT *head_who_ptr;


/* functions defined in main.c */
int compare_token(const char *token1, const char *token2);
void prompt_user(char *buf, int n, const char *format, ...);
void safe_strcat(char *dest, char *src[], size_t src_size, size_t n, int offset);
INTENT *create_node(const char *entity, const char *resp);
void add_linkedlist(INTENT *head, INTENT *node);
void update_linkedlist(INTENT *head, INTENT *node);
void free_linkedlist(INTENT *node);
int update_what_ptr(INTENT *new_node, char action[MAX_INTENT]);
int update_where_ptr(INTENT *new_node, char action[MAX_INTENT]);
int update_who_ptr(INTENT *new_node, char action[MAX_INTENT]);

/* functions defined in chatbot.c */
const char *chatbot_botname();
const char *chatbot_username();
int chatbot_main(int inc, char *inv[], char *response, int n);
int chatbot_is_exit(const char *intent);
int chatbot_do_exit(int inc, char *inv[], char *response, int n);
int chatbot_is_load(const char *intent);
int chatbot_do_load(int inc, char *inv[], char *response, int n);
int chatbot_is_question(const char *intent);
int chatbot_do_question(int inc, char *inv[], char *response, int n);
int chatbot_is_reset(const char *intent);
int chatbot_do_reset(int inc, char *inv[], char *response, int n);
int chatbot_is_save(const char *intent);
int chatbot_do_save(int inc, char *inv[], char *response, int n);
int chatbot_is_smalltalk(const char *intent);
int chatbot_do_smalltalk(int inc, char *inv[], char *resonse, int n);
int chatbot_is_delete(const char *intent);
int chatbot_do_delete(int inc, char *inv[], char *response, int n);
int chatbot_is_change(const char *intent);
int chatbot_do_change(int inc, char *inv[], char *response, int n);

/* functions defined in knowledge.c */
int knowledge_get(const char *intent, const char *entity, char *response, int n);
int knowledge_put(const char *intent, const char *entity, const char *response);
int knowledge_update(const char *intent, const char *entity, const char *response);
void knowledge_reset();
int knowledge_read(FILE *f);
void knowledge_write(FILE *f);


#endif
