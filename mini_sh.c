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
#define swap(T, a, b) {T t; t=a; a=b; b=t;}
#define inv_par(p) {if ((p).type == LEFT_PAR) (p).type = RIGHT_PAR; else if ((p).type == RIGHT_PAR) (p).type = LEFT_PAR;}

typedef struct _ExecUnit
{
	char* args[16];
	int type;
	int argv;
	int input;
	int output;
	int input_opposite;
	int output_opposite;
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

int build_postfix(ExecUnit* units, int unit_count, ExecUnit** postfix)
{
	Stack ust = {0,};
	int post_idx = 0;
	int par_count = 0;

	for (int i = 0; i < unit_count; ++i)
	{
		switch (units[i].type)
		{
		case PIPE:
			while (ust.size > 0 && typed_top(ust, ExecUnit)->type != LEFT_PAR)
				postfix[post_idx++] = pop(&ust);
			push(&ust, units+i);
			break;

		case LEFT_REDIR:
		case RIGHT_REDIR:
			while (ust.size > 0 && typed_top(ust, ExecUnit)->type != LEFT_PAR && 
					(typed_top(ust, ExecUnit)->type == LEFT_REDIR || typed_top(ust, ExecUnit)->type == RIGHT_REDIR))
				postfix[post_idx++] = pop(&ust);
			push(&ust, units+i);
			break;
		
		case LEFT_PAR:
			++par_count;
			push(&ust, units+i);
			break;
		case RIGHT_PAR:
			++par_count;
			while (typed_top(ust, ExecUnit)->type != LEFT_PAR)
				postfix[post_idx++] = pop(&ust);
			pop(&ust);
			break;
		case AMPERSAND:
			break;
		default:
			postfix[post_idx++] = units+i;
			break;
		}
	}

	while (ust.size > 0)
		postfix[post_idx++] = pop(&ust);
	
	return unit_count - par_count;
}

int build_prefix(ExecUnit* units, int unit_count, ExecUnit** prefix)
{
	int length;

	for (int i = 0; i < unit_count/2; ++i)
	{
		int j = unit_count - i - 1;
		swap(ExecUnit, units[i], units[j]);
		inv_par(units[i]);
		inv_par(units[j]);
	}

	length = build_postfix(units, unit_count, prefix);

	for (int i = 0; i < length/2; ++i)
	{
		int j = length - i - 1;
		swap(ExecUnit*, prefix[i], prefix[j]);
	}

	return length;
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

int set_pipe(ExecUnit** prefix, int length, int start, ExecUnit** input_end, ExecUnit** output_end, ExecUnit* piped[], int *piped_count)
{
	int fd[2];
	int file;
	ExecUnit *left_input_end = NULL;
	ExecUnit *left_output_end = NULL;
	ExecUnit *right_input_end = NULL;
	ExecUnit *right_output_end = NULL;
	int right_start, right_end;

	if (length <= start)
		return -1;

	if (strcmp(prefix[start]->args[0], "|") == 0)
	{
		if (pipe(fd) != 0)
		{
			fprintf(stderr, "minish : pipe error\n");
			return -1;
		}

		right_start = set_pipe(prefix, length, start+1, &left_input_end, &left_output_end, piped, piped_count);
		right_end = set_pipe(prefix, length, right_start, &right_input_end, &right_output_end, piped, piped_count);

		if (left_output_end != NULL)
		{
			left_output_end->output = fd[1];
			left_output_end->output_opposite = fd[0];
		}
		if (right_input_end != NULL)
		{
			right_input_end->input = fd[0];
			right_input_end->input_opposite = fd[1];
		}

		*input_end = left_input_end;
		*output_end = right_output_end;

		return right_end;
	}
	else if (strcmp(prefix[start]->args[0], ">") == 0)
	{
		right_start = set_pipe(prefix, length, start+1, &left_input_end, &left_output_end, piped, piped_count);
		file = open(prefix[right_start]->args[0], O_RDWR | O_CREAT | S_IROTH, 0644);

		if (file < 0)
		{
			fprintf(stderr, "minish : open error\n");
			return -1;
		}

		if (left_output_end != NULL)
		{
			left_output_end->output = file;
			left_output_end->output_opposite = 0;
		}

		*input_end = left_input_end;
		*output_end = NULL;

		return right_start + 1;
	}
	else if (strcmp(prefix[start]->args[0], "<") == 0)
	{
		right_start = set_pipe(prefix, length, start+1, &left_input_end, &left_output_end, piped, piped_count);
		file = open(prefix[right_start]->args[0], O_RDONLY);

		if (file < 0)
		{
			fprintf(stderr, "minish : open error\n");
			return -1;
		}

		if (left_input_end != NULL)
		{
			left_input_end->input = file;
			left_input_end->input_opposite = 0;
		}

		*input_end = NULL;
		*output_end = left_output_end;

		return right_start + 1;
	}
	else
	{
		piped[(*piped_count)++] = prefix[start];
		*input_end = prefix[start];
		*output_end = prefix[start];
		return start + 1;
	}
}

int start_process(ExecUnit* info, ExecUnit* piped[], int piped_count)
{
	int pid;

	if (strcmp(info->args[0], "quit") == 0 || strcmp(info->args[0], "exit") == 0)
		exit(0);
	else if (strcmp(info->args[0], "cd") == 0)
	{
		if (chdir(info->args[1]) != 0)
		{
			fprintf(stderr, "cd : %s\n", strerror(errno));
			return -1;
		}
		return 0;
	}
	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "minish : fork error\n");
		return -1;
	}
	else if (pid == 0) // Child Process
	{
		if (info->input_opposite != 0)
			close(info->input_opposite);
		if (info->output_opposite != 0)
			close(info->output_opposite);

		dup2(info->input, STDIN_FILENO);
		dup2(info->output, STDOUT_FILENO);

		execvp(*(info->args), info->args);
		fprintf(stderr, "minish : command not found\n");
		return -1;
	}

	if (info->input != STDIN_FILENO && info->input != 0)
		close(info->input);
	if (info->output != STDOUT_FILENO && info->output != 0)
		close(info->output);

	return pid;
}

/*
Redirection(<, >)는 Pipe(|)보다 우선순위가 높다.

(STDOUT) > (파일 경로)
(STDIN) < (파일 경로)
(STDOUT) | (STDIN)
*/
int execute(ExecUnit** prefix, int length, int how)
{
	int end, piped_count = 0;
	int pids[32] = {0,};
	ExecUnit* piped[32] = {0,};
	ExecUnit* input_end = NULL;
	ExecUnit* output_end = NULL;
	end = set_pipe(prefix, length, 0, &input_end, &output_end, piped, &piped_count);

	if (input_end != NULL)
		input_end->input = STDIN_FILENO;
	if (output_end != NULL)
		output_end->output = STDOUT_FILENO;

	for (int i = 0; i < piped_count; ++i)
		pids[i] = start_process(piped[i], piped, piped_count);

	if (how == BACKGROUND)
	{
		printf("[%d]\n", pids[0]);
		return 0;
	}
	for (int i = 0; i < piped_count; ++i)
		if (waitpid(pids[i], NULL, 0) < 0)
			if (errno != EINTR)
				return -1;
}

int parse_and_execute(char *input)
{
	char *arg[1024];
	char tokens[1024];

	ExecUnit units[32] = {0,};
	int unit_top = 0;
	int unit_count = 0;

	ExecUnit* prefix[32] = {0,};
	int post_idx = 0;

	int inputIdx = 0, tokenIdx = 0;
	int type, how;
	int quit = FALSE;
	int narg = 0;
	int finished = FALSE;

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
			break;
		case EOL:
		case AMPERSAND:
			unit_count = (units[unit_top].argv ? 1 : 0) + unit_top;
			int pf_length = build_prefix(units, unit_count, prefix);

			how = (type == AMPERSAND) ? BACKGROUND : FOREGROUND;
			execute(prefix, pf_length, how);

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
