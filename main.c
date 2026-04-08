#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PARKINGS 2000

typedef struct {
    char identifiant[100];
    char NOM[100];
    char Adresse[100];
    char Ville[100];
    char Etat[100];
    int place_disponibles;
    int capacite_maximale;
    char Date_de_mise_a_jour[100];
    char affichage_panneau[100];
} parking;

int lesparkings(const char *nomFichier, parking tab[], int max, int *nb) {
    FILE *f = fopen(nomFichier, "r");
    if (f==NULL) {
        printf("Erreur ouverture fichier\n");
        return 0;
    }

    char ligne[2000];
    int count = 0;

    fgets(ligne, sizeof(ligne), f);

    while (count < max && fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\r\n")] = '\0';
        if (sscanf(ligne,
                   "%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%d;%d;%99[^;];%99[^;]",
                   tab[count].identifiant,
                   tab[count].NOM,
                   tab[count].Adresse,
                   tab[count].Ville,
                   tab[count].Etat,
                   &tab[count].place_disponibles,
                   &tab[count].capacite_maximale,
                   tab[count].Date_de_mise_a_jour,
                   tab[count].affichage_panneau) == 9) {
            count++;
        }
    }

    fclose(f);
    *nb = count;
    return 1;
}


void afficheparking(parking tab[], int nb, char idRecherche[]) {
    int trouve = 0;
    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].identifiant, idRecherche) == 0) {
            printf("\n--- PARKING ---\n");
            printf("Identifiant : %s\nNom : %s\nAdresse : %s\nVille : %s\nEtat : %s\n",
                   tab[i].identifiant, tab[i].NOM, tab[i].Adresse, tab[i].Ville, tab[i].Etat);
            printf("Places disponibles : %d / %d\n", tab[i].place_disponibles, tab[i].capacite_maximale);
            printf("Date mise à jour : %s\nAffichage panneau : %s\n",
                   tab[i].Date_de_mise_a_jour, tab[i].affichage_panneau);
            trouve = 1;
            break;
        }
    }
    if (!trouve) printf("Parking introuvable.\n");
}


void afficheparkings(parking tab[], int nb) {
    int i;     for(i = 0; i < nb; i++)     {
        printf("Identifiant : %s\n", tab[i].identifiant);
        printf("Nom : %s\n", tab[i].NOM);
        printf("Adresse : %s\n", tab[i].Adresse);
        printf("Ville : %s\n", tab[i].Ville);
        printf("Etat : %s\n", tab[i].Etat);
        printf("Places disponibles : %d\n", tab[i].place_disponibles);
        printf("Capacite maximale : %d\n", tab[i].capacite_maximale);
        printf("Date de mise a jour : %s\n", tab[i].Date_de_mise_a_jour);
        printf("Affichage panneau : %s\n", tab[i].affichage_panneau);
        printf("-----------------------------\n");     } }


void suiviClient(const char *nomFichier, char plaque[], double montant, time_t entree, time_t sortie) {
    FILE *f = fopen(nomFichier, "a");
    if (!f) {
        printf("Erreur ouverture fichier client.\n");
        return;
    }

    fprintf(f, "Plaque : %s\nMontant payé : %.2f euros\nHeure entrée : %sHeure sortie : %s-----------------------------\n",
            plaque, montant, ctime(&entree), ctime(&sortie));

    fclose(f);
}


void sortieParking(parking tab[], int nb, char idRecherche[], char plaque[], time_t entree) {
    int trouve = 0;
    time_t sortie;
    time(&sortie);
    double duree, montant;

    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].identifiant, idRecherche) == 0) {
            trouve = 1;
            duree = difftime(sortie, entree);
            montant = (duree / 3600.0) * 2.0;

            printf("\n--- SORTIE VEHICULE ---\nParking : %s\nPlaque : %s\nDuree : %.0f secondes\nMontant a payer : %.2f euros\n",
                   tab[i].NOM, plaque, duree, montant);

            suiviClient("clients.txt", plaque, montant, entree, sortie);
            break;
        }
    }
    if (!trouve) printf("Parking introuvable.\n");
}


void mettreAJourOccupation(parking tab[], int nb, char idRecherche[], int entree) {
    int trouve = 0;

    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].identifiant, idRecherche) == 0) {
            trouve = 1;
            if (entree) {
                if (tab[i].place_disponibles > 0) {
                    tab[i].place_disponibles--;
                    printf("Vehicule entre dans %s. Places restantes : %d\n", tab[i].NOM, tab[i].place_disponibles);
                } else {
                    printf("Le parking est plein.\n");
                }
            } else {
                if (tab[i].place_disponibles < tab[i].capacite_maximale) {
                    tab[i].place_disponibles++;
                    printf("Vehicule sorti de %s. Places disponibles : %d\n", tab[i].NOM, tab[i].place_disponibles);
                } else {
                    printf("Erreur : places déjà au maximum.\n");
                }
            }
            break;
        }
    }

    if (!trouve) printf("Parking introuvable.\n");
}


int modeAdministrateur() {
    int code;
    const int codeSecret = 1234;
    printf("Entrez le code administrateur : ");
    scanf("%d", &code);

    if (code == codeSecret) {
        printf("Accès administrateur autorisé.\n");
        return 1;
    } else {
        printf("Code incorrect. Accès refusé.\n");
        return 0;
    }
}


int verifierPlacesDisponibles(parking tab[], int nb, char idRecherche[]) {
    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].identifiant, idRecherche) == 0) {
            if (tab[i].place_disponibles == 0) return 1;
            else return 0;
        }
    }
    return -1;
}


void sauvegarderEtatParking(const char *nomFichier, parking tab[], int nb) {
    FILE *f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur ouverture fichier sauvegarde.\n");
        return;
    }

    fprintf(f, "Identifiant;Nom;Adresse;Ville;Etat;Places disponibles;Capacite max;Date mise à jour;Affichage panneaux\n");

    for (int i = 0; i < nb; i++) {
        fprintf(f, "%s;%s;%s;%s;%s;%d;%d;%s;%s\n",
                tab[i].identifiant, tab[i].NOM, tab[i].Adresse,
                tab[i].Ville, tab[i].Etat,
                tab[i].place_disponibles, tab[i].capacite_maximale,
                tab[i].Date_de_mise_a_jour, tab[i].affichage_panneau);
    }

    fclose(f);
    printf("État des parkings sauvegardé.\n");
}


int main() {
    parking tab[MAX_PARKINGS];
    int nb = 0;

    char idRecherche[100], plaque[50];
    time_t entree;
    int accesAdmin, resultatDisponibilite;

    if (!lesparkings("parking-metropole (2).csv", tab, MAX_PARKINGS, &nb))
        return 1;

    printf("Nombre de parkings : %d\n\n", nb);

    printf("Entrez l'identifiant du parking : ");
    scanf("%99s", idRecherche);

    printf("Entrez la plaque du vehicule : ");
    scanf("%49s", plaque);

    resultatDisponibilite = verifierPlacesDisponibles(tab, nb, idRecherche);

    if (resultatDisponibilite == 1) printf("Le parking est plein.\n");
    else if (resultatDisponibilite == 0) printf("Des places sont disponibles.\n");
    else printf("Parking introuvable.\n");

    mettreAJourOccupation(tab, nb, idRecherche, 1);

    time(&entree);

    printf("\n===== PARKING DEMANDÉ =====\n");
    afficheparking(tab, nb, idRecherche);

    printf("\n===== TOUS LES PARKINGS =====\n");
    afficheparkings(tab, nb);

    printf("\nAppuyez sur Entree pour simuler la sortie...");
    getchar(); getchar();

    printf("\n===== SORTIE VEHICULE =====\n");
    sortieParking(tab, nb, idRecherche, plaque, entree);

    mettreAJourOccupation(tab, nb, idRecherche, 0);

    printf("\n===== MODE ADMINISTRATEUR =====\n");
    accesAdmin = modeAdministrateur();
    if (accesAdmin) printf("Bienvenue dans le mode administrateur.\n");
    else printf("Retour au programme normal.\n");

    sauvegarderEtatParking("parking_sauvegarde.csv", tab, nb);

    return 0;
}


On Wed, Apr 8, 2026 at 2:19 PM Coralie Charbit <coralie.charbit@gmail.com> wrote:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

typedef struct {
    char id[100];
    char nom[100];
    char adresse[150];
    char ville[50];
    char etat[20];
    int places_dispo;
    int capacite;
    char date[20];
    char affichage[20];
}Parking;

int lesparkings(const char *nomFichier, Parking tab[], int max, int *n)
{
    FILE *f = fopen(nomFichier, "r");
    if (f == NULL){
        printf("Erreur ouverture fichier\n");
        return 0;
    }

    char ligne[2000];
    int count = 0;

    fgets(ligne, sizeof(ligne), f);

    while (count < max && fgets(ligne, sizeof(ligne), f) != NULL)
    {
        ligne[strcspn(ligne, "\r\n")] = '\0';

        if (sscanf(ligne,
                   "%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%d;%d;%99[^;];%99[^;]",
                   tab[count].id,
                   tab[count].nom,
                   tab[count].adresse,
                   tab[count].ville,
                   tab[count].etat,
                   &tab[count].places_dispo,
                   &tab[count].capacite,
                   tab[count].date,
                   tab[count].affichage) == 9)
        {
            count++;
        }
    }

    fclose(f);
    *n = count;
    return 1;
}

void afficheparking(Parking tab[], int n, int id) {
    for (int i = 0; i < n; i++) {
            // (strcmp(tab[i].id, id) == 0)
        if (strcmp(tab[i].id, id) == 0) {
            printf("id: %d\n", tab[i].id);
            printf("nom: %s\n", tab[i].nom);
            printf("adresse: %s\n", tab[i].adresse);
            printf("capacite: %d\n", tab[i].capacite);
            printf("places_dispo: %d\n", tab[i].places_dispo);
            return;
        }
    }
    printf("Parking non trouve\n");
}

void afficheparkings(Parking tab[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\n---parking %d ---\n", i + 1);
        printf("nom: %s\n", tab[i].nom);
        printf("adresse: %s\n", tab[i].adresse);
        printf("capacite: %d\n", tab[i].capacite);
        printf("places_dispo: %d\n", tab[i].places_dispo);
    }
}

float sortieParking(int id, Parking tab[], int n, char plaque[]) {
    time_t entree, sortie;
    double duree;
    float prix;

    time(&entree);

    printf("Simulation... appuie sur entree pour sortir");
    getchar();

    time(&sortie);

    duree = difftime(sortie, entree) / 60;

    prix = duree * 0.05;

    printf("plaque: %s\n", plaque);
    printf("temps passe: %.2f minutes\n", duree);
    printf("montant a payer: %.2f euros\n", prix);

    return prix;
}

void suiviClient(char plaque[], float montant) {
    FILE *f = fopen("clients.txt","a");

    if (f == NULL) {
        printf("Erreur fichier\n");
        return;
    }

    time_t now;
    time(&now);

    fprintf(f, "plaque: %s | montant: %.2f | date: %s\n",
            plaque, montant, ctime(&now));

    fclose(f);
}

void mettreAJourOccupation(Parking tab[], int n, int id, int entree) {
    for (int i = 0; i < n; i ++) {
        if (tab[i].id == id) {

            if (entree == 1 && tab[i].places_dispo > 0) {
                tab[i].places_dispo--;

            }
            else if (entree == 0) {
                tab[i].places_dispo++;
            }

            return;
        }
    }
}

int modeAdministrateur() {
    int code;
    printf("Entrer code admin: ");
    scanf("%d", &code);

    if (code == 1234) {
        printf("Acces autorise\n");
        return 1;
    } else {
        printf("Acces autorise\n");
        return 0;
    }
}

int verifierPlacesDisponibles(Parking tab[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (tab[i].id == id) {
            if (tab[i].places_dispo == 0) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return -1;
}

void sauvegarderEtatParking(Parking tab[], int n) {
    FILE *f = open("parking-metropole (2).csv", "w");

    if (f == NULL) {
        printf("Erreur fichier\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(f, "%d,%s,%s,%d,%d\n",
                tab[i].id,
                tab[i].nom,
                tab[i].adresse,
                tab[i].capacite,
                tab[i].places_dispo);
    }

    fclose(f);
}

int main() {
    Parking tab[MAX];
    int n = 0;
    int choix;
    int id;
    char plaque[20];
    float montant;

if(lesparkings("parking-metropole (2).csv",tab, 2000, &n) == 0)
{
    return 1;
}

do {
        printf("\n===== MENU PARKING =====\n");
        printf("1. Afficher tous les parkings\n");
        printf("2. Afficher un parking\n");
        printf("3. Entrer un vehicule\n");
        printf("4. Sortir un vehicule\n");
        printf("5. Verifier disponibilite\n");
        printf("6. Mode administrateur\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {

            case 1:
                afficheparkings(tab, n);
                printf("Nombre de parkings charges : %d\n", n);
                break;

            case 2:
                printf("Entrer ID du parking : ");
                scanf("%d", &id);
                while(getchar() != '\n');
                afficheparking(tab, n, id);
                break;

            case 3:
                printf("Entrer ID du parking : ");
                scanf("%d", &id);
                while(getchar() != '\n');

                if (verifierPlacesDisponibles(tab, n, id) == 1) {
                        printf("Parking plein !\n");
                } else {
                    mettreAJourOccupation(tab, n, id, 1);
                    printf("Vehicule entre avec succes\n");
                }
                break;

            case 4:
                printf("Entrer ID du parking : ");
                scanf("%d", &id);
                while(getchar() != '\n');
                printf("Entrer plaque : ");
                scanf("%s", plaque);
                while(getchar() != '\n');

                montant = sortieParking(id, tab, n, plaque);
                mettreAJourOccupation(tab, n, id, 0);
                suiviClient(plaque, montant);
                break;

            case 5:
                printf("Entrer ID du parking : ");
                scanf("%d", &id);
                while(getchar() != '\n');

                if (verifierPlacesDisponibles(tab, n, id) == 1) {
                        printf("Parking plein\n");
                } else {
                    printf("Places disponibles\n");
                }
                break;

            case 6:
                if (modeAdministrateur()) {
                        printf("Mode admin active\n");
                }
                break;

            case 0:
                sauvegarderEtatParking(tab, n);
                printf("Sauvegarde et fermeture...\n");
                break;

                default:
                    printf("Choix invalide\n");
        }

    } while (choix != 0);

    return 0;
}
