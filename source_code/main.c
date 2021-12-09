/*
 * ICT1002 Assignment 2, 2018-19 Trimester 2.
 *
 * This file implements the main loop, including dividing input into words.
 *
 * You should not need to modify this file. You may invoke its functions if you like, however.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chat1002.h"
#include "chatbot.c"
#include "knowledge.c"

/* word delimiters */
const char *delimiters = " ?\t\n";


/*
 * Main loop.
 */
int main(int argc, char *argv[]) {
	char input[MAX_INPUT];      /* buffer for holding the user input */
	int inc;                    /* the number of words in the user input */
	char *inv[MAX_INPUT];       /* pointers to the beginning of each word of input */
	char output[MAX_RESPONSE];  /* the chatbot's output */
	int len;                    /* length of a word */
	int done = 0;               /* set to 1 to end the main loop */
	
	/* initialise the chatbot */
	inv[0] = "reset";
	inv[1] = NULL;
	chatbot_do_reset(1, inv, output, MAX_RESPONSE);
	
	/* print a welcome message */
	printf("%s: Hello, I'm %s.\n", chatbot_botname(), chatbot_botname());
	
	/* main command loop */
	do {
	
		do {
			/* read the line */
			printf("%s: ", chatbot_username());
			fgets(input, MAX_INPUT, stdin);

			/**
			* [main.c] MODIFICATION
			* If no newline is found, user input is larger
			* than the buffer.
			*/
			// Find newline and get the pointer to it.
			char *nl = strchr(input, '\n');
			if (nl != NULL) {
				// newline found, replace it with null terminator.
				*nl = '\0';
			} else {
				// Clear the remaining input.
				int c;
				while ((c = getchar()) != '\n' && c != EOF) {
					continue;
				}
			}
			/* End of modification. */
			
			/* split it into words */
			inc = 0;
			inv[inc] = strtok(input, delimiters);
			while (inv[inc] != NULL) {
				
				/* remove trailing punctuation */
				len = strlen(inv[inc]);
				while (len > 0 && ispunct(inv[inc][len - 1])) {
					inv[inc][len - 1] = '\0';
					len--;
				}
				
				/* go to the next word */
				inc++;
				inv[inc] = strtok(NULL, delimiters);
			}
		} while (inc < 1);
		
		/* invoke the chatbot */
		done = chatbot_main(inc, inv, output, MAX_RESPONSE);
		printf("%s: %s\n", chatbot_botname(), output);
	} while (!done);
	
	return 0;
}


/*
 * Utility function for comparing string case-insensitively.
 *
 * Input:
 *   token1 - the first token
 *   token2 - the second token
 *
 * Returns:
 *   as strcmp()
 */
int compare_token(const char *token1, const char *token2) {
	int i = 0;
	while (token1[i] != '\0' && token2[i] != '\0') {
		if (toupper(token1[i]) < toupper(token2[i]))
			return -1;
		else if (toupper(token1[i]) > toupper(token2[i]))
			return 1;
		i++;
	}
	
	if (token1[i] == '\0' && token2[i] == '\0')
		return 0;
	else if (token1[i] == '\0')
		return -1;
	else
		return 1;
}


/*
 * Prompt the user.
 *
 * Input:
 *   buf    - a buffer into which to store the answer
 *   n      - the maximum number of characters to write to the buffer
 *   format - format string, as printf
 *   ...    - as printf
 */
void prompt_user(char *buf, int n, const char *format, ...) {
	/* print the prompt */
	va_list args;
	va_start(args, format);
	printf("%s: ", chatbot_botname());
	vprintf(format, args);
	printf(" ");
	va_end(args);
	printf("\n%s: ", chatbot_username());
	
	/* get the response from the user */
	fgets(buf, n, stdin);

	/**
	* [main.c] MODIFICATION
	* If no newline is found, user input is larger
	* than the buffer.
	*/
	// Find newline and get the pointer to it.
	char *nl = strchr(buf, '\n');
	if (nl != NULL) {
		// newline found, replace it with null terminator.
		*nl = '\0';
	} else {
		// Clear the remaining input.
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {
			continue;
		}
	}
	/* End of modification. */
}

void safe_strcat(char *dest, char *src[], size_t src_size, size_t dest_size, int offset) {
	for (int i = offset; i < src_size; i++){
		// Check length
		if (strlen(dest) < dest_size + 2) {
			strcat(dest, src[i]);
			// Check if last element
			if (i != src_size - 1){
				strcat(dest, " ");
			}
		}
		else {
			strncat(dest, src[i], dest_size - strlen(dest) - 1);
		}
	}
}


INTENT * create_node(const char *entity, const char *response) {

	INTENT *node = malloc(sizeof(INTENT));

	if (node == NULL) {
		return NULL;
	}

	snprintf(node->entity, MAX_ENTITY, "%s", entity);
	snprintf(node->response, MAX_RESPONSE, "%s", response);
	node->next = NULL;

	return node;
}


void add_linkedlist(INTENT *head, INTENT *node) {

	INTENT *current = head;

	while (current != NULL) {
		if (compare_token(current->entity, node->entity) == 0) {
			snprintf(current->response, MAX_RESPONSE, "%s", node->response);
			break;
		}
		else if (current->next == NULL) {
			current->next = node;
			break;
		}
		current = current->next;
	}
}


void free_linkedlist(INTENT *node) {

	INTENT *current;

	while (node != NULL) {
		current = node;
		node = node->next;
		free(current);
	}
}

int update_what_ptr(INTENT *new_node, char action[MAX_INTENT]) {

    if (new_node == NULL) {
        return KB_NOMEM;
    }
    else {
        if (head_what_ptr == NULL) {
			head_what_ptr = new_node;
            what_ptr = new_node;
        }
        else if (compare_token(action, "add")==0){
            add_linkedlist(what_ptr, new_node);
        } 
        else if (compare_token(action, "update")==0) {
            update_linkedlist(what_ptr, new_node);
        }
        return KB_FOUND;
    }
}


int update_where_ptr(INTENT *new_node, char action[MAX_INTENT]) {

    if (new_node == NULL) {
        // Failed to allocate memory.
        return KB_NOMEM;
    }
    else {
        if (where_ptr == NULL) {
			head_where_ptr = new_node;
            where_ptr = new_node;
        }
        else if (compare_token(action, "add")==0){
            add_linkedlist(where_ptr, new_node);
        } 
        else if (compare_token(action, "update")==0) {
            update_linkedlist(where_ptr, new_node);
        }
        return KB_FOUND;
    }
}

int update_who_ptr(INTENT *new_node, char action[MAX_INTENT]) {

    if (new_node == NULL) {
        return KB_NOMEM;
    }
    else {
        if (who_ptr == NULL) {
			head_who_ptr = new_node;
            who_ptr = new_node;
        }
        else if (compare_token(action, "add") ==0){
            add_linkedlist(who_ptr, new_node);
        } 
        else if (compare_token(action, "update")==0) {
            update_linkedlist(who_ptr, new_node);
        }
        return KB_FOUND;
    }
}

void update_linkedlist(INTENT *head, INTENT *node) {

    INTENT *temp = head;

    while (temp != NULL) {
        /* If replacing the temp->entity is too hard because strings is stupid in C, just use this lol, but rip garbage cleaning.
        if (compare_token(temp->next->entity, node->entity) == 0) {
            node->next = temp->next->next;
            temp->next = node;
        } else if (temp->next == NULL) {
            snprintf(temp->response, MAX_RESPONSE, "Question doesn't exist!", node->response);
            break;
        }*/
        if (compare_token(temp->entity, node->entity) == 0) {
            strcpy(temp->response, node->response);
        } else if (temp->next == NULL) {
            //snprintf(temp->response, MAX_RESPONSE, "Question doesn't exist!", node->response);
            break;
        }
        temp = temp->next;
    }
}
