#include "pch.h"
#include "vp_shell.h"
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

void help();

int main(int argc, char ** argv)
{
	struct optparse_long longopts[] = {
        {"help", 'h', OPTPARSE_NONE},
        {"version", 'v', OPTPARSE_NONE},
        {"attach", 'a', OPTPARSE_REQUIRED},
        {"file", 'f', OPTPARSE_REQUIRED},
        {0}
    };

  char *arg;
  int option;
  struct optparse options;
	static struct _CONFIG conf;

  optparse_init(&options, argv);


	conf_init(&conf);

  while ((option = optparse_long(&options, longopts, NULL)) != -1) {
      switch (option) {
      case 'h': {
          help();
					exit(0);
          break;
				}
      case 'v': {
	        printf("Verison: %s\n", VP_VERSION);
					exit(0);
	        break;
				}
      case 'a': {
				conf.process = 1;
				conf.target = options.optarg;
				shell_init(&conf);
				break;
			}
      case 'f':{
				char buf[4] = {0};
				conf.target = options.optarg;
				FILE * fp = fopen((char *)conf.target, "rb");

				if (!fp)
				{
					fprintf(stderr, "Can't open file :%s\n", (char *)conf.target);
					return -1;
				}
				fread(buf, 4, 1, fp);
				// printf("%x\n",*(uint32_t *)buf);
				if ((*(uint32_t *)buf & 0x5a4d) == 0x5a4d)
					conf.type = PE;
				else if ((*(uint32_t *)(buf) & 0x464c457f) == 0x464c457f)
					conf.type = ELF;
				else
				{
					fprintf(stderr, "%s file type maybe incorrect\n", FAILED);
					return -1;
				}
				fclose(fp);
				shell_init(&conf);
				break;
			}
      case '?': {

			}
			default: {
				fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
				exit(EXIT_FAILURE);
			}

      }
  }

  /* Print remaining arguments. */
  while ((arg = optparse_arg(&options)))
      printf("%s\n", arg);
  return 0;

}

void help()
{
	puts("Usage: vp.exe [options] target.\n\nOptions:\n");
	puts(" -v|--version\tshow program's version number and exit");
	puts(" -h|--help\tshow this help message and exit");
	puts(" -p attach process pid");
	puts(" -e load pe file\n");
}
