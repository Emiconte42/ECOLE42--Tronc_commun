//GNL.h :
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

// librairie
# include <stdlib.h>
# include <unistd.h>
# include <stddef.h>
# include <fcntl.h>
#include "get_next_line.h"

static char *ft_strchr(char *s, int c) // MODIF: static
{
	int i = 0;

	// MODIF: proteger contre la lecture hors limites
	while (s[i] && s[i] != c)
		i++;

	if (s[i] == c)
		return (s + i);

	return (NULL); // MODIF: supp le else
}

static void	*ft_memcpy(void *dest, const void *src, size_t n) // MODIF: static
{
	size_t i = 0; // MODIF: initialisation

	// MODIF COMPLETE: copie normale (l'ancienne version etait bug)
	while (i < n)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i++;
	}
	return (dest);
}

static size_t ft_strlen(char *s) // MODIF: static
{
	size_t	ret = 0;

	while (*s)
	{
		s++;
		ret++;
	}
	return (ret);
}

static int str_append_mem(char **s1, char *s2, size_t size2) // MODIF: static
{
	size_t size1; // MODIF: supp du strlen

	if (*s1) // MODIF: protection
		size1 = ft_strlen(*s1);
	else // MODIF: protection
		size1 = 0;

	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return (0);

	// MODIF: copier seulement si *s1 existe
	if (*s1)
		ft_memcpy(tmp, *s1, size1); // MODIF: decaler la fonction pour aller dans le if

	ft_memcpy(tmp + size1, s2, size2);
	tmp[size1 + size2] = 0;

	free(*s1);
	*s1 = tmp;
	return (1);
}

static int str_append_str(char **s1, char *s2) // MODIF: static
{
	return (str_append_mem(s1, s2, ft_strlen(s2)));
}

static void *ft_memmove(void *dest, const void *src, size_t n) // MODIF: static + reecriture complete du code
{
	size_t i;

	if (dest == src) // MODIF
		return (dest);

	// MODIF: vrai memmove, l'ancien est pourri 
	if (dest > src)
	{
		i = n;
		while (i > 0)
		{
			((char *)dest)[i - 1] = ((char *)src)[i - 1];
			i--;
		}
	}
	else
	{
		i = 0;
		while (i < n)
		{
			((char *)dest)[i] = ((char *)src)[i];
			i++;
		}
	}
	return (dest);
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = "";
	char *ret = NULL;
	char *tmp = ft_strchr(b, '\n');

	while (!tmp)
	{
		// MODIF: ajouter seulement si b contient qqch
		if (*b)
			if (!str_append_str(&ret, b)) // MODIF : decaler la fonction pour aller dans le if
				return (NULL); // MODIF : decaler la fonction pour aller dans le if

		int read_ret = read(fd, b, BUFFER_SIZE);

		if (read_ret < 0) // MODIF : gestion erreur
		{
			free(ret);
			return (NULL);
		}

		if (read_ret == 0) // MODIF: gestion EOF
		{
			if (ret && *ret)
			{
				b[0] = 0; // MODIF: reset le buffer a 0
				return (ret);
			}
			return (NULL);
		}

		b[read_ret] = 0;

		tmp = ft_strchr(b, '\n'); // MODIF: recheck le \n
	}

	if (!str_append_mem(&ret, b, tmp - b + 1))
	{
		free(ret);
		return (NULL);
	}

	// MODIF: garder le reste du buffer
	ft_memmove(b, tmp + 1, ft_strlen(tmp + 1) + 1);

	return (ret);
}

// pour test 
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
	int fd = open("test.txt", O_RDONLY);
	char *line;

	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
