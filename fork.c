#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[]) {
	int pid;

	printf("Starting program; process has pid %d\n", getpid());
	FILE * file = fopen("fork-output.txt", "w");
	fprintf(file, "%s", "BEFORE FORK\n");
	fflush(file);
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		fprintf(stderr, "Could not pipe()");
		exit(1);
	}
	if ((pid = fork()) < 0) {
		fprintf(stderr, "Could not fork()");
		exit(1);
	}

	/* BEGIN SECTION A */

	printf("Section A;  pid %d\n", getpid());
	fprintf(file, "%s", "SECTION A\n");
	fflush(file);
	wait(NULL);

	/* END SECTION A */
	if (pid == 0) { // Child reads from pipe
		/* BEGIN SECTION B */

		close(pipefd[0]); // Close read end
		// Create file stream
		FILE * fileStream = fdopen(pipefd[1], "w");
		fputs("hello from Section B\n", fileStream);
		printf("Section B\n");
		fprintf(file, "%s", "SECTION B\n");
		fflush(file);
		fclose(file);
		char *newenviron[] = { NULL };

 		if (argc <= 1) {
                	printf("No program to exec.  Exiting...\n");
                	exit(0);
        	}

        	printf("Running exec of \"%s\"\n", argv[1]);
        	dup2(fileno(fileStream), 2);
		execve(argv[1], &argv[1], newenviron);
        	printf("End of program \"%s\".\n", argv[0]);
		exit(0);

		/* END SECTION B */
	} else {
		/* BEGIN SECTION C */
		close(pipefd[1]);
		FILE * fileStream = fdopen(pipefd[0], "r");
		char str[60];
		if( fgets(str, 60, fileStream) != NULL ) {
			printf("%s", str);
		}
		printf("Section C\n");
		fprintf(file, "%s", "SECTION C\n");
		fflush(file);

		fclose(file);
		exit(0);

		/* END SECTION C */
	}
	/* BEGIN SECTION D */

	printf("Section D\n");
	fprintf(file, "%s", "SECTION D\n");
	fflush(file);
	fclose(file);

	/* END SECTION D */
}
