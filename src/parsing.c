/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 18:54:10 by alvanaut          #+#    #+#             */
/*   Updated: 2025/07/31 18:56:07 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//Prendre les arg et les split a chaque separateur
//Regarder de quel est le type du separateur
//Rassembler par ligne de commande

void    free_split(char **s, size_t index)
{
    size_t i;

    i = 0;
    while (i < index)
    {
        free(s[i]);
        i++;
    }
    free(s);
}

size_t count_arg(int ac, char **av)
{
    size_t i;
    size_t j;
    char **tmp;
    size_t count;

    i = 1;
    count = 0;
    while (i < (size_t)ac)
    {
        tmp = ft_split(av[i], ' ');
        if (!tmp)
            return 0;
        j = 0;
        while (tmp[j])
        {
            j++;
            count++;
        }
        free_split(tmp, j);
        i++;
    }
    return (count);
}


char **args_splitted(int ac, char **av)
{
    char **splitted;
    char **tmp;
    size_t i;
    size_t j;
    size_t k;

    splitted = malloc(sizeof(char *) * count_arg(ac, av) + 1);
    if (!splitted)
        return (printf("Error: allocated splitted\n"), NULL);
    i = 1;
    k = 0;
    while (i < (size_t)ac)
    {
        tmp = ft_split(av[i], ' ');
        if (!tmp)
            return (printf("Error: args_splited\n"), NULL);
        j = 0;
        while (tmp[j])
        {
            splitted[k] = ft_strdup(tmp[j]);
            if (!splitted[k])
                return (printf("Error: args_splited\n"), NULL);
            k++;
            j++;
        }
        free_split(tmp, j);
    }
    splitted[k] = NULL;
    return (splitted);
}
