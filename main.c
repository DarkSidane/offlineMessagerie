#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define TRUE 1
#define FALSE 0
#define S_MAX 256
#define NB_UTILISATEURS_MAX 10

char utilisateurs[NB_UTILISATEURS_MAX][S_MAX];
int nb_utilisateurs;

char **decoupe_mots(char *buf) {

	char delim[] = " ";
	char **com;
	com = malloc(2 * sizeof(char*));
	char *ptr = strtok(buf, delim);

	int i = 0;
	while (ptr != NULL) {
		com[i] = ptr;
		i++;
		ptr = strtok(NULL, delim);
	}
	return com;
}
void parler(char *nom1, char *nom2) {

	pid_t pid1 = fork();
	if (pid1 < 0) { fprintf(stderr, "Failed to create first process\n"); exit(EXIT_FAILURE); }
	else if (pid1 == 0) {
		char *const xterm_args[] = {"xterm", "-e", "./build/dialogue", "build/u1u2", "build/u2u1", nom1, nom2, NULL};
		if (execvp("xterm", xterm_args) == -1) { perror("execvp"); exit(EXIT_FAILURE); }
		fprintf(stderr, "Failed to launch xterm for process 1\n");
	}

	pid_t pid2 = fork();
	if (pid2 < 0) { fprintf(stderr, "Failed to create second process\n"); exit(EXIT_FAILURE); }
	else if (pid2 == 0) {
		char *const xterm_args[] = {"xterm", "-e", "./build/dialogue", "build/u2u1", "build/u1u2", nom2, nom1, NULL};
		if (execvp("xterm", xterm_args) == -1) { perror("execvp"); exit(EXIT_FAILURE); }
		fprintf(stderr, "Failed to launch xterm for process 2\n");
	}

	wait(NULL);
	wait(NULL);
}
int interp_commande(char *commande) {

	int R = TRUE;
	char **buf = decoupe_mots(commande);
	int choix = -1, choix2 = -1;

	if (strlen(buf[0]) > 1) { printf("\033[0;31mCommande non reconnue\n\033[0;36m\n> "); }
	else {

		switch (buf[0][0]) {

		case 'e':
			if (buf[1] == NULL) {
				printf("\033[0;31mVous devez renseigner un nom d'utilisateur\n");
			} else if (strlen(buf[1]) >= S_MAX) {
				printf("\033[0;31mNom d'utilisateur trop long\n");
			} else {
				// Copie sécurisée du nom d'utilisateur
				strncpy(utilisateurs[nb_utilisateurs], buf[1], S_MAX);
				utilisateurs[nb_utilisateurs][S_MAX - 1] = '\0';
				nb_utilisateurs++;
				printf("\033[1;32m%s \033[0;32menregistré !\n\033[0;36m", buf[1]);
			}
			break;

		case 'p':
			if (nb_utilisateurs >= 2) {

				printf("\nQuel compte voulez-vous utiliser ?\n");
				for (int i = 0; i < nb_utilisateurs; i++) printf(" - %d : %s\n", i, utilisateurs[i]);
				printf("\n> ");
				scanf("%d", &choix);
				while (choix < 0 || choix > nb_utilisateurs - 1) {
					printf("\nVeillez entrer un nombre entre 0 et %d\n> ", nb_utilisateurs - 1);
					int c;
					while ((c = getchar()) != '\n' && c != EOF) { }
					scanf("%d", &choix);
					printf("\n");
				}
				printf("\nAvec quelles compte voulez-vous parler ?\n");
				for (int i = 0; i < nb_utilisateurs; i++) printf(" - %d : %s\n", i, utilisateurs[i]);
				printf("\n> ");
				scanf("%d", &choix2);
				while (choix2 < 0 || choix2 > nb_utilisateurs - 1) {
					printf("\nVeillez entrer un nombre entre 0 et %d\n> ", nb_utilisateurs - 1);
					int c;
					while ((c = getchar()) != '\n' && c != EOF) { }
					scanf("%d", &choix2);
					printf("\n");
				}
				printf("Ouverture de la fenêtre de dialogue ...\n");
				parler(utilisateurs[choix], utilisateurs[choix2]);
				choix = -1;
				choix2 = -1;
			}
			else printf("\033[0;31mIl faut au moins deux utilisateurs pour parler\n");
			break;

		case 'q':
			printf("\033[1;36mAu revoir.\n\n");
			R = FALSE;
			break;

		default:
			printf("\033[0;36mVeuillez écrire une lettre entre e, p ou q\n");
			break;
		}
	}
	return R;
}

void printMenu() {

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
}

int main() {

	mkfifo("build/u1u2", 0666);
	mkfifo("build/u2u1", 0666);

	printMenu();

	int running = TRUE;
	while (running == TRUE) {
		fflush(stdin);
		char buf[S_MAX];
		while (fgets(buf, S_MAX, stdin) != NULL) {
			buf[strcspn(buf, "\n")] = 0;
			break;
		}
		
		fflush(stdin);
		if(strlen(buf) == 0) continue;
		else running = interp_commande(buf);
		if(running == TRUE) printf("\033[0;36m\n> ");
	}
	sleep(1);
	return EXIT_SUCCESS;
}
