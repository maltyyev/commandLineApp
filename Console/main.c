#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#define SIZE 1000

char back_up_path[SIZE];
char path[SIZE];
char input[SIZE];

void cd_back()
{
	if (strlen(path) == 0)
	{
		printf("you are in the root directory\n");
		return;
	}
	char* last_backslash = strrchr(path, '\\');
	if (last_backslash)
		*last_backslash = '\0';
	else
		path[0] = '\0';
}

void cd()
{
	char* parameter = strtok(NULL, "");
	if (parameter == NULL)
		return;
	int parameter_len = strlen(parameter);
	while (parameter_len >= 3 && parameter[0] == '.' && parameter[1] == '.' && parameter[2] == '\\')
	{
		cd_back();
		parameter += 3;
		parameter_len -= 3;
	}
	if (parameter_len == 0)
		return;
	if (parameter_len >= 2 && parameter[0] == '.' && parameter[1] == '.')
	{
		cd_back();
		return;
	}
	strcpy(back_up_path, path);
	if (strrchr(parameter, ':') != NULL)
		path[0] = '\0';
	if (strlen(path) > 0)
		strcat(path, "\\");
	strcat(path, parameter);
	if (path[strlen(path) - 1] == '\\')
		path[strlen(path) - 1] = '\0';
	if (access(path, 0) != 0)
	{
		printf("wrong path to directory\n");
		strcpy(path, back_up_path);
		return;
	}
}

int  main()
{
	strcpy(path, "c:");
	while (true)
	{
		printf("%s> ", path);
		gets(input);
		char* command = strtok(input, " ");
		if (command == NULL)
			continue;
		if (strcmp(command, "exit") == 0)
			break;
		if (strcmp(command, "cd") == 0)
		{
			cd();
			continue;
		}
		printf("wrong command\n");
	}
	return 1;
}