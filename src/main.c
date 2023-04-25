#include <getopt.h>
#include <sysexits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "lib/colors.h"
#include "lib/tweak-loader.h"
#include "lib/tweak-handler.h"
#include "lib/bypass.h"
#include "lib/sideload.h"
#include "lib/chromeos.h"
#include "lib/update-handler.h"

/* TEMP */
int unimplemented(int argc, char **argv)
{
	fprintf(stderr, "work in progress; unimplemented\n");
	exit(1);
}

void error(int type);
void help();
void run();
int cmd_run(int argc, char **argv);
int cmd_update(int argc, char **argv);
int cmd_tweak(int argc, char **argv);

typedef int (*pie_command_main)(int,char**);

static const struct option pie_longopts[] =
{
	{"help",	no_argument,       0,  'h' },
	{0,         0,                 0,   0  }
};

static const char *pie_shortopts =
	"h" /* help */
	;

struct pie_subcommand
{
	const char *name;
	pie_command_main routine;
};

struct pie_subcommand pie_commands[] =
{
	{"run",     cmd_run},
	{"pico",    cmd_run},
	{"update",  cmd_update},
	{"tweak",   cmd_tweak},
	{0,0}
};

/**
 * @brief parse and take required action for global options. stops on first non-option
 *
 * @param argc number of elements before the NULL-terminator in `argv`
 * @param argv NULL-terminated array of program arguments
 *
 * @returns positive offset to add to `argc` and `argv` for further parsing
 */
int do_global_opts(int argc, char **argv)
{
	while(1)
	{
		int longopt_index;
		int c = getopt_long(argc, argv, pie_shortopts, pie_longopts, &longopt_index);

		/* done parsing */
		if(c == -1)
			return optind;

		switch(c)
		{
			case 0:
				/* no longopts yet */
				break;

			case 'h':
				/* TODO: help should be printed to stderr */
				help();
				break;

			/* unrecognised option */
			case '?':
				break;

			/* shouldn't be reached */
			default:
				break;
		}
	}

	return optind;
}

int cmd_run(int argc, char **argv)
{
	run();
	return 0;
}

int cmd_update(int argc, char **argv)
{
	update();
	return 0;
}

int tweak_load(int argc, char **argv)
{
	if(argv[1])
		if(argv[2])
      loadTweak(argv[1], argv[2]);
    else
      printf(RED "Expected a tweak name, " GRN "Example: " CYN "pie tweak load ~/Desktop/chrome-pie/src/tweaks/demo.c demo");
	else
		printf(RED "Expected a tweak file, " GRN "Example: " CYN "pie tweak load ~/Desktop/chrome-pie/src/tweaks/demo.c\n");

	return 0;
}

int cmd_tweak(int argc, char **argv)
{
	struct pie_subcommand verbs[] =
	{
		{"load", tweak_load},
		{"create", unimplemented},
		{0, 0},
	};

	/* iterate valid commands and look for a match */
	for(struct pie_subcommand *cmd = verbs; cmd->name != NULL; cmd++)
	{
		/* if there is a match, call the command's main routine */
		if(strcmp(argv[1], cmd->name) == 0)
			return cmd->routine(argc - 1, argv + 1);
	}
	
	/* if no command is found, error out */
	error(1);

	return 0;
}

int main(int argc, char *argv[])
{
	int r = do_global_opts(argc, argv);

	argc -= r;
	argv += r;

	if(argc < 2)
	{
		/* TODO: error code enum? */
		error(1);
		exit(EX_USAGE);
	} else if(argc <= 0) {
		error(3);
		exit(EX_USAGE);
	}

	/* iterate valid commands and look for a match */
	for(struct pie_subcommand *cmd = pie_commands; cmd->name != NULL; cmd++)
	{
		/* if there is a match, call the command's main routine */
		if(strcmp(*argv, cmd->name) == 0)
			return cmd->routine(argc, argv);
	}
	/* if no command is found, error out */
	error(2);
}

void error(int type)
{
    if (type == 1)
    {
        printf(RED "Expected a parameter or a command, " reset GRN "see --help or -h for all the avalible commands and parameters\r\n");
    }
    if (type == 2)
    {
        printf(RED "Did not find command or parameter, " reset GRN "see --help or -h for all the avalible commands and parameters\r\n");
    }
}

void help()
{
    printf(UYEL "Chrome Pie" reset "\n -help, -h - Displays this message\n run - runs pie\n tweak - create tweak command(s)\n  tweak create -n 'tweak name' - creates a template file with the name of your tweak\n update - updates pie\r\n");
}

/*void run(int argc, char **argv)
{
    system("sudo chmod +xrw ./ascii.sh && sudo ./ascii.sh\n\n");
    printf(BRED "Currently a work in progress, there is no functionallity as of right now. All of the features that work are private!\n\n");
    printf(UMAG "Developed with love from puffer <3\r\n\n");
}*/
