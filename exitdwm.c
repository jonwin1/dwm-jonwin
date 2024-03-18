# include <stdio.h>
# include <string.h>

void exitdwm ()
{
# if							   \
	defined S_LOCK				|| \
	defined S_RESTART_DWM		|| \
	defined S_OFFSCREEN			|| \
	defined S_EXIT				|| \
	defined S_REBOOT			|| \
	defined S_SHUTDOWN			|| \
	defined S_FORMAT
# error (conflicting macro names)
# endif

# define S_LOCK "Lock"
# define S_RESTART_DWM "Restart-Dwm"
# define S_OFFSCREEN "Off-screen"
# define S_EXIT "Exit"
# define S_REBOOT "Reboot"
# define S_SHUTDOWN "Shutdown"

# define S_FORMAT(ACTION) S_##ACTION

	FILE * exit_menu = popen (
		"echo \""
			S_FORMAT (LOCK) "\n"
			S_FORMAT (RESTART_DWM) "\n"
			S_FORMAT (OFFSCREEN) "\n"
			S_FORMAT (EXIT) "\n"
			S_FORMAT (REBOOT) "\n"
			S_FORMAT (SHUTDOWN)
			"\" | dmenu -p exit:"
		,
		"r"
	);

	char exit_action [16];

	if (
		exit_menu == NULL ||
		fscanf (exit_menu, "%15[a-zA-Z -]", exit_action) == EOF
	) {
		fputs ("Error. Failure in exit_dwm.", stderr);
		goto close_streams;
	}

    fprintf(stderr, "Selected action: %s\n", exit_action);

	if (strcmp (exit_action, S_LOCK) == 0) system ("slock");
	else if (strcmp (exit_action, S_RESTART_DWM) == 0) quit (& (const Arg) {1});
	else if (strcmp (exit_action, S_OFFSCREEN) == 0) system ("sleep .5; xset dpms force off");
	else if (strcmp (exit_action, S_EXIT) == 0) quit (& (const Arg) {0});
	else if (strcmp (exit_action, S_REBOOT) == 0) system ("systemctl reboot");
	else if (strcmp (exit_action, S_SHUTDOWN) == 0) system ("systemctl poweroff -i");

close_streams:
	pclose (exit_menu);

# undef S_LOCK
# undef S_RESTART_DWM
# undef S_OFFSCREEN
# undef S_EXIT
# undef S_REBOOT
# undef S_SHUTDOWN
# undef S_FORMAT
}
