/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lem_in.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakrouna <rakrouna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/07 22:18:19 by rakrouna          #+#    #+#             */
/*   Updated: 2020/10/09 13:25:36 by fgarault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lem_in.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int		is_cmd(char *cmd, int section, t_farm **farm, int *io)
{
	char	**rooms;
	int		aux;

	if (cmd[0] && cmd[0] == '#' && !ft_scmp(cmd, "##end", "##start"))
		return (section);
	if ((*io < 0 && section == 0) && is_num(cmd))
		return (add_ants(farm, ft_atoi(cmd)));
	else if (section == 0)
		section = 1;
	if (section == 1)
	{
		if (!(rooms = ft_strsplit(cmd, ' ')))
			section = 2;
		if (!(aux = is_room(&(*farm)->rms, rooms, io)))
			section = 2;
		section = (aux < 0) ? -1 : section;
		free_split(rooms);
	}
	if (section == 2)
	{
		if (!is_link(farm, cmd, *io))
			section = -1;
	}
	*io = -1;
	return (section);
}

int		fill_farm(t_farm **farm, int section, int io)
{
	char		*line;
	static char	*pitcher;
	int			start;
	int			end;

	pitcher = NULL;
	start = 0;
	end = 0;
	while (section >= 0 && get_next_line2(0, &line, &pitcher) == 1)
	{
		if (io < 0 && ft_strcmp(line, "##start") == 0)
			io = 0;
		else if (io < 0 && ft_strcmp(line, "##end") == 0)
			io = 1;
		else
			section = is_cmd(line, section, farm, &io);
		if (ft_scmp(line, "##end", "##start"))
			section = repeated_endpoints(&start, &end, io, section);
#ifndef __EMSCRIPTEN__
		(section >= 0) ? ft_putendl(line) : 0;
#endif
		ft_memdel((void**)&line);
	}
	(section < 0) ? free(pitcher) : 0;
	(line) ? free(line) : 0;
	return (1);
}

void	ft_usage(void)
{
	ft_printf("USAGE: ./lem-in [--d] [--dp] < map\n\n");
	ft_printf("--d:\t\tNumber of lines\n\n");
	ft_printf("--dp:\t\tNodes information and flow\n");
	exit(0);
}

char *g_output_buffer = NULL;
size_t g_output_size = 0;
size_t g_output_capacity = 0;

void	append_output(const char *str)
{
	size_t len = strlen(str);
	if (g_output_size + len + 1 > g_output_capacity)
	{
		g_output_capacity = (g_output_size + len + 1) * 2;
		g_output_buffer = realloc(g_output_buffer, g_output_capacity);
	}
	strcpy(g_output_buffer + g_output_size, str);
	g_output_size += len;
}

void	custom_putchar(int c)
{
	char str[2] = {c, 0};
	append_output(str);
}

void	custom_putstr(const char *str)
{
	append_output(str);
}

void	custom_putendl(const char *str)
{
	append_output(str);
	append_output("\n");
}

void	custom_putnbr(int n)
{
	char buf[32];
	sprintf(buf, "%d", n);
	append_output(buf);
}

int		main_web(const char *input_map)
{
	t_farm	*farm;
	int		debug = 0;
	int		ants;
	char	*old_input;

	g_output_buffer = malloc(1);
	g_output_buffer[0] = '\0';
	g_output_size = 0;
	g_output_capacity = 1;

	old_input = stdin->_IO_read_ptr;
	
	// Redirect input from string
	FILE *mem_stream = fmemopen((void*)input_map, strlen(input_map), "r");
	if (!mem_stream) {
		append_output("ERROR: Failed to create memory stream\n");
		return -1;
	}
	
	// Hack to redirect stdin (works in emscripten)
	freopen("/dev/stdin", "w", stdin); // Close original stdin
	
	init_farm(&farm);
	
	// We need to modify fill_farm to accept string directly for web
	// For now, let's use a simpler approach
	fclose(mem_stream);
	
	// Restore original behavior for now - will be handled by JS side
	append_output("Web version initialized. Use run_lemin(map_string) from JavaScript.\n");
	
	return 0;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
const char* run_lemin(const char *input_map)
{
	t_farm	*farm;
	int		debug = 0;
	int		ants;
	
	// Reset output buffer
	if (g_output_buffer) free(g_output_buffer);
	g_output_buffer = malloc(1);
	g_output_buffer[0] = '\0';
	g_output_size = 0;
	g_output_capacity = 1;
	
	// Create temporary file from input map
	FILE *tmp = fopen("/tmp/map.txt", "w");
	if (!tmp) {
		append_output("ERROR: Cannot create temp file\n");
		return g_output_buffer;
	}
	fprintf(tmp, "%s", input_map);
	fclose(tmp);
	
	// Redirect stdin to temp file
	freopen("/tmp/map.txt", "r", stdin);
	
	init_farm(&farm);
	fill_farm(&farm, 0, -1);
	
	if (!farm->rms)
	{
		free(farm);
		append_output("ERROR:There are no rooms\n");
		return g_output_buffer;
	}
	
	ants = farm->ants;
	append_output("\n");
	solve(&farm, debug);
	farm->ants = ants;
	(debug == 2) ? print_farm(farm) : 0;
	destruct_farm(&farm);
	
	return g_output_buffer;
}

EMSCRIPTEN_KEEPALIVE
void init_web()
{
	// Override printf functions for web output capture
	// This is handled in the wrapper JS
}
#endif

int		main(int ac, char **av)
{
	t_farm	*farm;
	int		debug;
	int		ants;

#ifdef __EMSCRIPTEN__
	// Web version - main is called differently
	init_web();
	return 0;
#else
	if (ac == 1 || (ac == 2 && ft_scmp(av[1], "--d", "--dp")))
	{
		debug = (ac == 2) ? 1 : 0;
		debug = (ac == 2 && !ft_strcmp(av[1], "--dp")) ? 2 : debug;
		init_farm(&farm);
		fill_farm(&farm, 0, -1);
		if (!farm->rms)
		{
			free(farm);
			return (ft_puterr("ERROR:There are no rooms", 1));
		}
		ants = farm->ants;
		ft_putendl("");
		solve(&farm, debug);
		farm->ants = ants;
		(debug == 2) ? print_farm(farm) : 0;
		destruct_farm(&farm);
	}
	else
		ft_usage();
	return (0);
#endif
}
