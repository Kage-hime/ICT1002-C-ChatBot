/*
 * ICT1002 Assignment 2, 2018-19 Trimester 2.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "chat1002.h"


/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {
	return "Chatbot";
}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {
	return "Me";
}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {
	
	/* check for empty input */
	if (inc < 1) {
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else if (chatbot_is_delete(inv[0]))
		return chatbot_do_delete(inc, inv, response, n);
	else if (chatbot_is_change(inv[0]))
        return chatbot_do_change(inc, inv, response, n);
	else {
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {
	return (compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0);
}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {
	snprintf(response, n, "Have a nice day. Goodbye!");
	return 1;
}

/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {
	return compare_token(intent, "load") == 0;
}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {
	/*
		fp:		The file pointer.
		ctr:	The number of successful results retrieved from the file.
	*/
	char file_path[MAX_INPUT];

	if (inc <2) {
		snprintf(response, n, "Please specify the filename");
		return 0;
	}
	else {
		if (compare_token(inv[1], "from") == 0) {
			strcpy(file_path, inv[2]);
			strcat(file_path,".ini");
		}
		else {
			strcpy(file_path, inv[1]);
			strcat(file_path,".ini");
		}

		//check if file exists
		FILE *fp;
		if (fp = fopen(file_path, "r")) {
			int result = knowledge_read(fp);
			fclose(fp);
			if (result == KB_NOMEM) {
				snprintf(response, n, "Sorry, out of memory. Please try again later");
			}
			else {
				snprintf(response, n, "I have loaded '%s' file from my knowledge base", file_path);

			}
		}
		else {
			snprintf(response, n, "Sorry the file '%s' was not found", file_path);
		} 

		return 0;
	}
}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
	return (compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "who") == 0);
}


/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {
	/*
		unsure:		A string for unsure questions.
		enti:		A string to store the entity.
		ans:		A string to store the answer to the question.
	*/
	char unsure[MAX_RESPONSE] = "I don't know. ";
	char enti[MAX_ENTITY] = "";
	char ans[MAX_RESPONSE] = "";
	char question[MAX_RESPONSE] = "";
	int result;

	size_t offset = 1;
	
	// Check where does the question start.
	if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0) {
		offset = 2;
	}

	//safe_strcat(enti, inv, inc, (MAX_ENTITY - 1), offset);
	safe_strcat(enti, inv, inc, n, offset);
	//result = knowledge_get(inv[0], enti, response, n);
	result = knowledge_get(inv[0], enti, response, n);

	if (result == KB_NOTFOUND) {
		safe_strcat(unsure, inv, inc, (MAX_RESPONSE - 1), 0);
		strcat(unsure, "?");
		prompt_user(ans, MAX_RESPONSE, "%s", unsure);
		result = knowledge_put(inv[0], enti, ans);

		if (result == KB_FOUND) {
			snprintf(response, n, "Thank you.");
		}
	}
	
	return 0;
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {
	return compare_token(intent, "reset") == 0;
}


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
	knowledge_reset();
	snprintf(response, n, "My knowledge has been resetted.");	
	return 0;
}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {
	return compare_token(intent, "save") == 0;
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
	/*
		fp:		The file pointer.
	*/
	FILE *fp;
	char file_path[MAX_INPUT];

	if (inc < 2) {
		snprintf(response, n, "%s", "Please enter a filename to save to");
		return 0;
	}
	else {
		if (compare_token(inv[1], "to") == 0 || compare_token(inv[1], "as") == 0) {
			strcpy(file_path, inv[2]);
			strcat(file_path,".ini");
		}
		else {
			strcpy(file_path, inv[1]);
			strcat(file_path,".ini");
		}

		//check if file exists
		if (fp = fopen(file_path, "r")) {
			char overwrite[3];
			//ask if user wants to overwrite 
			prompt_user(overwrite, 3, "File already exist, do you wish to overwrite it? [y/n]");
			fclose(fp);

			if (compare_token(overwrite, "y") ==0) {
				fp = fopen(file_path, "w");
				knowledge_write(fp);
				fclose(fp);
				snprintf(response, n, "I have overwritten and save the file '%s' into my knowledge base", file_path);
			} 
			else if (compare_token(overwrite, "n") ==0) {
				snprintf(response, n, "File was not overwritten.");
			}
		}
		//else if file doesnt exist, create a new one
		else {
			fp = fopen(file_path, "w");
			knowledge_write(fp);
			fclose(fp);
			snprintf(response, n, "I have save the file '%s' into my knowledge base", file_path);
		}
	}
	return 0;
}
 
 
/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent) {
	return (compare_token(intent, "hello") == 0 
	|| compare_token(intent, "hey") == 0 
	|| compare_token(intent, "hi") == 0 
	|| compare_token(intent, "can") == 0 
	|| compare_token(intent, "should") ==0 
	|| compare_token(intent, "time") ==0 
	|| compare_token(intent, "riddle") ==0 
	|| compare_token(intent, "help") ==0);

}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n) {
	char *can_answers[3] = {"Yes", "No", "Maybe"};
	int random = rand()%3;
	char riddle_guess[MAX_RESPONSE];

	//if first word is "hello", "hey" or "hi"
	if (compare_token("hello", inv[0]) ==0 || compare_token("hey", inv[0]) ==0 || compare_token("hi", inv[0]) ==0) {
		snprintf(response, n, "Hello. It is nice to hear from you");
	}

	//if first word is "can" or "should"
	else if (compare_token("can", inv[0]) ==0 || compare_token("should", inv[0]) ==0) {
		snprintf(response, n, "%s", can_answers[random]);
	}

	//if first word is "time", display current time
	else if (compare_token("time", inv[0]) ==0) {
		time_t t;
    	time(&t);
		//time_t rawtime;
		//struct tm *timeinfo;
		//time(&rawtime);
		//timeinfo = localtime(&rawtime);
		snprintf(response, n, "Today Date: %s", ctime(&t));
	}

	//if first word is "riddle", prompt for riddle
	else if (compare_token("riddle", inv[0]) ==0) {
		switch (random) {
		case 0:
			prompt_user(riddle_guess, MAX_RESPONSE, "What can you hold without ever touching or using your hands?");
			if (compare_token("breath", riddle_guess) ==0 || compare_token("your breath", riddle_guess) ==0) {
				snprintf(response, n, "Yep that is correct");
			}
			else {
				snprintf(response, n, "Wrong answer");
			}
			break;

		case 1:
			prompt_user(riddle_guess, MAX_RESPONSE, "This belongs to you, but everyone else uses it more.");
			if (compare_token("your name", riddle_guess) ==0 || compare_token("my name", riddle_guess) ==0 || compare_token("name", riddle_guess) ==0) {
				snprintf(response, n, "Yep that is correct");
			}
			else {
				snprintf(response, n, "Wrong answer");
			}
			break;

		case 2:
			prompt_user(riddle_guess, MAX_RESPONSE, "Many have heard me, but nobody has seen me. I will not speak until spoken to first. What am I?");
			if (compare_token("echo", riddle_guess) ==0 || compare_token("an echo", riddle_guess) ==0) {
				snprintf(response, n, "Yep that is correct");
			}
			else {
				snprintf(response, n, "Wrong answer");
			}
			break;
		}
		
	}
	else if (compare_token("help", inv[0]) ==0) {
        snprintf(response, n, "List of commands: <what/where/who>, save, change, delete, time, riddle, load, reset");
    }
	
	return 0;
}

int chatbot_is_delete(const char *intent) {
	return compare_token(intent, "delete") ==0;
}

int chatbot_do_delete(int inc, char *inv[], char *response, int n) {
	/*
		fp:		The file pointer.
	*/
	FILE *fp;
	char file_path[MAX_INPUT];

	if (inc < 2) {
		snprintf(response, n, "%s", "Please enter a filename to delete from");
		return 0;
	}
	else {
		if (compare_token(inv[1], "from") ==0) {
			strcpy(file_path, inv[2]);
		}
		else {
			strcpy(file_path, inv[1]);
		}

		//check if file exist
		if (fp = fopen(file_path, "r")) {
			char confirm[3];
			fclose(fp);
			//ask if user wants to delete
			prompt_user(confirm, 3, "Are you sure you wish to delete this file? [y/n]");
			
			if (compare_token(confirm, "y") ==0) {
				//fp = fopen(file_path, "w");
				//fclose(fp);
				int del = remove(file_path);
				if (del ==0) {
					snprintf(response, n, "The file '%s' was deleted successfully", file_path);
				}
				else {
					snprintf(response, n, "Sorry. I am unable to delete the file '%s' from my knowledge base", file_path);
				}
			}

			else if (compare_token(confirm, "n") ==0) {
				snprintf(response, n, "File was not deleted.");
			}
			else {
				snprintf(response, n, "Sorry. I don't understand \"%s\".", confirm);
			}
		}
		//file doesnt exist. cannot delete
		else {
			snprintf(response, n, "Sorry. The file '%s' does not exist in my knowledge base", file_path);
		}
	}
	return 0;
}

int chatbot_is_change(const char *intent) {
    return compare_token(intent, "change") ==0;
}

int chatbot_do_change(int inc, char *inv[], char *response, int n) {
    char unsure[MAX_RESPONSE] = "";
    char enti[MAX_ENTITY] = "";
    char ans[MAX_RESPONSE] = "";
    int result;

    size_t offset = 2;
    
    if (compare_token(inv[2], "is") == 0 || compare_token(inv[2], "are") == 0) {
        offset = 3;
    }

    safe_strcat(enti, inv, inc, (MAX_ENTITY - 1), offset);
    result = knowledge_get(inv[1], enti, response, n);

    if (result == KB_FOUND) {
        safe_strcat(unsure, inv, inc, (MAX_RESPONSE - 1), 1);
        strcat(unsure, "?");
        prompt_user(ans, MAX_RESPONSE, "%s", unsure);
        result = knowledge_update(inv[1], enti, ans);
        snprintf(response, n, "Thank you."); 
    }
	else {
		snprintf(response, n, "");
	}

	return 0;
}
