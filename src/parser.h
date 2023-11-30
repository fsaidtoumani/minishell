/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023
 
  Parsing de la ligne de commandes utilisateur.
 
 */

#ifndef _PARSER_H
#define _PARSER_H

#define MAX_LINE_SIZE   1024
#define MAX_CMD_SIZE    256

#include "localvariables.h"
/*
  Voir le TP sur les chaînes de caractères.
  Commentaires à compléter.
*/
/*
  Cette fonction enleve les espace en debut et fin de chaine de catactère
  str : chaine de caractere representant notre ligne de commande
*/
int trim(char* str);

/*
  Cette fonction enleve les espaces en doublon dans une chaine de caractère
  str : chaine de caractere representant notre ligne de commande
*/
int clean(char* str);

/*
  Cette fonction sépare une chaine de caractère en fonction d'un séparateur
  str : chaine de caractere representant notre ligne de commande
  s : chaine de caractere representant les caracteres separateurs
  max : taille maximal de notre chaine str
*/
int separate_s(char* str, char* s, size_t max);

/*
  Cette fonction remplace les variables d'environnement par leurs valeurs
  str : chaine de caractere representant notre ligne de commande
  max : taille maximal de notre chaine str
  variableslocal : pointeur vers la racine de l'arbre des variables locales
*/
int substenv(char* str, size_t max, AVLvariables* variableslocal);

/*
  Cette fonction decoupe la chaine str pour la mettre dans token en les separant par un caractere
  str : chaine de caractere representant notre ligne de commande
  sep : caractere permettant de separer notre chaine
  token : le tableau des éléments de la ligne de
          commandes (peut être modifié)
  max : taille maximal de notre chaine str
*/
int strcut(char* str, char sep, char** tokens, size_t max);

// Optionnel
int strcut_s(char* str, char sep, char** tokens, size_t max);
/*
  Cette fonction sert a remplacer les jokers par les fichiers correspondant
  srt : chaine de caractere representant notre ligne de commande
  max : taille maximal de notre chaine srt
*/
int expand_joker(char *srt, size_t max);
#endif
