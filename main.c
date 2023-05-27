#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
//#include "fenetre.c"
#define TRUE 1
#define FALSE 0
#define S_MAX 256 // Taille max d'une chaîne de caractères

char **decoupe_mots(char *buf)
{
	// Prend un mot en argument et renvoie les mots contenus dedans
	char delim[] = " ";
	char **com;
	com = malloc(2 * S_MAX * sizeof(char));
	char *ptr = strtok(buf, delim);

	int i = 0;
	while (ptr != NULL)
	{
		com[i] = ptr;
		i++;
		ptr = strtok(NULL, delim);
	}
	return com;
}
int parler(char *nom)
{
	// Create the first process
	pid_t pid1 = fork();

	if (pid1 < 0)
	{
		fprintf(stderr, "Failed to create first process\n");
		return 1;
	}
	else if (pid1 == 0)
	{
		// Child process 1
		execlp("./build/fenetre", "fenetre", NULL);
		fprintf(stderr, "Failed to launch xterm for process 1\n");
		return 1;
	}

	// Create the second process
	pid_t pid2 = fork();

	if (pid2 < 0)
	{
		fprintf(stderr, "Failed to create second process\n");
		return 1;
	}
	else if (pid2 == 0)
	{
		// Child process 2
		execlp("./build/fenetre", "fenetre", NULL);
		fprintf(stderr, "Failed to launch xterm for process 2\n");
		return 1;
	}

	// Wait for both processes to complete
	wait(NULL);
	wait(NULL);

	return 0;
}
int interp_commande(char *commande)
{
	/*Interprète les commandes et exécute la fonction appropriée.
	 * Renvoie 1 si la commande est différente de q, 0 sinon.
	 * La fonction découpe_mots servira à découper la commande dans les cas où il y a des arguments (par exemple : la commande p).*/
	char **buf = decoupe_mots(commande);
	char nom[S_MAX];
	int enregistre = TRUE;
	if (strlen(buf[0]) > 1) printf("Commande non recconu\n"); 
	else {
		switch (buf[0][0])
		{
		case 'e':
			// Si on veut enregistrer le nom d'utilisateur
			if (buf[1] == 0) printf("Vous devez renseigner un nom d'utilisateur");
			else {
				printf("Votre nom d'utilisateur est %s\n", buf[1]);
				printf("Enregistrement en cours...\n");
				enregistre = TRUE;
			}
			break;
		case 'p':
			if (enregistre == TRUE) {
				printf("Ouverture de la fenêtre de dialogue\n");
				parler("TEST");
			}
			else
				printf("Vous n'êtes pas enregistré ! \n");
			break;
		case 'q':
			printf("Au revoir.\n");
			return FALSE;
			break;
		default:
			printf("Veuillez écrire une lettre entre e, p ou q\n");
			break;
		}
		printf("\n===================================\n");
	}
	return TRUE;
}
int main(int argc, char **argv)
{
	int buildStatus = system("gcc fenetre.c -o build/fenetre -lX11");

	if (buildStatus == 0) {
		printf("Build successful.\n");
	} else {
		printf("Build failed.\n");
	}
	int running = TRUE;
	char buf[S_MAX];

	while (running == TRUE)
	{
		while (fgets(buf, S_MAX, stdin) != NULL)
		{
			buf[strcspn(buf, "\n")] = 0;
			break;
		}
		fflush(stdin);
		running = interp_commande(buf);
	}

	return EXIT_SUCCESS;
}
