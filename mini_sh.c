#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FALSE 0
#define TRUE 1

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define LEFT_REDIR 4
#define RIGHT_REDIR 5
#define PIPE 6
#define LEFT_PAR 7
#define RIGHT_PAR 8

#define FOREGROUND 0
#define BACKGROUND 1

#define typed_top(st, T) ((T*)get_top(&(st)))

typedef struct _ExecUnit
{
	char* args[16];
	int type;
	int argv;
	int executed;
	int input;
	int output;
} ExecUnit;

typedef struct _Stack
{
	void* items[32];
	int size;
} Stack;

void push(Stack* st, void* item)
{
	st->items[st->size++] = item;
}

void* get_top(Stack* st)
{
	return st->items[st->size-1];
}

void* pop(Stack* st)
{
	return st->items[(st->size--)-1];
}

int get_token(char **outptr, char* tokens, char* input, int* tokenIdx, int* inputIdx)
{
	int type;

	*outptr = tokens + *tokenIdx;
	while ((input[*inputIdx] == ' ') || (input[*inputIdx] == '\t'))
		(*inputIdx)++;

	tokens[(*tokenIdx)++] = input[*inputIdx];

	switch (input[(*inputIdx)++])
	{
	case '\0':
		type = EOL;
		break;
	case '&':
		type = AMPERSAND;
		break;
	case '<':
		type = LEFT_REDIR;
		break;
	case '>':
		type = RIGHT_REDIR;
		break;
	case '|':
		type = PIPE;
		break;
	case '(':
		type = LEFT_PAR;
		break;
	case ')':
		type = RIGHT_PAR;
		break;
	default:
		type = ARG;
		while ((input[*inputIdx] != ' ') && (input[*inputIdx] != '&') && (input[*inputIdx] != '|') &&
			   (input[*inputIdx] != '\t') && (input[*inputIdx] != '\0') &&
			   (input[*inputIdx] != '<') && (input[*inputIdx] != '>') &&
			   (input[*inputIdx] != '(') && (input[*inputIdx] != ')'))
			tokens[(*tokenIdx)++] = input[(*inputIdx)++];
	}
	tokens[(*tokenIdx)++] = '\0';
	return (type);
}

int start_process(ExecUnit* info)
{
}

/*
Redirection(<, >)는 Pipe(|)보다 우선순위가 높다.

(STDOUT) > (파일 경로)
(STDIN) < (파일 경로)
(STDOUT) | (STDIN)
*/
int execute(ExecUnit** postfix, int length, int how)
{
	
}

int parse_and_execute(char *input)
{
	char *arg[1024];
	char tokens[1024];

	ExecUnit units[32] = {0,};
	int unit_top = 0;
	int unit_count = 0;

	Stack ust = {0,};

	ExecUnit* postfix[32] = {0,};
	int postIdx = 0;

	int inputIdx = 0, tokenIdx = 0;
	int type, how;
	int quit = FALSE;
	int narg = 0;
	int finished = FALSE;
	int par_count = 0;

	while (!finished)
	{
		type = get_token(&arg[narg], tokens, input, &tokenIdx, &inputIdx);

		switch (type)
		{
		case ARG:
			units[unit_top].type = ARG;
			units[unit_top].args[units[unit_top].argv++] = arg[narg++];
			break;
		case RIGHT_REDIR:
		case LEFT_REDIR:
		case PIPE:
		case RIGHT_PAR:
		case LEFT_PAR:
			if (units[unit_top].argv)
				++unit_top;
			units[unit_top].type = type;
			units[unit_top].args[units[unit_top].argv++] = arg[narg++];
			++unit_top;
			if (type == RIGHT_PAR || type == LEFT_PAR)
				++par_count;
			break;
		case EOL:
		case AMPERSAND:
			unit_count = (units[unit_top].argv ? 1 : 0) + unit_top;
			for (int i = 0; i < unit_count; ++i)
			{
				switch (units[i].type)
				{
				case PIPE:
					while (ust.size > 0 && typed_top(ust, ExecUnit)->type != LEFT_PAR)
						postfix[postIdx++] = pop(&ust);
					push(&ust, units+i);
					break;

				case LEFT_REDIR:
				case RIGHT_REDIR:
					while (ust.size > 0 && typed_top(ust, ExecUnit)->type != LEFT_PAR && 
							(typed_top(ust, ExecUnit)->type == LEFT_REDIR || typed_top(ust, ExecUnit)->type == RIGHT_REDIR))
						postfix[postIdx++] = pop(&ust);
					push(&ust, units+i);
					break;
				
				case LEFT_PAR:
					push(&ust, units+i);
					break;
				case RIGHT_PAR:
					while (typed_top(ust, ExecUnit)->type != LEFT_PAR)
						postfix[postIdx++] = pop(&ust);
					pop(&ust);
					break;
				case AMPERSAND:
					break;
				default:
					postfix[postIdx++] = units+i;
					break;
				}
			}

			while (ust.size > 0)
				postfix[postIdx++] = pop(&ust);

			int pf_length = unit_count - par_count;
			// for (int i = 0; i < pf_length; ++i)
			// {
			// 	printf("postfix #%d: ", i+1);
			// 	for (int j = 0; j < postfix[i]->argv; ++j)
			// 		printf("<%s> ", postfix[i]->args[j]);
			// 	printf("\n");
			// }

			how = (type == AMPERSAND) ? BACKGROUND : FOREGROUND;
			execute(postfix, pf_length, how);

			narg = 0;
			finished = TRUE;
			break;
		}
	}
	return quit;
}

int main()
{
	char *arg[1024];
	char path[1024];
	char input[512];
	int quit;
	
	printf("msh:%s # ", getcwd(path, 1024));
	while (fgets(input, 512, stdin))
	{
		input[strlen(input)-1] = '\0';
		quit = parse_and_execute(input);
		if (quit)
			break;
		printf("msh:%s # ", getcwd(path, 1024));
	}
}
