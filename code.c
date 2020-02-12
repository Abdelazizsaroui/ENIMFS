#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PATH_L             255
#define PATH_STRING_L      65280 
#define NAME_L             255
#define DATA_L             255
#define MAX_SONS           1024
#define DIR_T              'D'
#define FILE_T             'F'
#define CMD_L              20             

typedef enum { OK, NO } result;
typedef enum { true, false } boolean;

struct result {
	char p[PATH_STRING_L];
	struct result * next;
};

typedef struct res {
	char type;
	char name[NAME_L];
	//char path[PATH_STRING_L];
	char data[DATA_L];
	struct res * son;
	struct res * bro;
	struct res * prev;
	struct res * father;
	int sonsnum;
} node;

node * root;   		  	    
struct result * results;    

//####################################################################################

node * create_element(node *T, node *F, char *name, char res_type) {
	int i, l;
	T = (node *) malloc(sizeof(node));
	if (T == NULL) {
		return NULL;
	}
	T->sonsnum = 0;
	T->type = res_type;
	strcpy(T->data, "");
	//T->path = (char *) malloc(sizeof(char) * strlen(path));
	//strcpy(T->path, path);
	//T->name = (char *)malloc(sizeof(char) * strlen(name));
	strcpy(T->name, name);
	T->father = F;
	T->son = NULL;
	T->bro = NULL;
	T->prev = NULL;

	return T;
}

//####################################################################################

void extract_name(char * path, char * name) {
	int i, l;
	l = (int) strlen(path);
	i = l - 1;
	while (path[i] != '/') {
		i--;
	}
	if (strlen(path)-i > NAME_L)
		strcpy(name, "");
	else
		strcpy(name, &path[i+1]);
}

//####################################################################################

int calc_path_length(char * s) {
	int i, k = 0;
	for (i = 0; s[i] != '\0'; i++) {
		if (s[i] == '/') {
			k = k + 1;
		}
	}
	return k;
}

//####################################################################################

node * path_travel(char * path) {

	node * t;
	char * token;
	char temp_path[PATH_STRING_L];
	int i = 0;
	boolean found = false;

	t = root;

	strcpy(temp_path, path);
	token = strtok(temp_path, "/");

	while (token != NULL) {  // comment faire i <chemin_longueur // cette boucle déplace t vers l'avant-dernière partie du chemin
		found = false;
		if (t->son != NULL)
			t = t->son;
		else {
			return NULL;
		}
		if (strcmp(token, t->name) == 0)
			found = true;

		while (t->bro != NULL && found == false) {
			t = t->bro;
			if (strcmp(token, t->name) == 0)
				found = true;
		}

		if (found == false)   {  // Je n'ai pas trouvé la ressource demandée


			return NULL;
		}

		token = strtok(NULL, "/");
	}
	
	return t;

}

//####################################################################################

result create(char * name, char * path, int path_length, char res_type) {

	node * t;
	node * new;
	node * f;
	char * token;
	char temp_path[PATH_STRING_L];
	int i = 0;
	boolean found = false;
	boolean free_slot = false;
	t = root;
	f = t;

	strcpy(temp_path, path);
	token = strtok(temp_path, "/");

	new = (node *) malloc(sizeof(node));   	     // J'alloue une nouvelle ressource
	if (new == NULL)
		return NO;

	while (i < path_length-1) {    // ce cycle se déplace vers l'avant-dernière partie de l'itinéraire
		found = false;
		f = t;
		if (t->son != NULL)
			t = t->son;
		if (strcmp(token, t->name) == 0)
			found = true;

		while(t->bro != NULL && found == false) {
			t = t->bro;
			if (strcmp(token, t->name) == 0) {
				found = true;
			}
		}

		if (found == false)   { // chemin invalide, j'essaye de créer un nœud sous un autre inexistant
			return NO;
		}
		else if (found == true && t->type == FILE_T)
			return NO;

		token = strtok(NULL, "/");
		i++;
	}

	if (t->sonsnum == MAX_SONS) {
		return NO;
	}

	if (t->son == NULL) {
		f = t;
		f->sonsnum++;
		new = create_element(t, f, name, res_type);
		t->son = new;
		new->prev = NULL; // déjà fait dans le create_element
		return OK;
	}

	f = t;
	t = t->son;
	if (strcmp(t->name, name) == 0) {
		return NO;
	}
	while(t->bro != NULL) {
		t = t->bro;
		if (strcmp(t->name, name) == 0) {
			return NO;
		}
	}

	f->sonsnum++;
	new = create_element(t, f, name, res_type);
	t->bro = new;
	new->prev = t;
	new->father = f;
	return OK;
}

//####################################################################################

result read_file(char * path, char * name, char * contenuto) {

	node * t;

	t = path_travel(path);

	if (t == NULL) {
		return NO;
	}

	if (strcmp(t->name, name) == 0 && t->type == FILE_T) {
		strcpy(contenuto, t->data);
		return OK;
	}

	return NO;

}

//####################################################################################

int write_file(char * path, char * name, char * contenuto) {

	node * t;

	t = path_travel(path);

	if (t == NULL) {
		return -1;
	}

	if (strcmp(t->name, name) == 0 && t->type == FILE_T) {
		contenuto[strlen(contenuto)-1] = '\0';
		strcpy(t->data, contenuto);                    
		return (int) strlen(t->data);
	}

	return -1;

}

//####################################################################################

result delete(char * path, char * name) {

		node * t;

		t = path_travel(path);
		
		if (t == NULL) {
			return NO;
		}
		
		if (t->type == DIR_T && t->sonsnum != 0) {
			return NO;
		}
		
		if (strcmp(t->name, name) == 0) {
			//printf(" %d ", t->sonsnum);
			//printf(" %s %s %d ", t->name, name, p_l);
			

			if (t->prev == NULL && t->bro == NULL) {			      // le nœud à supprimer est le seul de la liste
				t->father->son = NULL;
			}
			
			else if (t->prev == NULL && t->bro != NULL) {		 	  // le nœud à supprimer est en tête
				t->father->son = t->bro;
				t->bro->prev = NULL;
			}

			else if (t->prev != NULL && t->bro == NULL) {     // le nœud à supprimer est mis en file d'attente
				t->prev->bro = NULL;
			}
			
			else {
				t->prev->bro = t->bro;    					  // le nœud à supprimer est au milieu
				t->bro->prev = t->prev;
			}
			
			t->father->sonsnum--;
			t->father = NULL;
			free(t);
			return OK;
	    }

	return NO;
}

//####################################################################################

int delete_r(node * R, int del_num) {

	if (R->son != NULL)
		del_num = del_num + delete_r(R->son, del_num);

	if (R->bro != NULL)
		del_num = del_num + delete_r(R->bro, del_num);

	if (R->prev == NULL && R->bro == NULL) {  		// le nœud à supprimer est le seul de la liste
		R->father->son = NULL;
	}

	else if (R->prev != NULL && R->bro == NULL) {   // le nœud à supprimer est mis en file d'attente
		R->prev->bro = NULL;
	}
	
	else {										    // le nœud à supprimer est au milieu
		R->prev->bro = R->bro;    					  
		R->bro->prev = R->prev;
	}
	
	R->father->sonsnum--;
	R->father = NULL;
	free(R);
	return del_num;

}

//####################################################################################

void insert_in_order(char * path) {

	struct result * new, * curr, * prev;
	int l;

	new = (struct result *) malloc(sizeof(struct result));
	strcpy(new->p, &path[1]);
	prev = NULL;
	curr = results;

	while (curr != NULL && strcmp(new->p, curr->p) > 0) {
		 prev = curr;
		 curr = curr->next;
	}

	if (prev == NULL) {
		new->next = results;
		results = new;
	}
	else {
		prev->next = new;
		new->next = curr;
	}

	return;
}

//####################################################################################

void find(node * T, char * name, char * temp_path) {
	
	char next_path[PATH_STRING_L];
	
	if (strcmp(T->name, name) == 0) {
		//printf("ok %s\n", T->path);;
		strcpy(next_path, temp_path);
		strcat(next_path, "/");
		strcat(next_path, name);
		insert_in_order(next_path);
		return;
	}
	
	if (T->bro != NULL) {
		find(T->bro, name, temp_path);
	}

	if (T->son != NULL) {
		strcpy(next_path, temp_path);
		strcat(next_path, "/");
		strcat(next_path, T->name);
		find(T->son, name, next_path);
	}
	
	return;
}

//####################################################################################

struct result * print_results_and_free(struct result * L) {
	// Le noeud ongi de la liste des résultats de la recherche contient le chemin complet de la ressource dans le champ 'n'
	struct result * prev;

	prev = NULL;
	// Je fais défiler la liste des résultats de recherche en imprimant et en supprimant chaque nœud
	while (L != NULL) {
		printf("ok %s\n", L->p);
		prev = L;
		L = L->next;
		free(prev);
	}

	if ( L == NULL)
		return L;
	else
		return NULL;
}

//####################################################################################

int main(int argc, char * argv[]) {

		char name[NAME_L];
		char path[PATH_STRING_L];
		char cmd[CMD_L];
		char temp_path[PATH_STRING_L];
		int path_length = 0;
		int found_num = 0;
		char contenuto[DATA_L];
		int written_bytes;
		int deleted_num = 0;
		result ris;
		node * R;
		
		results = NULL;
		
		root = create_element(root, NULL, "", DIR_T);
		if (root == NULL) {
			return 0;
		}

		while (strcmp(cmd, "exit") != 0) {
			scanf("%s", cmd);

			if (strcmp("exit", cmd) != 0) {

				scanf("%s", path);
				
				if (strcmp("find", cmd) != 0) {
					extract_name(path, name);
					path_length = calc_path_length(path);

					if (path_length > PATH_L || strcmp(path, "/") == 0 || path[0] != '/' || strcmp(name, "") == 0) {
						printf("no\n");
					}

					else {
						
						if (strcmp(cmd, "create") == 0) {
							ris = create(name, path, path_length, FILE_T);
							if (ris == OK) {
								printf("ok\n");                     // création de fichier réussie
							}
							else
								printf("no\n");                     // échec de la création du fichier
						}

						else if (strcmp(cmd, "create_dir") == 0) {
							ris = create(name, path, path_length, DIR_T);
							if (ris == OK) {
								printf("ok\n");                      // création de dossier réussie
							}
							else
								printf("no\n");                      // échec de la création du dossier
						}

						else if (strcmp(cmd, "read") == 0) {
							ris = read_file(path, name, contenuto);
							if (ris == OK) {
								if (strlen(contenuto) == 0)
									printf("contenuto \n");
								else
									printf("contenuto %s\n", contenuto); // lecture de fichier réussie
							}
							else
								printf("no\n");                      // échec de la lecture du fichier
						}

						else if (strcmp(cmd, "write") == 0) {
							//scanf("%s", contenuto);        // lit la chaîne à écrire dans le fichier, y compris les espaces, jusqu'au premier '\ n' (ENTER)

															  // (pris comme caractères simples ---> ne mettez pas le '\ 0' seul)
							int i = 0;
							char c;
							c = getchar(); // premier espace
							c = getchar(); // premier "
							c = '\0';
							while (c != '\n') {
								c = getchar();
								if (c != '"') {
									contenuto[i] = c;
									i++;
								}
							}
							contenuto[i] = '\0';
							written_bytes = write_file(path, name, contenuto);
							if (written_bytes != -1) {
								printf("ok %d\n", written_bytes); // écriture de fichier réussie
							}
							else
								printf("no\n");                   // échec d'écriture du fichier
						}

						else if (strcmp(cmd, "delete") == 0) {
							ris = delete(path, name);
							if (ris == OK) {
								printf("ok\n");                   // suppression de fichier réussie
							}
							else
								printf("no\n");                   // échec de suppression de fichier
						}

						else if (strcmp(cmd, "delete_r") == 0) {
							R = path_travel(path);
							if (R == NULL)
								printf("no\n");
							else {
								if (R->son != NULL)
									deleted_num = delete_r(R->son, 0);
								ris = delete(path, name);
								if (ris == OK)
									printf("ok\n");                   // suppression su ressource réussie
								else
									printf("no\n"); 				// échec de suppression du ressource
							}
						}
					}
				}

				else if (strcmp(cmd, "find") == 0) {
							strcpy(temp_path, "");
							find(root, path, temp_path);     // chemin contient le nom des ressources à rechercher

							if (results == NULL)  				  // aucune ressource avec le nom recherché trouvée
								printf("no\n");
							else
								results = print_results_and_free(results);				 
						}

					}
				}

		return 0;

}
