#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 b8;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float r32;
typedef double r64;

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
_ReadFile(char* Input, u32 Length) {
	DWORD NumRead;
	if (!ReadFile(hStdin, Input, Length, &NumRead, NULL)) ErrorExit("ReadFile");
}

void
GetInputLine(char** Input) {
	_ReadFile(*Input, BUFFER_LENGTH);
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

#if 0
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
#else

// Lifted from http://www.cs.princeton.edu/courses/archive/spring09/cos333/beautiful.html
b8 match(char *regexp, char *text);
b8 matchhere(char *regexp, char *text);
b8 matchstar(int c, char *regexp, char *text);

/* match: search for regexp anywhere in text */
b8
match(char *regexp, char *text)
{
	if (regexp[0] == '^')
		return matchhere(regexp+1, text);
	do {    /* must look even if string is empty */
		if (matchhere(regexp, text))
			return 1;
	} while (*text++ != '\0');
	return 0;
}

/* matchhere: search for regexp at beginning of text */
b8
matchhere(char *regexp, char *text)
{
	if (regexp[0] == '\0') return 1;
	if (regexp[1] == '*') return matchstar(regexp[0], regexp+2, text);
	if (regexp[0] == '$' && regexp[1] == '\0') return *text == '\0';
	if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text)) return matchhere(regexp+1, text+1);
	return 0;
}

/* matchstar: search for c*regexp at beginning of text */
b8
matchstar(int c, char *regexp, char *text)
{
	do {    /* a * matches zero or more instances */
		if (matchhere(regexp, text)) return 1;
	} while (*text != '\0' && (*text++ == c || c == '.'));
	return 0;
}

b8
MatchPattern(char* Pattern, char* Input, char* Value) {
	return match(Pattern, Input) == 1;
}

char*
CreateResponse(char* Response, char* Value) {
	return Response;
}

char*
MatchResponses(char* Input, response* Responses) {
	char Buffer[256];
	for (u32 ResponseIndex = 0; ResponseIndex < 5; ++ResponseIndex) {
		response* Response = Responses + ResponseIndex;
		if (MatchPattern(Response->Pattern, Input, Buffer)) {
			return CreateResponse(Response->Response, Buffer);
		}
	}

	return NULL;
}

#endif

response Responses[] = {
	{"Hello", "Hello"},
	{"My name is (.*) of (.*)", "Nice to meet you \1 of \2"},
	{"My name is (.*)", "Nice to meet you \1"},
	{"Goodbye", "Goodbye"},
	{"", "I don't understand"}
};

char*
ParseInput(char* Input) {
	TrimInputLine(Input);

#if 0
	char* Tokens = (char*)malloc(16 * 256 * sizeof(*Tokens));
	TokenizeInput(&Tokens, Input);

	return MatchTokens(&Tokens, Responses);
#else
	return MatchResponses(Input, Responses);
#endif
}

#define TEST(String) do { \
	char* Resp = ParseInput(String); \
	printf("%s: %s\n", String, Resp); \
	} while (0)

int
main(void) {
	InitializeHandles();

#if 0
	char* Input = (char*)malloc(BUFFER_LENGTH * sizeof(*Input));
	GetInputLine(&Input);
	char* Response = ParseInput(Input);
	printf("%s\n", Response);
#endif

	TEST("Hello");
	TEST("Goodbye");
	TEST("My name is Trowa");
	TEST("My name is Trowa of Earth");
	return 0;
}

