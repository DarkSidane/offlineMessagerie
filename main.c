#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
//#include "fenetre.c"
#define TRUE 1
#define FALSE 0
#define S_MAX 256 // Taille max d'une chaîne de caractères
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

	if (pid1 < 0)
	{
		fprintf(stderr, "Failed to create first process\n");
		return 1;
	}
	else if (pid1 == 0)
	{
		// Child process 1
		execlp("./build/fenetre", "fenetre", nom1, NULL);
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
		execlp("./build/fenetre", "fenetre", nom2, NULL);
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
	if (strlen(buf[0]) > 1) printf("Commande non reconnue\n"); 
	else {
		printf("------------------------\n");
		switch (buf[0][0])
		{
		case 'e':
			// Si on veut enregistrer le nom d'utilisateur
			if (buf[1] == 0) printf("Vous devez renseigner un nom d'utilisateur");
			else 
			{
				printf("Enregistrement en cours...\n");
				if (nb_utilisateurs == NB_UTILISATEURS_MAX) printf("Nombre d'utilisateurs maximum atteint !\n");
				else
				{
					strcpy(utilisateurs[nb_utilisateurs], buf[1]);
					nb_utilisateurs++;
					printf("%s enregistré !\n", buf[1]);
				}
				enregistre = TRUE;
			}
			break;
		case 'p':
			if (enregistre == TRUE) 
			{
				printf("Quel compte voulez-vous utiliser ?\n");
				for (int i = 0; i < nb_utilisateurs; i++)
				{
					printf("%d : %s\n",i, utilisateurs[i]);
				}
				int choix = -1;
				scanf("%d", &choix);
				while (choix < 0 || choix > nb_utilisateurs)
				{
					printf("Veillez entrer un nombre entre 0 et %d\n", nb_utilisateurs);
					scanf("%d", &choix);
				}	
				printf("Avec quelles compte voulez-vous parler ?\n");
				for (int i = 0; i < nb_utilisateurs; i++)
				{
					printf("%d : %s\n",i, utilisateurs[i]);
				}
				int choix2 = -1;
				scanf("%d", &choix2);
				while (choix2 < 0 || choix2 > nb_utilisateurs)
				{
					printf("Veillez entrer un nombre entre 0 et %d\n", nb_utilisateurs);
					scanf("%d", &choix2);
				}	
				printf("Ouverture de la fenêtre de dialogue\n");
				parler(utilisateurs[choix], utilisateurs[choix2]);
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
int main()
{
	int nb_utilisateurs = 0;
	int buildStatus = system("gcc fenetre.c -o build/fenetre -lX11");
	if (buildStatus == 0) {
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
" '----------------'  '----------------'  '----------------'  '----------------' \n"
 				"Bienvenue dans CHAT OFFLINE.\n" 
				"=============================\n"
				"Le nouveau système de CHAT en local, permettant de communiquer d'une fenêtre à une autre. \n"
				"=============================\n"
				"Entrer e <nom> pour vous enregistrer.\n"
				"Entrer p pour afficher la fenêtre de Dialogue Utilisateur.\n"
				"Entrer q pour quitter notre logiciel.\n"
				"\n"
				"---------------------------------------\n"
				"\n");
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
