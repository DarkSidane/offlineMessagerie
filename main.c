#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

// Taille max d'une chaîne de caractères
#define S_MAX 256
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
			if (enregistre == TRUE)
				printf("Ouverture de la fenêtre de dialogue\n");
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
