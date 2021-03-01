if (!argv[1])
	{
		size_t *mask;

		mask = clean_calloc(countarr, sizeof(size_t), micli);
		size_t z;

		z = 0;
		while(z < countarr)
		{
			mask[z] = z;
			z++;
		}
		// z = 0;
		// while (z < countarr)
		// 	printf("MASK: %zu\n", mask[z++]);
		//order array in alphabetical reorder with bubble sort algo
		i = 0;
		while(i < countarr - 1)
		{
			j = 0;
			while (j < countarr - i - 1)
			{
				if(ft_strcmp(micli->envp[j], micli->envp[j + 1]) > 0)
				{
					tmp_envp = mask[j];
					mask[j] = mask[j + 1];
					mask[j + 1] = tmp_envp;
				}
				j++;
			}
			i++;
		}
		//this is ft_env
		i = 0;
		while (mask[i] < countarr - 1)
		{
			printf("MASK NO: %zu, MASK ARR NO: %zu declare -x %s\n", mask[i], i, micli->envp[mask[i]]);
			i++;
		}
		store = *micli->envp;
		i = 0;
		while (store)
		{
			printf("declare -x %s\n", store);
			store = *(micli->envp + ++i);
		}
		return (0);
	}
	return (0);
}
