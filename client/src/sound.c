#include <stdio.h>
#include <unistd.h>

void play_sound (const char *file,int times)
{
	pid_t pid;
	pid = fork ();
	if (pid == 0)
	{
		execlp ("mplayer", "mplayer", file, NULL);
	}
}

