/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023
 
  //Bonus :
  Gestion des variables local.
 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "localvariables.h"

int max(int a, int b) {
    return (a > b) ? a : b;
}


AVLvariables* creenouveauneaud(char* nom, char* valeur) {
    AVLvariables* noeud = (AVLvariables*)malloc(sizeof(AVLvariables));
    noeud->nom = strdup(nom);
    noeud->valeur = strdup(valeur);
    noeud->fgauche = NULL;
    noeud->fdroit = NULL;
    noeud->hauteur = 1;  // Nouveau nœud ajouté à la feuille
    return noeud;
}



int hauteur(AVLvariables* noeud) {
    if (noeud == NULL)
        return 0;
    return noeud->hauteur;
}


int avlequilibre(AVLvariables* noeud) {
    if (noeud == NULL)
        return 0;
    return hauteur(noeud->fgauche) - hauteur(noeud->fdroit);
}

AVLvariables* roationdroite(AVLvariables* y) {
    AVLvariables* x = y->fgauche;
    AVLvariables* T2 = x->fdroit;

    // Effectuer la rotation
    x->fdroit = y;
    y->fgauche = T2;

    // Mettre à jour les hauteurs
    y->hauteur = max(hauteur(y->fgauche), hauteur(y->fdroit)) + 1;
    x->hauteur = max(hauteur(x->fgauche), hauteur(x->fdroit)) + 1;

    // Retourner la nouvelle racine
    return x;
}


AVLvariables* roationgauche(AVLvariables* x) {
    AVLvariables* y = x->fdroit;
    AVLvariables* T2 = y->fgauche;

    // Effectuer la rotation
    y->fgauche = x;
    x->fdroit = T2;

    // Mettre à jour les hauteurs
    x->hauteur = max(hauteur(x->fgauche), hauteur(x->fdroit)) + 1;
    y->hauteur = max(hauteur(y->fgauche), hauteur(y->fdroit)) + 1;

    // Retourner la nouvelle racine
    return y;
}


AVLvariables* insertAVLvariables(AVLvariables* noeud, char* nom, char* valeur) {
    // 1. Insertion standard BST
    if (noeud == NULL) return creenouveauneaud(nom, valeur);

    //Si le nom existe déjà, on met à jour la valeur
    AVLvariables* variable = rechercheAVLvariables(noeud,nom);
    if(variable != NULL)
    {
        variable->valeur = strdup(valeur);
        return noeud;
    }

    if (strcmp(nom, noeud->nom) < 0)
        noeud->fgauche = insertAVLvariables(noeud->fgauche, nom, valeur);
    else if (strcmp(nom, noeud->nom) > 0)
        noeud->fdroit = insertAVLvariables(noeud->fdroit, nom, valeur);

    // 2. Mise à jour de la hauteur du nœud actuel
    noeud->hauteur = 1 + max(hauteur(noeud->fgauche), hauteur(noeud->fdroit));

    // 3. Vérification de l'équilibre et rééquilibrage si nécessaire
    int balance = avlequilibre(noeud);

    // Rotation simple gauche
    if (balance > 1 && strcmp(nom, noeud->fgauche->nom) < 0)
        return roationdroite(noeud);

    // Rotation simple droite
    if (balance < -1 && strcmp(nom, noeud->fdroit->nom) > 0)
        return roationgauche(noeud);

    // Rotation gauche-droite
    if (balance > 1 && strcmp(nom, noeud->fgauche->nom) > 0) {
        noeud->fgauche = roationgauche(noeud->fgauche);
        return roationdroite(noeud);
    }

    // Rotation droite-gauche
    if (balance < -1 && strcmp(nom, noeud->fdroit->nom) < 0) {
        noeud->fdroit = roationdroite(noeud->fdroit);
        return roationgauche(noeud);
    }

    // Retourne le nœud (inchangé)
    return noeud;
}


AVLvariables* rechercheAVLvariables(AVLvariables* noeud, char* nom) {
    if (noeud == NULL )
        return NULL;
    if(strcmp(noeud->nom, nom) == 0)
        return noeud;
    if (strcmp(nom, noeud->nom) < 0)
        return rechercheAVLvariables(noeud->fgauche, nom);
    else
        return rechercheAVLvariables(noeud->fdroit, nom);
}
