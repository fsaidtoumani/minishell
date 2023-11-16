/*
  Projet minishell - Licence 3 Info - PSI 2023
 
  Nom : SAID TOUMANI |
  Prénom :Fahardine |
  Num. étudiant :22000032 | ;
  Groupe de projet :
  Date :09/11/2023 |
 
  Parsing de la ligne de commandes utilisateur (implémentation).
 
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int trim(char* str) {
  char *end;
  //On decalle le pointeur src jusqu'au premier caractere non "blanc"
  while (*str == ' ' || *str == '\t'){
    str++;
  }

  // On supprime les espaces en fin de chaine
  end = &str[strlen(str) - 1];
  while (end > str && (*end == ' ' || *end == '\t')) --end;
  *(end + 1) = '\0';
  return 0;
}

int clean(char* str) {

  char *dest = str, *src = str;
  char c_preced = 0;
  // Move non-white space characters
  while (*src) {
    if (!(*src == ' ' || *src == '\t')) {
      // Copy non-whitespace characters
      c_preced = *src;
      *dest++ = *src++;
    }
    else if (c_preced && !(c_preced == ' ' || c_preced == '\t')) {
      // On copie un seul espace
      c_preced = *src;
      *dest++ = ' ';
      while (*src == ' ' || *src == '\t') ++src;  // On se place sur le prochain caractere non "blanc" 
    }
  }
  *dest = '\0'; // On termine la chaine
  return 0;
}

int separate_s(char* str, char* s, size_t max) {
  /*
    Verifie que les premier caractere de la chaine ne soit pas un separateur
    Si c'est le cas on depalce la chaine de caractere j'usqu'au premier caractere valide
  */
 /* V1
  for (int i=0;i < strlen(s);i++){
    if (*str == s[i])  {
      memmove(str, str+1, strlen(str));
      i=0;
      str--;
    }
  }
  */
  while (*str != '\0' && strchr(s, *str) != NULL) {
    memmove(str, str + 1, strlen(str));
  }
  
  if(strlen(str) < 1)
  {
    //printf("Erreur: la chaine de caractère est vide\n");
    return -2;
  }
  while (*str != '\0' )
  {
    if(strlen(str) > max)
    {
      //printf("Erreur: la chaine de caractère est trop longue\n");
      return -1;
    }
    for (int i=0;i < strlen(s);i++)
    {
      if (*str == s[i])
      {
        if(!(*(str-1) == ' ') && !(*(str-1) == s[i]))
        {
          memmove(str+1, str, strlen(str));
          *str = ' ';
        }
        if(!(*(str+1) == ' ') && !(*(str+1) == s[i]))
        {
          memmove(str+2, str+1, strlen(str));
          *(str+1)= ' ';          
        }
        break;
      }
    }
    str++;
  }
  //printf("chaine de caractère: %s\n", str);
  return 0; 
}

int substenv(char* str, size_t max) {

  return 0;
}

int strcut(char* str, char sep, char** tokens, size_t max) {
  int nb_token = 1;
    //int size = strlen(str);
    tokens[0]=str;
    for (int i = 0; str[i] != '\0' ; i++)
    {
        if(str[i] == sep)
        {
            str[i] = '\0';

            tokens[nb_token] = (str+i+1);

            nb_token++;
        }
    }
    // str[ nb_token] = (str+i+1);
    tokens[nb_token] = NULL;
    return nb_token;
}


// Optionnel
int strcut_s(char* str, char sep, char** tokens, size_t max) {

  size_t i = 0; // Indice pour les tokens
  size_t tokenLength = 0; // Longueur du token courant
  char* tokenStart = str; // Début du token courant

  while (*str != '\0') {
    // Lorsque le séparateur est trouvé ou que nous sommes à la fin de la chaîne
    if (*str == sep || *(str + 1) == '\0') {
      // Si on est la fin et que ce n'est pas un séparateur, il faut incrémenter la longueur du token
      tokenLength = (*str == sep) ? (size_t)(str - tokenStart) : (size_t)(str - tokenStart + 1);

      if (tokenLength >= max) {
        // Si la longueur du token est trop grande, nous avons un problème.
        return -1;
      }

      // Allouer de la mémoire pour le token actuel
      tokens[i] = malloc(tokenLength + 1); // +1 pour le caractère nul
      if (tokens[i] == NULL) {
        // Gestion d'erreur pour l'allocation de mémoire
        return -2;
      }

      // Copier le token dans le tableau de tokens
      strncpy(tokens[i], tokenStart, tokenLength);
      tokens[i][tokenLength] = '\0'; // Ajouter la terminaison nulle

      i++; // Passer au token suivant

      // Reset pour le prochain token
      tokenStart = str + 1;
      tokenLength = 0;
    } else {

      tokenLength++;
    }

    // Passer au caractère suivant dans la chaîne d'entrée
    str++;
  }
  tokens[i] = NULL; // Terminer le tableau de tokens par NULL
  // Retourner le nombre de tokens
  return i;
}
