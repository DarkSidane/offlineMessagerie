#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define TRUE 1
#define FALSE 0
#define S_MAX 256
#define NB_UTILISATEURS_MAX 10
#define SHM_KEY 9876
#define MAX_UTILISATEURS 100


struct Utilisateur {
    char nom[S_MAX];
    int disponible;
};

struct Utilisateur *shm_utilisateurs = NULL;

int nb_utilisateurs = 1;

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
				int trouve = 0;
				for (int i = 0; i < MAX_UTILISATEURS; i++) {
					if (shm_utilisateurs[i].disponible != TRUE) {
						strcpy(shm_utilisateurs[i].nom, buf[1]);
						shm_utilisateurs[i].disponible = TRUE;
						printf("\033[1;32m%s \033[0;32ms'est connecté(e) !\n\033[0;36m", buf[1]);
						trouve = 1;
						nb_utilisateurs++;
						break;
					}
				}
				if (!trouve) {
					printf("\033[0;31mNombre maximum d'utilisateurs atteint\n");
				}
			}
			break;

		case 'p':
			if (nb_utilisateurs >= 2) {
				if (buf[1] == NULL || buf[2] == NULL) {
					printf("\033[0;31mVous devez spécifier les noms d'utilisateurs pour parler\n");
				} else {
					int utilisateur1_present = 0;
					int utilisateur2_present = 0;
					int utilisateur1_index, utilisateur2_index;

					for (int i = 0; i < nb_utilisateurs; i++) {
						if (strcmp(shm_utilisateurs[i].nom, buf[1]) == 0 && shm_utilisateurs[i].disponible == TRUE) {
							utilisateur1_present = 1;
							utilisateur1_index = i;
						}
						if (strcmp(shm_utilisateurs[i].nom, buf[2]) == 0 && shm_utilisateurs[i].disponible == TRUE) {
							utilisateur2_present = 1;
							utilisateur2_index = i;
						}
					}

					if (utilisateur1_present && utilisateur2_present) {
						parler(buf[1], buf[2]);
					} else {
						printf("\033[0;31mLes utilisateurs spécifiés ne sont pas connectés\n");
					}
				}
			} else {
				printf("\033[0;31mIl faut au moins deux utilisateurs pour parler\n");
			}
			break;
		case 'l':
			printf("Utilisateurs connectés :\n");
			for (int i = 0; i < nb_utilisateurs; i++) {
				if (shm_utilisateurs[i].disponible) {
					printf(" - %s\n", shm_utilisateurs[i].nom);
				}
			}
			break;

		case 'd':
			if (buf[1] == NULL) {
				printf("\033[0;31mVous devez renseigner votre nom d'utilisateur\n");
			} else {
				int trouve = 0;
				for (int i = 0; i < nb_utilisateurs; i++) {
					if (strcmp(shm_utilisateurs[i].nom, buf[1]) == 0) {
						shm_utilisateurs[i].disponible = FALSE;
						trouve = 1;
						printf("\033[1;32m%s \033[0;32ms'est déconnecté(e) !\n\033[0;36m", buf[1]);
						break;
					}
				}
				if (!trouve) {
					printf("\033[0;31mUtilisateur non trouvé\n");
				}
			}
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
		   " -\033[1;36m l	\033[0;36m : affichage de la liste des utilisateur.\n"
		   " -\033[1;36m d 	\033[0;36m : déconnecte l'utilisateur local.\n"
		   " -\033[1;36m q      \033[0;36m : quitter le chat.\n");
	printf("\033[0m");
	printf("\033[0;36m\n\n> ");
}

int main() {

	mkfifo("build/u1u2", 0666);
	mkfifo("build/u2u1", 0666);

	int shm_id = shmget(SHM_KEY, MAX_UTILISATEURS * sizeof(struct Utilisateur), IPC_CREAT | 0666);
	if (shm_id == -1) {
		perror("Erreur lors de la création du segment de mémoire partagée");
		exit(EXIT_FAILURE);
	}

	shm_utilisateurs = shmat(shm_id, NULL, 0);
	if (shm_utilisateurs == (void *)-1) {
		perror("Erreur lors de l'attachement au segment de mémoire partagée");
		exit(EXIT_FAILURE);
	}

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

	if (shmdt(shm_utilisateurs) == -1) {
		perror("Erreur lors du détachement du segment de mémoire partagée");
		exit(EXIT_FAILURE);
	}
	if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		perror("Erreur lors de la suppression du segment de mémoire partagée");
		exit(EXIT_FAILURE);
	}

	sleep(1);
	return EXIT_SUCCESS;
}
