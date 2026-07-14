int	main(void)
{
	t_game	game;
	char	*s1;
	char	*s2;
	char	*joined;
	char	**split;
	char	*num;

	/* Initialisation du GC */
	game.gc = NULL;
	printf("----- AVANT allocations -----\n\n");
	print_gc(game.gc);

	/* Test ft_gc_strdup */
	s1 = ft_gc_strdup("hello", &game.gc);
	s2 = ft_gc_strdup(" world", &game.gc);
	printf("s1 = \"%s\" at %p\n", s1, (void *)s1);
	printf("s2 = \"%s\" at %p\n", s2, (void *)s2);

	/* Test ft_gc_strjoin */
	joined = ft_gc_strjoin(s1, s2, &game.gc);
	printf("joined = \"%s\" at %p\n\n", joined, (void *)joined);

	/* Test ft_gc_itoa */
	num = ft_gc_itoa(42, &game.gc);
	printf("itoa(42) = \"%s\" at %p\n\n", num, (void *)num);

	/* Test ft_gc_split */
	split = ft_gc_split("un:deux:trois", ':', &game.gc);
	printf("split[0] = \"%s\"\n", split[0]);
	printf("split[1] = \"%s\"\n", split[1]);
	printf("split[2] = \"%s\"\n\n", split[2]);
	printf("----- APRES allocations -----\n\n");
	print_gc_str(game.gc);
	print_gc(game.gc);
	print_gc_count(game.gc);

	/* Liberation de tout via le GC */
	ft_gc_free(&game.gc);
	printf("----- APRES free -----\n\n");
	print_gc(game.gc);

	/* Verification que les pointeurs ne sont plus accessibles */
	/* (ne pas déréférencer s1/s2/etc. ici, comportement indéfini) */
	printf("----- APRES AVOIR TOUT NETTOYER (doit etre NULL) -----\n\n");
	printf("game.gc = %p \n", (void *)game.gc);
	return (SUCCESS);
}