#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

// #include "fenetre.c"
#define TRUE 1
#define FALSE 0
#define S_MAX 256			   // Taille max d'une chaîne de caractères
#define NB_UTILISATEURS_MAX 10 // Nombre max d'utilisateurs

char utilisateurs[NB_UTILISATEURS_MAX][S_MAX];
int nb_utilisateurs;
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
int parler(char *nom1, char *nom2)
{
	// Create the first process
	pid_t pid1 = fork();

	mkfifo("build/u1u2", 0666);
    mkfifo("build/u2u1", 0666);

	if (pid1 < 0)
	{
		fprintf(stderr, "Failed to create first process\n");
		return 1;
	}
	else if (pid1 == 0)
	{
		// Child process 1
		// execlp("./build/fenetre", "fenetre", nom1, NULL);
		// execlp("xterm", "xterm", "-e", "./build/dialogue", "u1u2", "u2u1", (char*) NULL);

		char *const xterm_args[] = {"xterm", "-e", "./build/dialogue", "build/u1u2", "build/u2u1", nom1, NULL};
		if (execvp("xterm", xterm_args) == -1) { perror("execvp"); exit(EXIT_FAILURE); }

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
		// execlp("./build/fenetre", "fenetre", nom2, NULL);
		// execlp("xterm", "xterm", "-e", "./build/dialogue", "u2u1", "u1u2", (char*) NULL);

		char *const xterm_args[] = {"xterm", "-e", "./build/dialogue", "build/u2u1", "build/u1u2", nom2, NULL};
		if (execvp("xterm", xterm_args) == -1) { perror("execvp"); exit(EXIT_FAILURE); }

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
	if (strlen(buf[0]) > 1) {
		printf("\033[0;31mCommande non reconnue\n");
		printf("\033[0;36m\n> ");
	}
	else
	{
		switch (buf[0][0])
		{
		case 'e':
			// Si on veut enregistrer le nom d'utilisateur
			if (buf[1] == 0)
				printf("\033[0;31mVous devez renseigner un nom d'utilisateur\n");
			else
			{
				printf("\033[0;36mEnregistrement en cours...\n");
				if (nb_utilisateurs == NB_UTILISATEURS_MAX)
					printf("\033[0;31mNombre d'utilisateurs maximum atteint !\n");
				else
				{
					strcpy(utilisateurs[nb_utilisateurs], buf[1]);
					nb_utilisateurs++;
					printf("\033[1;32m%s \033[0;32menregistré !\n", buf[1]);
				}
				enregistre = TRUE;
			}
			break;
		case 'p':
			if (nb_utilisateurs >= 2)
			{
				printf("\nQuel compte voulez-vous utiliser ?\n");
				for (int i = 0; i < nb_utilisateurs; i++)
				{
					printf(" - %d : %s\n", i, utilisateurs[i]);
				}
				int choix = -1;
				printf("\n> ");
				scanf("%d", &choix);
				while (choix < 0 || choix > nb_utilisateurs)
				{
					printf("\nVeillez entrer un nombre entre 0 et %d\n> ", nb_utilisateurs - 1);
					int c;
					while ((c = getchar()) != '\n' && c != EOF)
					{
					}
					scanf("%d", &choix);
					printf("\n");
				}
				printf("\nAvec quelles compte voulez-vous parler ?\n");
				for (int i = 0; i < nb_utilisateurs; i++)
				{
					printf(" - %d : %s\n", i, utilisateurs[i]);
				}
				int choix2 = -1;
				printf("\n> ");
				scanf("%d", &choix2);
				while (choix2 < 0 || choix2 > nb_utilisateurs)
				{
					printf("\nVeillez entrer un nombre entre 0 et %d\n> ", nb_utilisateurs - 1);
					int c;
					while ((c = getchar()) != '\n' && c != EOF)
					{
					}
					scanf("%d", &choix2);
					printf("\n");
				}
				printf("Ouverture de la fenêtre de dialogue\n");
				parler(utilisateurs[choix], utilisateurs[choix2]);
			}
			else
				printf("\033[0;31mIl faut au moins deux utilisateurs pour parler\n");
			break;
		case 'q':
			printf("\033[1;36mAu revoir.\n");
			return FALSE;
			break;
		default:
			printf("\033[0;36mVeuillez écrire une lettre entre e, p ou q\n");
			break;
		}
		printf("\033[0;36m\n> ");
	}
	return TRUE;
}
int main()
{
	printf("\033[H\033[2J");
	printf("\033[0;32m");
	printf(" .----------------.  .----------------.  .----------------.  .----------------. \n"
		   "| .--------------. || .--------------. || .--------------. || .--------------. |\n"
		   "| |     ______   | || |  ____  ____  | || |      __      | || |  _________   | |\n"
		   "| |   .' ___  |  | || | |_   ||   _| | || |     /  \\     | || | |  _   _  |  | |\n"
		   "| |  / .'   \\_|  | || |   | |__| |   | || |    / /\\ \\    | || | |_/ | | \\_|  | |\n"
		   "| |  | |         | || |   |  __  |   | || |   / ____ \\   | || |     | |      | |\n"
		   "| |  \\ `.___.'\\  | || |  _| |  | |_  | || | _/ /    \\ \\_ | || |    _| |_     | |\n"
		   "| |   `._____.'  | || | |____||____| | || ||____|  |____|| || |   |_____|    | |\n"
		   "| |              | || |              | || |              | || |              | |\n"
		   "| '--------------' || '--------------' || '--------------' || '--------------' |\n"
		   " '----------------'  '----------------'  '----------------'  '----------------' \n");
	printf("\n\n\033[0m");
	printf("\033[1;36m\n");

	printf("Bienvenue dans CHAT OFFLINE.\n"
		   "Le nouveau système de CHAT en local, permettant de communiquer d'une fenêtre à une autre.\n\n"
		   "Liste des commandes\033[0;36m\n"
		   " -\033[1;36m e <nom>\033[0;36m : s'enregister avec le nom <nom>.\n"
		   " -\033[1;36m p <nom>\033[0;36m : ouvrir un dialogue avec l'utilisateur nomme <nom>.\n"
		   " -\033[1;36m q      \033[0;36m : quitter le chat.\n");
	printf("\033[0m");
	printf("\033[0;36m\n\n> ");
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
