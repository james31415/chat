#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

typedef uint32_t u32;

typedef int32_t s32;

HANDLE hStdin;
HANDLE hStdout;

#define DisplayMessage(Message) _DisplayMessage(Message, (u32)strlen(Message))
void
_DisplayMessage(const char* Message, u32 MessageLength) {
	DWORD CharsWritten;
	WriteConsole(hStdout, Message, MessageLength, &CharsWritten, NULL); 
}

void
ErrorExit(const char* Message) {
	DisplayMessage(Message);

	ExitProcess(0);
}

void
InitializeHandles() {
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE) ExitProcess(1);

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE) ErrorExit("GetStdHandle");
}

#define BUFFER_LENGTH 256

void
_ReadConsole(char* Input, u32 Length) {
	DWORD NumRead;
	if (!ReadConsole(hStdin, Input, Length, &NumRead, NULL)) ErrorExit("ReadConsole");
}

void
GetInputLine(char** Input) {
	_ReadConsole(*Input, BUFFER_LENGTH);
}

#define TrimInputLine(Input) _TrimInputLine(Input, (u32)strlen(Input))
void
_TrimInputLine(char* Input, u32 Length) {
	u32 Index = 0;
	while (Index < Length) {
		if (Input[Index] == '\r') {
			Input[Index] = '\0';
			break;
		} else if (Input[Index] == '\n') {
			Input[Index] = '\0';
			break;
		}

		++Index;
	}
}

struct response {
	char* Pattern;
	char* Response;
};

void
TokenizeInput(char** Tokens, char* Input) {
	u32 TokenIndex = 0;
	char* token = strtok(Input, ", "); 
	while (token) {
		strcpy(Tokens[TokenIndex], token);
		++TokenIndex;

		token = strtok(NULL, ", ");
	}
}

char*
MatchTokens(char** Tokens, response* Responses) {
	for (int ResponseIndex = 0; ResponseIndex < 3; ++ResponseIndex) {
		if (strcmp(Responses[ResponseIndex].Pattern, Tokens[0]) == 0) {
			return Responses[ResponseIndex].Response;
		}
	}

	return NULL;
}

response Responses[] = {
	{"Hello", "Hello"},
	{"My name is (.*)", "Nice to meet you \1"},
	{"Goodbye", "Goodbye"},
	{"", "I don't understand"}
};

char*
ParseInput(char* Input) {
	TrimInputLine(Input);

	char* Tokens = (char*)malloc(16 * 256 * sizeof(*Tokens));
	TokenizeInput(&Tokens, Input);

	return MatchTokens(&Tokens, Responses);
}

int
main(void) {
	InitializeHandles();

	char* Input = (char*)malloc(BUFFER_LENGTH * sizeof(*Input));
	GetInputLine(&Input);
	char* Response = ParseInput(Input);
	printf("%s\n", Response);

	return 0;
}

