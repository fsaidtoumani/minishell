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

#ifndef LOCALVARIABLES_H
#define LOCALVARIABLES_H
#include "localvariables.h"
/*
    Cette structure permet de stocker les variables locales.
    Et pour cela, nous avons utilisé un arbre AVL.

    nom : nom de la variable
    valeur : valeur de la variable
    fgauche : pointeur vers le fils gauche
    fdroit : pointeur vers le fils droit
    hauteur : hauteur du noeud
*/

typedef struct AVLvariables {
    char* nom;
    char* valeur;
    struct AVLvariables* fgauche;
    struct AVLvariables* fdroit;
    int hauteur;
} AVLvariables;

/*
    Fonction max :
    Cette fonction permet de donner le maximum entre deux entiers.
    a : premier entier à comparer
    b : deuxième entier à comparer
    Elle retourne le maximum entre a et b.
*/

int max(int a, int b);

/*
    Fonction creenouveauneaud :
    Cette fonction permet de créer un noeud de l'arbre AVL.
    nom : nom de la variable à stocker
    valeur : valeur de la variable à stocker
    Elle retourne un pointeur AVLvariables vers le noeud créé.
*/

AVLvariables* creenouveauneaud(char* nom, char* valeur);

/*
    Fonction hauteur :
    Cette fonction permet de donner la hauteur d'un noeud.
    node : pointeur vers le noeud dont on veut la hauteur
    Elle retourne la hauteur du noeud.
*/

int hauteur(AVLvariables* node);

/*
    Fonction avlequilibre :
    Cette fonction permet de donner la balance d'un noeud.
    node : pointeur vers le noeud dont on veut la balance
    Elle retourne la l'equilibre de l'AVL.
*/
int avlequilibre(AVLvariables* node);

/*
    Fonction roationdroite :
    Cette fonction permet de faire une rotation simple droite.
    y : pointeur vers le noeud à faire tourner
    Elle retourne un pointeur vers le noeud tourné.
*/

AVLvariables* roationdroite(AVLvariables* y);

/*
    Fonction roationgauche :
    Cette fonction permet de faire une rotation simple gauche.
    x : pointeur vers le noeud à faire tourner
    Elle retourne un pointeur vers le noeud tourné.
*/
AVLvariables* roationgauche(AVLvariables* x);

/*
    Cette fonction permet de créer un noeud de l'arbre AVL.
    Elle prend en paramètre une AVLvariables le nom et la valeur de la variable.
    Elle retourne un pointeur vers le noeud créé.
*/
AVLvariables *insertAVLvariables(AVLvariables* variable, char* nom, char* valeur);

/*
    Fonction rechercheAVLvariables :
    Cette fonction permet de rechercher une variable dans l'arbre AVL.
    noeud : pointeur vers la racine de l'arbre
    nom : nom de la variable à rechercher
    Elle retourne un pointeur vers le contenu la variable recherchée.
*/
AVLvariables* rechercheAVLvariables(AVLvariables* noeud, char* nom);





#endif