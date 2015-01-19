#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tchar.h>
#include <wchar.h>
#include <Windows.h>
#include <locale.h>

#define SIZE 1000

wchar_t buff1[SIZE];
wchar_t buff2[SIZE];
wchar_t path[SIZE];

bool cd_back(wchar_t* path)
{
	wchar_t* last_backslash = wcsrchr(path, '\\');
	if (last_backslash)
	{
		*last_backslash = '\0';
		return true;
	}
	return false;
}

bool join_pathes(wchar_t* path1, wchar_t* path2)
{
	int path2_len = wcslen(path2);
	while (path2_len >= 3 && path2[0] == '.' && path2[1] == '.' && path2[2] == '\\')
	{
		if (!cd_back(path1))
			return false;
		path2 += 3;
		path2_len -= 3;
	}
	if (path2_len == 0)
		return true;
	if (path2_len >= 2 && path2[0] == '.' && path2[1] == '.')
		return cd_back(path1);
	if (wcschr(path2, ':') != NULL)
		path1[0] = '\0';
	if (wcslen(path1) > 0)
		wcscat(path1, _T("\\"));
	wcscat(path1, path2);
	if (path1[wcslen(path1) - 1] == '\\')
		path1[wcslen(path1) - 1] = '\0';
	return true;
}

void cd()
{
	wchar_t* parameter = wcstok(NULL, _T(""));
	if (parameter == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wcscpy(buff1, path);
	if (!join_pathes(path, parameter) || _waccess(path, 0))
	{
		wprintf(_T("wrong path\n"));
		wcscpy(path, buff1);
	}
}

void ls()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("<empty directory>\n");
		return;
	}
	do
	printf("%s %d\n", FindFileData.cFileName, FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	while (FindNextFile(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

void mk()
{
	wchar_t* parameter = wcstok(NULL, _T(""));
	if (parameter == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wcscpy(buff1, path);
	if (!join_pathes(buff1, parameter))
	{
		wprintf(_T("can't create the file, check its path\n"));
		return;
	}
	FILE*file = _wfopen(buff1, _T("a"));
	if (file == NULL)
		wprintf(_T("can't create the file, check its name and access\n"));
	else
		fclose(file);
}

void rm()
{
	wchar_t* parameter = wcstok(NULL, _T(""));
	if (parameter == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wcscpy(buff1, path);
	if (!join_pathes(buff1, parameter))
	{
		wprintf(_T("can't remove the file, check its path\n"));
		return;
	}
	if (_wunlink(buff1) == -1)
	{
		wprintf(_T("can't remove the file, check its name and access\n"));
	}
}

void cp()
{
	wchar_t* parameters = wcstok(NULL, _T(""));
	if (parameters == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wchar_t* delim;
	if (wcschr(parameters, '\"') != NULL)
		delim = _T("\"");
	else
		delim = _T(" ");
	wchar_t* parameter1 = wcstok(parameters, delim);
	if (parameter1 == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wchar_t* parameter2 = wcstok(NULL, _T(""));
	if (parameter2 == NULL)
	{
		wprintf(_T("the command need more parameters\n"));
		return;
	}
	wcscpy(buff1, path);
	if (!join_pathes(buff1, parameter1))
	{
		wprintf(_T("can't copy the file, check its path\n"));
		return;
	}
	wcscpy(buff2, path);
	if (!join_pathes(buff2, parameter2))
	{
		wprintf(_T("can't copy the file, check the path of destination\n"));
		return;
	}
	if (!CopyFile(buff1, buff2, false))
	{
		wprintf(_T("can't copy the file, check parameters names and access\n"));
	}
}

int  main()
{

	setlocale(LC_ALL, "russian_russia.866");
	wchar_t input[SIZE];
	wcscpy(path, _T("c:"));
	while (true)
	{
		wprintf(_T("%s> "), path);
		_getws(input);
		wchar_t* command = wcstok(input, _T(" "));
		if (command == NULL)
			continue;
		if (wcscmp(command, _T("exit")) == 0)
			break;
		if (wcscmp(command, _T("cd")) == 0)
			cd();
		else if (wcscmp(command, _T("..")) == 0)
		{
			if (!cd_back(path))
				wprintf(_T("can't go over the root directory\n"));
		}
		else if (wcscmp(command, _T("ls")) == 0)
			ls();
		else if (wcscmp(command, _T("rm")) == 0)
			rm();
		else if (wcscmp(command, _T("mk")) == 0)
			mk();
		else if (wcscmp(command, _T("cp")) == 0)
			cp();
		else if (wcscmp(command, _T("help")) == 0)
		{
			wprintf(_T("Usage examples:\n"));
			wprintf(_T("cd ..    the same as    cd ..\\\n"));
			wprintf(_T("cd ..\\.. and similar commands\n"));
			wprintf(_T("cd directory    the same as    cd directory\\\n"));
			wprintf(_T("cd dir1\\dir2\n"));
			wprintf(_T("cd ..\\..\\dir1\\dir2\n\n"));
			wprintf(_T("ls\n\n"));
			wprintf(_T("rm file\n"));
			wprintf(_T("rm dir\\file\n"));
			wprintf(_T("rm ..\\dir\\file\n\n"));
			wprintf(_T("mk file\n"));
			wprintf(_T("mk ..\\dir\\file\n\n"));
			wprintf(_T("cp file1 file2\n"));
			wprintf(_T("cp ..\\file1 ..\\dir\\file2\n"));
			wprintf(_T("cp file_with_spaces\"file2\n"));
			wprintf(_T("cp ..\\dir_with_spaces\\file_with_spaces\"dir\\file2\n\n"));
		}
		else
			wprintf(_T("wrong command\n"));
	}
	return 1;
}