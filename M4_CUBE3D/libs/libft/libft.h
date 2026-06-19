/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wissalhumeau <wissalhumeau@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 10:35:26 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/31 19:30:02 by wissalhumea      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

// A Conserver
# include <unistd.h>
# include <stdlib.h>

// 1ere Partie
/* Tests de caractères et conversions */
int			ft_atoi(const char *nptr);
long		ft_atol(const char *nptr);
long long	ft_atoll(const char *nptr);
int			ft_isalnum(int c);
int			ft_isalpha(int c);
int			ft_isascii(int c);
int			ft_isdigit(int c);
int			ft_isprint(int c);
int			ft_tolower(int c);
int			ft_toupper(int c);

/* Mémoire */
void		*ft_memset(void *s, int c, size_t n);
void		ft_bzero(void *s, size_t n);
void		*ft_memcpy(void *dest, const void *src, size_t n);
void		*ft_memmove(void *dest, const void *src, size_t n);
void		*ft_memchr(const void *s, int c, size_t n);
int			ft_memcmp(const void *s1, const void *s2, size_t n);

/* Chaînes de caractères */
char		*ft_strchr(const char *s, int c);
size_t		ft_strlcat(char *dst, const char *src, size_t siz);
size_t		ft_strlcpy(char *dst, char *src, size_t siz);
size_t		ft_strlen(const char *str);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
int			ft_strcmp(const char *s1, const char *s2);
char		*ft_strrchr(const char *s, int c);
char		*ft_strnstr(const char *big, const char *little, size_t len);

/* Allocation / duplication */
void		*ft_calloc(size_t nmemb, size_t size);
char		*ft_strdup(const char *s1);

/* 2ème Partie */
char		*ft_substr(char const *s, unsigned int start, size_t len);
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_strtrim(char const *s1, char const *set);
char		**ft_split(char const *s, char c);
char		*ft_itoa(int n);
char		*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void		ft_striteri(char *s, void (*f)(unsigned int, char*));
void		ft_putchar_fd(char c, int fd);
void		ft_putstr_fd(char *s, int fd);
void		ft_putendl_fd(char *s, int fd);
void		ft_putnbr_fd(int n, int fd);

/* BONUS */

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

t_list		*ft_lstnew(void *content);
void		ft_lstadd_front(t_list **lst, t_list *new);
int			ft_lstsize(t_list *lst);
t_list		*ft_lstlast(t_list *lst);
void		ft_lstadd_back(t_list **lst, t_list *new);
void		ft_lstdelone(t_list *lst, void (*del)(void*));
void		ft_lstclear(t_list **lst, void (*del)(void*));
void		ft_lstiter(t_list *lst, void (*f)(void *));
void		ft_lstprint(t_list *lst);

/* garbage collector */
typedef struct s_gc	t_gc;

void		*ft_gc_calloc(size_t count, size_t size, t_list	**gc);
int			ft_gc_add(t_list **gc, void *ptr);
void		ft_gc_free(t_list **gc);
char		*ft_gc_strdup(const char *str, t_list **gc);
char		*ft_gc_strjoin(const char *s1, const char *s2, t_list **gc);
char		*ft_gc_substr(char const *s, unsigned int start, size_t len,
				t_list **gc);
char		*ft_gc_strdup(const char *s, t_list **gc);
char		*ft_gc_strjoin(char const *s1, char const *s2, t_list **gc);
char		*ft_gc_itoa(int n, t_list **gc);
char		**ft_gc_split(char const *s, char sep, t_list **gc);

/* get_next_line */
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char		*get_next_line(int fd);
char		*ft_gc_get_next_line(t_list **gc, int fd);

#endif
