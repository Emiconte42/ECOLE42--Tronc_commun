/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_get_next_line.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 11:31:13 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/19 16:58:04 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*read_and_stash(t_list **gc, int fd, char *stash)
{
	char	*buffer;
	int		len;
	char	*tmp;

	if (!stash)
		stash = ft_gc_strdup("", gc);
	if (!stash)
		return (NULL);
	buffer = ft_gc_calloc(1, sizeof(char) * BUFFER_SIZE + 1, gc);
	if (!buffer)
		return (NULL);
	len = 1;
	while (!ft_strchr(stash, '\n') && len != 0)
	{
		len = read(fd, buffer, BUFFER_SIZE);
		if (len < 0)
			return (NULL);
		buffer[len] = '\0';
		tmp = stash;
		stash = ft_gc_strjoin(tmp, buffer, gc);
		if (!stash)
			return (NULL);
	}
	return (stash);
}

static char	*extract(t_list **gc, char *stash)
{
	int		i;
	int		len;
	char	*line;

	if (!stash || stash[0] == '\0')
		return (NULL);
	len = 0;
	while (stash[len] && stash[len] != '\n')
		len++;
	if (stash[len] == '\n')
		len++;
	line = ft_gc_calloc (1, sizeof(char) * (len + 1), gc);
	if (!line)
		return (0);
	i = 0;
	while (i < len)
	{
		line[i] = stash[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*clean_stash(t_list **gc, char *stash)
{
	int		i;
	int		j;
	char	*new_stash;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (!stash[i])
		return (NULL);
	i++;
	if (stash[i] == '\0')
		return (NULL);
	new_stash = ft_gc_calloc(1, sizeof(char) * (ft_strlen(stash) - i + 1), gc);
	if (!new_stash)
		return (NULL);
	j = 0;
	while (stash[i])
		new_stash[j++] = stash[i++];
	new_stash[j] = '\0';
	return (new_stash);
}

char	*ft_gc_get_next_line(t_list **gc, int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0)
	{
		stash = NULL;
		return (NULL);
	}
	if (BUFFER_SIZE <= 0)
		return (NULL);
	stash = read_and_stash(gc, fd, stash);
	if (!stash)
		return (NULL);
	line = extract(gc, stash);
	if (!line)
		return (stash = NULL, NULL);
	stash = clean_stash(gc, stash);
	return (line);
}
