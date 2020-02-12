#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LON_CHEMIN        255
#define LON_NOM_CHEMIN    65280 
#define LON_NOM           255
#define LON_CONTENU       255
#define MAX_FILS          1024
#define TYPE_REP          'R'
#define TYPE_FICH         'F'
#define LON_CMD           20             

typedef enum { OK, NO } resultat;
typedef enum { true, false } boolean;

struct resultat {
	char p[LON_NOM_CHEMIN];
	struct resultat * suivant;
};

typedef struct element {
	char type;
	char nom[LON_NOM];
	//char path[LON_NOM_CHEMIN];
	char contenu[LON_CONTENU];
	struct element * fils;
	struct element * frere;
	struct element * prec;
	struct element * pere;
	int nbr_fils;
} noeud;

noeud * racine;   		  	    
struct resultat * resultats;    

//####################################################################################

noeud * creer_element(noeud *N, noeud *P, char *nom, char type_element) {
	int i, l;
	N = (noeud *) malloc(sizeof(noeud));
	if (N == NULL) {
		return NULL;
	}
	N->nbr_fils = 0;
	N->type = type_element;
	strcpy(N->contenu, "");
	//T->path = (char *) malloc(sizeof(char) * strlen(path));
	//strcpy(T->path, path);
	//T->name = (char *)malloc(sizeof(char) * strlen(name));
	strcpy(N->nom, nom);
	N->pere = P;
	N->fils = NULL;
	N->frere = NULL;
	N->prec = NULL;

	return N;
}

//####################################################################################

void extraire_nom(char * chemin, char * nom) {
	int i, l;
	l = (int) strlen(chemin);
	i = l - 1;
	while (chemin[i] != '/') {
		i--;
	}
	if (strlen(chemin)-i > LON_NOM)
		strcpy(nom, "");
	else
		strcpy(nom, &chemin[i+1]);
}

//####################################################################################

int calc_lon_chemin(char * s) {
	int i, k = 0;
	for (i = 0; s[i] != '\0'; i++) {
		if (s[i] == '/') {
			k = k + 1;
		}
	}
	return k;
}

//####################################################################################

noeud * positionner(char * chemin) {

	noeud * t;
	char * mot;
	char chemin_temp[LON_NOM_CHEMIN];
	int i = 0;
	boolean trouve = false;

	t = racine;

	strcpy(chemin_temp, chemin);
	mot = strtok(chemin_temp, "/");

	while (mot != NULL) {  
		trouve = false;
		if (t->fils != NULL)
			t = t->fils;
		else {
			return NULL;
		}
		if (strcmp(mot, t->nom) == 0)
			trouve = true;

		while (t->frere != NULL && trouve == false) {
			t = t->frere;
			if (strcmp(mot, t->nom) == 0)
				trouve = true;
		}

		if (trouve == false)   {  
			return NULL;
		}

		mot = strtok(NULL, "/");
	}
	
	return t;

}

//####################################################################################

resultat creer(char * nom, char * chemin, int longueur_chemin, char type_element) {

	noeud * t;
	noeud * new;
	noeud * f;
	char * mot;
	char chemin_temp[LON_NOM_CHEMIN];
	int i = 0;
	boolean trouve = false;
	t = racine;
	f = t;

	strcpy(chemin_temp, chemin);
	mot = strtok(chemin_temp, "/");

	new = (noeud *) malloc(sizeof(noeud));   	     
	if (new == NULL)
		return NO;

	while (i < longueur_chemin-1) {    
		trouve = false;
		f = t;
		if (t->fils != NULL)
			t = t->fils;
		if (strcmp(mot, t->nom) == 0)
			trouve = true;

		while(t->frere != NULL && trouve == false) {
			t = t->frere;
			if (strcmp(mot, t->nom) == 0) {
				trouve = true;
			}
		}

		if (trouve == false)   { 
			return NO;
		}
		else if (trouve == true && t->type == TYPE_FICH)
			return NO;

		mot = strtok(NULL, "/");
		i++;
	}

	if (t->nbr_fils == MAX_FILS) {
		return NO;
	}

	if (t->fils == NULL) {
		f = t;
		f->nbr_fils++;
		new = creer_element(t, f, nom, type_element);
		t->fils = new;
		new->prec = NULL; 
		return OK;
	}

	f = t;
	t = t->fils;
	if (strcmp(t->nom, nom) == 0) {
		return NO;
	}
	while(t->frere != NULL) {
		t = t->frere;
		if (strcmp(t->nom, nom) == 0) {
			return NO;
		}
	}

	f->nbr_fils++;
	new = creer_element(t, f, nom, type_element);
	t->frere = new;
	new->prec = t;
	new->pere = f;
	return OK;
}

//####################################################################################

resultat lire(char * chemin, char * nom, char * contenu) {

	noeud * t;

	t = positionner(chemin);

	if (t == NULL) {
		return NO;
	}

	if (strcmp(t->nom, nom) == 0 && t->type == TYPE_FICH) {
		strcpy(contenu, t->contenu);
		return OK;
	}

	return NO;

}

//####################################################################################

int ecrire(char * chemin, char * nom, char * contenu) {

	noeud * t;

	t = positionner(chemin);

	if (t == NULL) {
		return -1;
	}

	if (strcmp(t->nom, nom) == 0 && t->type == TYPE_FICH) {
		contenu[strlen(contenu)-1] = '\0';
		strcpy(t->contenu, contenu);                    
		return (int) strlen(t->contenu);
	}

	return -1;

}

//####################################################################################

resultat supprimer(char * chemin, char * nom) {

		noeud * t;

		t = positionner(chemin);
		
		if (t == NULL) {
			return NO;
		}
		
		if (t->type == TYPE_REP && t->nbr_fils != 0) {
			return NO;
		}
		
		if (strcmp(t->nom, nom) == 0) {
			//printf(" %d ", t->sonsnum);
			//printf(" %s %s %d ", t->name, name, p_l);
			

			if (t->prec == NULL && t->frere == NULL) {			     
				t->pere->fils = NULL;
			}
			
			else if (t->prec == NULL && t->frere != NULL) {		 	 
				t->pere->fils = t->frere;
				t->frere->prec = NULL;
			}

			else if (t->prec != NULL && t->frere == NULL) {     
				t->prec->frere = NULL;
			}
			
			else {
				t->prec->frere = t->frere;    					  
				t->frere->prec = t->prec;
			}
			
			t->pere->nbr_fils--;
			t->pere = NULL;
			free(t);
			return OK;
	    }

	return NO;
}

//####################################################################################

int supprimer_rec(noeud * R, int nbr_supp) {

	if (R->fils != NULL)
		nbr_supp = nbr_supp + supprimer_rec(R->fils, nbr_supp);

	if (R->frere != NULL)
		nbr_supp = nbr_supp + supprimer_rec(R->frere, nbr_supp);

	if (R->prec == NULL && R->frere == NULL) {  		
		R->pere->fils = NULL;
	}

	else if (R->prec != NULL && R->frere == NULL) {   
		R->prec->frere = NULL;
	}
	
	else {										    
		R->prec->frere = R->frere;    					  
		R->frere->prec = R->prec;
	}
	
	R->pere->nbr_fils--;
	R->pere = NULL;
	free(R);
	return nbr_supp;

}

//####################################################################################

void inserer(char * chemin) {

	struct resultat * new, * courant, * prec;
	int l;

	new = (struct resultat *) malloc(sizeof(struct resultat));
	strcpy(new->p, &chemin[1]);
	prec = NULL;
	courant = resultats;

	while (courant != NULL && strcmp(new->p, courant->p) > 0) {
		 prec = courant;
		 courant = courant->suivant;
	}

	if (prec == NULL) {
		new->suivant = resultats;
		resultats = new;
	}
	else {
		prec->suivant = new;
		new->suivant = courant;
	}

	return;
}

//####################################################################################

void trouver(noeud * T, char * nom, char * chemin_temp) {
	
	char chemin_suivant[LON_NOM_CHEMIN];
	
	if (strcmp(T->nom, nom) == 0) {
		//printf("ok %s\n", T->path);;
		strcpy(chemin_suivant, chemin_temp);
		strcat(chemin_suivant, "/");
		strcat(chemin_suivant, nom);
		inserer(chemin_suivant);
		return;
	}
	
	if (T->frere != NULL) {
		trouver(T->frere, nom, chemin_temp);
	}

	if (T->fils != NULL) {
		strcpy(chemin_suivant, chemin_temp);
		strcat(chemin_suivant, "/");
		strcat(chemin_suivant, T->nom);
		trouver(T->fils, nom, chemin_suivant);
	}
	
	return;
}

//####################################################################################

struct resultat * afficher_liberer(struct resultat * L) {
	struct resultat * prec;

	prec = NULL;
	while (L != NULL) {
		printf("Résultat: %s\n", L->p);
		prec = L;
		L = L->suivant;
		free(prec);
	}

	if ( L == NULL)
		return L;
	else
		return NULL;
}

//####################################################################################

int main(int argc, char * argv[]) {

		char nom[LON_NOM];
		char chemin[LON_NOM_CHEMIN];
		char cmd[LON_CMD];
		char chemin_temp[LON_NOM_CHEMIN];
		int longueur_chemin = 0;
		int nbr_trouve = 0;
		char contenu[LON_CONTENU];
		int octet_ecrit;
		int nbr_supp = 0;
		resultat res;
		noeud * R;
		
		resultats = NULL;
		
		racine = creer_element(racine, NULL, "", TYPE_REP);
		if (racine == NULL) {
			return 0;
		}

		while (strcmp(cmd, "sortir") != 0) {
			printf("\033[01;33m");
			printf("ENIM");
			printf("\033[1;34m");
			printf("FS");
			printf("\033[0m");
			printf("-$ ");
			scanf("%s", cmd);

			if (strcmp("sortir", cmd) != 0) {

				scanf("%s", chemin);
				
				if (strcmp("trouver", cmd) != 0) {
					extraire_nom(chemin, nom);
					longueur_chemin = calc_lon_chemin(chemin);

					if (longueur_chemin > LON_CHEMIN || strcmp(chemin, "/") == 0 || chemin[0] != '/' || strcmp(nom, "") == 0) {
						printf("Erreur: vérifier le chemin.\n");
					}

					else {
						
						if (strcmp(cmd, "creer") == 0) {
							res = creer(nom, chemin, longueur_chemin, TYPE_FICH);
							if (res == OK) {
								printf("\033[1;32m");
								printf("✓ ");
								printf("\033[0m");
								printf("Fichier créé avec succès.\n");                     
							}
							else {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin et le nom du fichier.\n"); 
							}                    
						}

						else if (strcmp(cmd, "creer_rep") == 0) {
							res = creer(nom, chemin, longueur_chemin, TYPE_REP);
							if (res == OK) {
								printf("\033[1;32m");
								printf("✓ ");
								printf("\033[0m");
								printf("Répertoire créé avec succès.\n");                      
							}
							else {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin et le nom du répertoire.\n");
							}                      
						}

						else if (strcmp(cmd, "lire") == 0) {
							res = lire(chemin, nom, contenu);
							if (res == OK) {
								if (strlen(contenu) == 0)
									printf("(Vide) \n");
								else
									printf("Contenu: %s\n", contenu); 
							}
							else {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin et le nom du fichier.\n");                      
							}
						}

						else if (strcmp(cmd, "ecrire") == 0) {
							//scanf("%s", contenu);        

															  
							int i = 0;
							char c;
							c = getchar(); // premier espace
							c = '\0';
							while (c != '\n') {
								c = getchar();
								if (c != '"') {
									contenu[i] = c;
									i++;
								}
							}
							contenu[i] = '\0';
							octet_ecrit = ecrire(chemin, nom, contenu);
							if (octet_ecrit != -1) {
								printf("\033[1;32m");
								printf("✓ ");
								printf("\033[0m");
								printf("Ecriture réussie: %d\n", octet_ecrit); 
							}
							else {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin et le nom du fichier.\n");                   
							}
						}

						else if (strcmp(cmd, "supprimer") == 0) {
							res = supprimer(chemin, nom);
							if (res == OK) {
								printf("\033[1;32m");
								printf("✓ ");
								printf("\033[0m");
								printf("Le fichier est supprimé avec succès.\n");                   
							}
							else {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin et le nom du fichier.\n");                   
							}
						}

						else if (strcmp(cmd, "supprimer_rec") == 0) {
							R = positionner(chemin);
							if (R == NULL) {
								printf("\033[1;31m");
								printf("✗ ");
								printf("\033[0m");
								printf("Erreur: vérifier le chemin de l'élément.\n");
							}
							else {
								if (R->fils != NULL)
									nbr_supp = supprimer_rec(R->fils, 0);
								res = supprimer(chemin, nom);
								if (res == OK) {
									printf("\033[1;32m");
									printf("✓ ");
									printf("\033[0m");
									printf("Elément supprimé avec succès.\n");   
								}                
								else {
									printf("\033[1;31m");
									printf("✗ ");
									printf("\033[0m");
									printf("Erreur: vérifier le chemin et le nom de l'élément.\n"); 				
								}
							}
						}
					}
				}

				else if (strcmp(cmd, "trouver") == 0) {
							strcpy(chemin_temp, "");
							trouver(racine, chemin, chemin_temp);     

							if (resultats == NULL)  				  
								printf("Aucun élément avec ce nom n'a été trouvé.\n");
							else
								resultats = afficher_liberer(resultats);				 
						}

					}
				}

		return 0;

}
