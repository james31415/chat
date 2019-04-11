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

struct string {
	char* String;
	u32 Length;
};

void
CopyString(string* String, char* Source) {
	u32 Length = (u32)strlen(Source);
	String->String = (char*)malloc(Length * sizeof(Source));
	String->Length = Length;

	strcpy(String->String, Source);
}

#define BUFFER_LENGTH 256

void
GetInputLine(string* Input) {
	Input->String = (char*)malloc(BUFFER_LENGTH * sizeof(char));
	Input->Length = BUFFER_LENGTH;

	DWORD NumRead;
	if (!ReadConsole(hStdin, Input->String, Input->Length, &NumRead, NULL)) ErrorExit("ReadConsole");
}

void
TrimInputLine(string* Input) {
	u32 Index = 0;
	while (Index < Input->Length) {
		if (Input->String[Index] == '\r') {
			Input->String[Index] = '\0';
			break;
		} else if (Input->String[Index] == '\n') {
			Input->String[Index] = '\0';
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
TokenizeInput(string Tokens[16], string* Input) {
	u32 TokenIndex = 0;
	char* token = strtok(Input->String, ", "); 
	while (token) {
		CopyString(&Tokens[TokenIndex], token);
		++TokenIndex;

		token = strtok(NULL, ", ");
	}
}

char*
MatchTokens(string Tokens[16], response Responses[]) {
	for (int ResponseIndex = 0; ResponseIndex < 3; ++ResponseIndex) {
		if (strcmp(Responses[ResponseIndex].Pattern, Tokens[0].String) == 0) {
			return Responses[ResponseIndex].Response;
		}
	}

	return NULL;
}

char*
ParseInput(string* Input) {
	response Responses[] = {
		{"Hello", "Hello"},
		{"Goodbye", "Goodbye"},
		{"", "I don't understand"}
	};

	TrimInputLine(Input);

	string Tokens[16];
	TokenizeInput(Tokens, Input);

	return MatchTokens(Tokens, Responses);
}

int
main(void) {
	InitializeHandles();

	string Input;
	GetInputLine(&Input);
	char* Response = ParseInput(&Input);
	printf("%s\n", Response);

	return 0;
}

