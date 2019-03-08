#include "pch.h"
#include "vp_shell.h"

static void welcome();
static void shell_help(unsigned int);
static unsigned int BKDRHash(char *);
static void shell_run(struct _CONFIG *);
static void show_config(struct _CONFIG *);
static void set_config(struct _CONFIG *, char *);

static void welcome()
{
	printf("   _____                                \n");
	printf("  / ___/_________  __  ___________ ____ \n");
	printf("  \\__ \\/ ___/ __ \\/ / / / ___/ __ `/ _ \\\n");
	printf(" ___/ / /__/ /_/ / /_/ / /  / /_/ /  __/\n");
	printf("/____/\\___/\\____/\\__,_/_/   \\__, /\\___/ \n");
	printf("                           /____/     \tversion:%s\n", VP_VERSION);

}

static void show_config(struct _CONFIG * conf)
{
// "|----------+---|"
// "| target |   |"
// "| path     |   |"
// "| type     |   |"
// "|          |   |"
// "|----------+---|"
}

static void shell_help(unsigned int key)
{
	if (key == 0)
	{
		printf("Core Commands\n=============\n\n");
		printf("\tCommand\t\tDescription\n");
		printf("\thelp\n");
		printf("\tset\n");
		printf("\tshow\n");
		printf("\tinfo\n");
	}
	if (key == 0x1e512e)
	{
		printf("\033[31m[-]\033[0m Unknown variable\n");
		printf("Usage: set [option] [value]\n\n");
	}

}

/*
help
set - start end mark compression
info
show
use - high low default
*/

static void set_config(struct _CONFIG * conf, char * arg)
{
	char * key = arg;
	char * value = NULL;

#ifdef DEBUG
	printf("arg: %s\n",key);
#endif

	#ifndef _WINDOWS_
		strtok_r(key, " ", &value);
	#else
		strtok_s(key, " ", &value);
	#endif

#ifdef DEBUG
	printf("key: %s value: %s\n", key, value);
#endif

	if(key == NULL || value == NULL)
	{
		shell_help(0x1e512e);
		return;
	}


	// switch(BKDRHash(key))
	// {
	// 	case :
	// }

}

static unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;

    while (*str)
        hash = hash * seed + (*str++);

    return (hash & 0x7FFFFFFF);
}

// static void run_cmd(char * cmd, char * arg)
// {
//
//
// }

static void shell_run(struct _CONFIG * conf)
{
	char cmd[128] = {0};
	char * save_arg = NULL;
	while(1)
	{
		printf(">> ");
		fflush(stdout);
		fflush(stdin);
#ifndef _WINDOWS_
		read(0,cmd,128);
		cmd[strlen(cmd)-1] = 0;
		strtok_r(cmd, " ", &save_arg);
#else
		scanf_s("%s", cmd, 128);
		strtok_s(cmd, " ", &save_arg);
#endif

unsigned int cmd_hash = BKDRHash(cmd);

// #ifdef DEBUG
// printf("show :%x\nhelp:%x\nset :%x\ninfo :%x\nuse :%x\n",
// BKDRHash((char *)"show"),
// BKDRHash((char *)"help"),
// BKDRHash((char *)"set"),
// BKDRHash((char *)"info"),
// BKDRHash((char *)"use")
// );
// printf("cmd: %s opt: %s\n",cmd,save_arg);
// #endif

switch (cmd_hash) {
	case 0xe0a3139:shell_help(NULL);break;//help e0a3139
	case 0x1e512e:set_config(conf, save_arg);break;//set 1e512e
	case 0xf84518d:show_config(conf);break; //show f84518d
	case 0xe2ed712:break;//info e2ed712
	case 0x1ede5b:break;//use 1ede5b
	default:
		printf("command not found: %s\n",cmd);
}
		memset(cmd, 0, 128);
		save_arg = NULL;
	}
}

void shell_init(struct _CONFIG * conf)
{
	welcome();
	printf("type %s,target %s.\n", conf->process ? "process" : "PE", conf->target);
	printf("\033[31m[*] load file error.\033[0m \n");
	shell_run(conf);
	exit(1);
}
