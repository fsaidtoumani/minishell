/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023

  Parsing de la ligne de commandes utilisateur (implémentation).

 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // isdigit
#include <glob.h>  // glob
#include "parser.h"
#include "localvariables.h"

int trim(char *str)
{
  char *end;
  // On decalle le pointeur src jusqu'au premier caractere non "blanc"
  while (*str == ' ' || *str == '\t')
  {
    str++;
  }

  // On supprime les espaces en fin de chaine
  end = &str[strlen(str) - 1];
  while (end > str && (*end == ' ' || *end == '\t'))
    --end;
  *(end + 1) = '\0';
  return 0;
}

int clean(char *str)
{

  char *dest = str, *src = str;
  char c_preced = 0;
  int flagdoublecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas
  int flagsimplecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas

  // On decalle le pointeur src jusqu'au premier caractere non "blanc"
  while (*src)
  {
    if (*src == '"')
    {
      flagdoublecote=!flagdoublecote;
    }
    if (*src == '\'')
    {
      flagsimplecote=!flagsimplecote;
    }

    if (!(*src == ' ' || *src == '\t') || flagdoublecote || flagsimplecote)
    {
      // On copie le caractere precedent
      c_preced = *src;
      *dest++ = *src++;
    }
    else if (c_preced && !(c_preced == ' ' || c_preced == '\t'))
    {
      // On copie un seul espace
      c_preced = *src;
      *dest++ = ' ';
      while (*src == ' ' || *src == '\t')
        ++src; // On se place sur le prochain caractere non "blanc"
    }
  }
  *dest = '\0'; // On termine la chaine
  return 0;
}

int separate_s(char *str, char *s, size_t max)
{
  int flagdoublecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas
  int flagsimplecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas
  // printf("chaine de caractère: %s\n", temp);
  while (*str != '\0' && strchr(s, *str) != NULL)
  {
    memmove(str, str + 1, strlen(str));
  }

  if (strlen(str) < 1)
  {
    // printf("Erreur: la chaine de caractère est vide\n");
    return -2;
  }
  while (*str != '\0')
  {
    // On regarde si on est pas dans une chaine de caractère
    if (*str == '"')
    {
      flagdoublecote = !flagdoublecote;
    }

    if (*str == '\'')
    {
      flagsimplecote = !flagsimplecote;
    }

    if(flagdoublecote || flagsimplecote)
    {
      str++;
      continue;
    }

    if (strlen(str) > max)
    {
      // printf("Erreur: la chaine de caractère est trop longue\n");
      return -1;
    }
    // On traite pour les descriptions de fichiers
    if (*str == '>' || *str == '<')
    {
      // On regarde le avant le caracteur ex 2>..
      if (!(*(str - 1) == ' ') && !(*(str - 2) == ' '))
      {
        memmove(str + 1, str, strlen(str));
        *str = ' ';
        str++;
      }

      // On traite pour les descriptions de fichiers de type >&1
      if (*(str + 1) == '&' && isdigit(*(str + 2)) && ((*(str + 3) == ' ') || (*(str + 3) == '\0')))
      {
        // Si ce qui suis n'est un espace ou la fin de la chaine on rajoute un espace
        str = str + 3;
      }
      // On regarde le apres le caracteur ex ..>2
      else if (!(*(str + 1) == ' ') && !(*(str + 2) == ' '))
      {
        memmove(str + 2, str + 1, strlen(str));
        *(str + 1) = ' ';
        str++;
      }
    }
    else
    {
      // On regarde si le caracteur n'est pas un caracteur special a rajouter des espaces
      for (int i = 0; i < strlen(s); i++)
      {
        if (*str == s[i])
        {
          //                                               On regarde si c'est pas un descripteur de fichier
          if (!(*(str - 1) == ' ') && !(*(str - 1) == s[i]))
          {
            memmove(str + 1, str, strlen(str));
            *str = ' ';
          }

          if (!(*(str + 1) == ' ') && !(*(str + 1) == s[i]))
          {
            memmove(str + 2, str + 1, strlen(str));
            *(str + 1) = ' ';
          }
          break;
        }
      }
    }
    str++;
  }
  // printf("chaine de caractère: %s\n", temp);
  return 0;
}

int substenv(char *str, size_t max, AVLvariables *variableslocal)
{
  int i = 0;
  while (*str != '\0' || i < max)
  {
    if (*str == '$')
    {
      // On copie le nom de la variable dans un char temp
      int i = 1, j = 0;
      char tempnomv[100];
      while (*(str + i) != ' ' && *(str + i) != '$' && *(str + i) != '\0')
      {
        tempnomv[j] = *(str + i);
        i++;
        j++;
      }
      tempnomv[j] = '\0';
      char *env = getenv(tempnomv);
      if (env != NULL)
      {
        memmove(str, env, strlen(env));
        // On termine la chaine
        *(str + strlen(env)) = '\0';
        str += strlen(env);
      }
      else
      {
        // On regarde si la variable existe dans les variables locales
        if(rechercheAVLvariables(variableslocal,tempnomv) != NULL)
        {
          memmove(str, rechercheAVLvariables(variableslocal,tempnomv)->valeur, strlen(rechercheAVLvariables(variableslocal,tempnomv)->valeur));
          // On termine la chaine
          *(str + strlen(rechercheAVLvariables(variableslocal,tempnomv)->valeur)) = '\0';
          str += strlen(rechercheAVLvariables(variableslocal,tempnomv)->valeur);
        }
        else
        {
          //On substitue le nom de variable par ""
          int len = 0;
          while (*(str + len) != ' ' && *(str + len) != '\0')
          {
            len++;
          }
          *str = '"';
          str++;
          *str = '"';
          str++;
          *str = ' ';
          memmove(str, str + len - 2, strlen(str + len));
          *(str + strlen(str + len)) = '\0';
          return -1;
        }
      }
    }
    str++;
    i++;
  }
  return 0;
}

int strcut(char *str, char sep, char **tokens, size_t max)
{
  int nb_token = 1;
  int flagdoublecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas
  int flagsimplecote = 0; // Ca nous permet de savoir si on est dans une chaine string ou pas
  // int size = strlen(str);
  tokens[0] = str;
  for (int i = 0; str[i] != '\0' || i<max ; i++)
  {
    if (str[i] == '"')
    {
      flagdoublecote = !flagdoublecote;
    }

    if (str[i] == '\'')
    {
      flagsimplecote = !flagsimplecote;
    }

    if(str[i]=='\\') //Pour gere les echappement
    {
      i=i+2;
    }

    if (str[i] == sep && flagdoublecote == 0 && flagsimplecote == 0)
    {
      str[i] = '\0';

      tokens[nb_token] = (str + i + 1);

      nb_token++;
    }
  }
  // str[ nb_token] = (str+i+1);
  tokens[nb_token] = NULL;
  return nb_token;
}

// Optionnel
int strcut_s(char *str, char sep, char **tokens, size_t max)
{

  size_t i = 0;           // Indice pour les tokens
  size_t tokenLength = 0; // Longueur du token courant
  char *tokenStart = str; // Début du token courant

  while (*str != '\0')
  {
    // Lorsque le séparateur est trouvé ou que nous sommes à la fin de la chaîne
    if (*str == sep || *(str + 1) == '\0')
    {
      // Si on est la fin et que ce n'est pas un séparateur, il faut incrémenter la longueur du token
      tokenLength = (*str == sep) ? (size_t)(str - tokenStart) : (size_t)(str - tokenStart + 1);

      if (tokenLength >= max)
      {
        // Si la longueur du token est trop grande, nous avons un problème.
        return -1;
      }

      // Allouer de la mémoire pour le token actuel
      tokens[i] = malloc(tokenLength + 1); // +1 pour le caractère nul
      if (tokens[i] == NULL)
      {
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
    }
    else
    {

      tokenLength++;
    }

    // Passer au caractère suivant dans la chaîne d'entrée
    str++;
  }
  tokens[i] = NULL; // Terminer le tableau de tokens par NULL
  // Retourner le nombre de tokens
  return i;
}

// Optionnel
int expand_joker(char *str, size_t max)
{
  char *copy = strdup(str); // Crée une copie de la chaîne
  if (!copy)
  {
    perror("strdup failed");
    return -1;
  }
  char *result = malloc(max); // Chaîne de résultat
  if (!result)
  {
    free(copy);
    perror("malloc failed");
    return -1;
  }
  result[0] = '\0'; // Initialiser la chaîne de résultat

  int suiveur = 0;
  int cpt_espace = 0;

  char *token = strtok(copy, " "); // Découpe la chaîne en tokens

  // Pour ne pas perdre le nombre despace de notre chaine a la base
  while (*(str + suiveur) != ' ' && *(str + suiveur) != '\0')
  {
    suiveur++;
  }

  while (token != NULL)
  {
    // on remets les d'avant
    for (int i = 0; i < cpt_espace; i++)
    {
      strncat(result, " ", max - strlen(result) - 1); // Ajouter un espace entre les tokens
    }
    cpt_espace = 0;
    if (strchr(token, '*') != NULL)
    {

      // On a trouvé un joker
      glob_t results;
      int ret = glob(token, 0, NULL, &results); // On utilise glob
      if (ret == 0)
      {
        for (size_t i = 0; i < results.gl_pathc - 1; i++)
        {
          strncat(result, results.gl_pathv[i], max - strlen(result) - 1);
          strncat(result, " ", max - strlen(result) - 1); // Ajouter un espace entre les noms de fichiers
        }
        strncat(result, results.gl_pathv[results.gl_pathc - 1], max - strlen(result) - 1);
      }
      globfree(&results);
    }
    else
    {
      // Pas de joker, ajouter le token tel quel
      strncat(result, token, max - strlen(result) - 1);
    }
    token = strtok(NULL, " ");
    // Pour ne pas perdre le nombre despace de notre chaine a la base
    while (*(str + suiveur) != ' ' && *(str + suiveur) != '\0')
    {
      suiveur++;
    }

    while (*(str + suiveur) == ' ' && *(str + suiveur) != '\0')
    {
      suiveur++;
      cpt_espace++;
    }

  }

  // Copier la chaîne résultante dans str
  strncpy(str, result, max - 1);
  str[max - 1] = '\0'; // S'assurer que la chaîne est bien terminée

  free(result);
  free(copy);
  return 0;
}