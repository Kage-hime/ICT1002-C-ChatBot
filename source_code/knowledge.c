/*
 * ICT1002 Assignment 2, 2018-19 Trimester 2.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * kowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1002.h"

INTENT *what_ptr;
INTENT *where_ptr;
INTENT *who_ptr;

INTENT *head_what_ptr;
INTENT *head_where_ptr;
INTENT *head_who_ptr;

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_FOUND, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {
	INTENT *current;

	if (compare_token(intent, "what") == 0) {
		current = head_what_ptr;
	} 
	else if (compare_token(intent, "where") == 0) {
		current = head_where_ptr;
	} 
	else if (compare_token(intent, "who") == 0) {
		current = head_who_ptr;
	} 
	else {
		return KB_INVALID;
	}

	while(current != NULL){

		if (compare_token(entity, current->entity) == 0){
			snprintf(response, n, "%s" , current->response);
			return KB_FOUND;
		}
		current = current->next;
	}

	return KB_NOTFOUND;
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {
    int result;

    if (compare_token(intent, "what") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_what_ptr(new_node_ptr, "add");
    } 
    else if (compare_token(intent, "where") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_where_ptr(new_node_ptr, "add");
    } 
    else if (compare_token(intent, "who") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_who_ptr(new_node_ptr, "add");
    } 
    else {
        result = KB_INVALID;
    }

    return result;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
	/*
		ctr:	The number of successful results retrieved from the file.
		result:	The result of inserting the entity-response into the knowledge base.
		line:	The buffer to store content of the file.
	*/
	int ctr = 0, result;
	char line[MAX_INPUT];
	char *f_intent;

	const size_t MAX_READLEN = (size_t)(MAX_ENTITY + MAX_RESPONSE);

	while (fgets(line, MAX_READLEN, (FILE*) f)) {
		char *f_entity, *f_response;
		char *cr, *nl;

		// Read line by line.
		if (
			strcmp(line, "\n") == 0 ||
			strcmp(line, "\r\n") == 0
		) {
			// Empty line.
			continue;
		} else {
			cr = strchr(line, '\r');
			nl = strchr(line, '\n');

			if (cr != NULL) {
				// Carriage return found, replace it with null terminator.
				*cr = '\0';
			} else if (nl != NULL) {
				// newline found, replace it with null terminator.
				*nl = '\0';
			} else {
				// Clear any remaining input to prevent overflow.
				int c;
				while ((c = getchar()) != '\n' && c != EOF) {
					continue;
				}
			}

			if (
				strchr(line, '[') != NULL &&
				strchr(line, ']') != NULL
			) {
				// Square brackets found. Check intent.
				if (compare_token(line, "[what]") == 0) {
					// Intent: what.
					f_intent = "what";
				} else if (compare_token(line, "[where]") == 0) {
					// Intent: where.
					f_intent = "where";
				} else if (compare_token(line, "[who]") == 0) {
					// Intent: who.
					f_intent = "who";
				} else {
					// Invalid intent.
					f_intent = NULL;
				}
			} else if (
				f_intent != NULL &&
				strchr(line, '=') != NULL
			) {
				// Entity-Response pair line.
				f_entity = strtok(line, "=");
				f_response = strtok(NULL, "=");
				result = knowledge_put(f_intent, f_entity, f_response);

				if (result == KB_FOUND) {
					// Increment the successful counter.
					ctr++;
				}
			}
		}
	}

	return ctr;
}


/*
 * Reset the knowledge base, removing all know entities from all intents.
 */
void knowledge_reset() {
	free_linkedlist(what_ptr);
	free_linkedlist(where_ptr);
	free_linkedlist(who_ptr);

	what_ptr = NULL;
	head_what_ptr = NULL;
	where_ptr = NULL;
	head_where_ptr = NULL;
	who_ptr = NULL;
	head_who_ptr = NULL;
}
/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {
	// Save "what" intent's entitiy-responses.
	fprintf(f, "[what]\n");
	what_ptr = head_what_ptr;
	while (what_ptr != NULL) {
		fprintf(f, "%s=%s\n", what_ptr->entity, what_ptr->response);
		what_ptr = what_ptr->next;
	}
	what_ptr = head_what_ptr;

	// Save "where" intent's entitiy-responses.
	fprintf(f, "\n[where]\n");
	where_ptr = head_where_ptr;
	while (where_ptr != NULL) {
		fprintf(f, "%s=%s\n", where_ptr->entity, where_ptr->response);
		where_ptr = where_ptr->next;
	}
	where_ptr = head_where_ptr;

	// Save "what" intent's entitiy-responses.
	fprintf(f, "\n[who]\n");
	who_ptr = head_who_ptr;
	while (who_ptr != NULL) {
		fprintf(f, "%s=%s\n", who_ptr->entity, who_ptr->response);
		who_ptr = who_ptr->next;
	}
	who_ptr = head_who_ptr;
}

int knowledge_update(const char *intent, const char *entity, const char *response) {
    int result;

    if (compare_token(intent, "what") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_what_ptr(new_node_ptr, "update");
    } 
    else if (compare_token(intent, "where") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_where_ptr(new_node_ptr, "update");
    } 
    else if (compare_token(intent, "who") == 0) {
        INTENT *new_node_ptr = create_node(entity, response);
        result = update_who_ptr(new_node_ptr, "update");
    } 
    else {
        result = KB_INVALID;
    }

    return result;
}