#include "pch.h"
#include "vp_shell.h"
#include "executefile.h"
#include "process.h"
#include "disassem.h"

#define size_malloc(p) (*(((size_t *)p)-1) & ~(0x01|0x02))

static void welcome();
static void shell_help(unsigned int);
static unsigned int BKDRHash(char *);
static void shell_run(struct _CONFIG *);
static void show_config(struct _CONFIG *);
static void set_config(struct _CONFIG *, char *);
static void show_info(struct _CONFIG *);
static void analysis_target(struct _CONFIG *);
static bool search_marked(uint8_t *, mi_t **);


static inline char * get_target(struct _CONFIG * x, char * buf)
{
	buf = x->process ? snprintf(buf, 33, "%ld", (unsigned long) x->target), buf : (char *)x->target;
	return buf;
}

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

static void analysis_target(struct _CONFIG * conf)
{
	// mi_t [] marked_list = {}
	// mi_t mark_inv = {};
	// uint8_t * ptr = get_text_ptr((elfobj_t *)conf->obj);
	// if (search_marked(ptr,&conf->marked) == false)
	// {
	// 	fprintf(stderr, "%s Analysis termination<search> \n", FAILED);
	// 	return;
	// }
	// #if DEBUG
	// printf("marked_interval:\n");
	// for (mi_t * tmp = conf->marked; tmp->begin != 0; tmp++)
	// {
	// 	printf("mi_begin: %p,mi_end: %p\n", tmp->begin, tmp->end);
	// }
	// #endif

}
static bool search_marked(uint8_t * ptr, mi_t ** marked)
{
	uint8_t * s_seek = ptr;
	uint8_t * mi_begin = NULL;
	uint8_t * mi_end = NULL;
	const uint8_t bgstrsz = strlen(BEGIN_FALGS) + 1;
	const uint8_t edstrsz = strlen(END_FLAGS) + 1;
  mi_t * tmp = (mi_t *)malloc(sizeof(mi_t));
	do {
			//TODO : add marked

		mi_begin = mi_end = NULL;
		mi_begin = (uint8_t *)strstr((char *)s_seek, (char *)BEGIN_FALGS);
		if (mi_begin != NULL)
			{
        s_seek = mi_begin + bgstrsz;
        tmp->begin = (size_t)mi_begin;
      }

		mi_end = (uint8_t *)strstr((char *)s_seek, (char *)END_FLAGS);
		if (mi_end != NULL)
    {
			s_seek = mi_end + edstrsz;
      tmp->end = (size_t)mi_end;
    }

		if(mi_begin == NULL && mi_end)
		{
			fprintf(stderr, "%s The flags do not match(Not closed).  %p\n", FAILED, s_seek);
			return false;
		}
    mi_begin && mi_end ? printf("mi_begin: %p,mi_end: %p\n", mi_begin, mi_end) : 0;
    tmp = (mi_t *)realloc(tmp, size_malloc(tmp) + sizeof(mi_t));
	} while(mi_begin || mi_end);

	*marked = tmp;
  return true;
}

static void show_info(struct _CONFIG * conf)
{
	char buf[33] = {0};
	printf("target: %s %s\n",
					conf->process ?
					"process\npid:" :
					"binary file\npath:",
					get_target(conf,buf));

	conf->process ?  get_process_info((size_t)conf->obj) : show_exe_info((elfobj_t *)conf->obj);
/*
ldd:
section_count:

 */
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
		printf("\tdisass\n");
	}
	if (key == 0x1e512e)
	{
		printf("%s Unknown variable\n", FAILED);
		printf("Usage: set [option] [value]\n\n");
	}

}

/*
help
set - start end mark compression
info
show
use - high low default
disass
*/

static void set_config(struct _CONFIG * conf, char * arg)
{
	char * key = arg;
	char * value = NULL;

#ifdef DEBUG
	printf("arg: %s\n",key);
#endif

	#ifdef _WIN32
		strtok_s(key, " ", &value);
	#else
		strtok_r(key, " ", &value);
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
	// {·
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
#ifndef _WIN32
		read(0,cmd,128);
		cmd[strlen(cmd)-1] = 0;
		strtok_r(cmd, " ", &save_arg);
#else
		scanf_s("%s", cmd, 128);
		strtok_s(cmd, " ", &save_arg);
#endif

unsigned int cmd_hash = BKDRHash(cmd);

switch (cmd_hash) {
	case 0xe0a3139:shell_help(0);break;//help e0a3139
	case 0x1e512e:set_config(conf, save_arg);break;//set 1e512e
	case 0xf84518d:show_config(conf);break; //show f84518d
	case 0xe2ed712:show_info(conf);break;//info e2ed712
	case 0x1ede5b:break;//use 1ede5b
	case 0x24fbe7b:show_disassem_code((elfobj_t *)conf->obj);break;//disass 24fbe7b
	case 0x278b3da4:break;//analysis_target(conf);break;//analysis 278b3da4
	case 0xda8408e:exit(0);
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
	printf("type %s,target %s.\n", conf->process ? "process" : "binary file", conf->target);
	open_file(conf);
	shell_run(conf);
	exit(1);
}
