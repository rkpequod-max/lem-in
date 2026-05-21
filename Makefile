# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rakrouna <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/04/13 03:19:21 by rakrouna          #+#    #+#              #
#    Updated: 2020/10/09 12:42:58 by fgarault         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	lem-in

INC			=	./include/
INC2		=	./libft/include/
INCLUDE		=	-I $(INC2) -I $(INC)
SUB_MAKE	=	./libft/
INC_LIB		=	-L$(SUB_MAKE) -lft

DIROBJ		=	./depo/
DIRSRC		=	./src/
DIRFARM		=	./src/farm/
DIRGRAPH	=	./src/graph/
DIRSOLVE	=	./src/solve/

SRC			=	lem_in.c
FARM		=	farm_treatment.c valid_input.c
GRAPH		=	graph_treatment.c attrib.c list.c print.c
SOLVE		=	solve.c valid_solution.c bfs.c bfs_treatment.c flow.c flow_treatment.c path_computation.c

OAUX		=	$(SRC:%=$(DIROBJ)%) $(FARM:%=$(DIROBJ)%) $(GRAPH:%=$(DIROBJ)%) $(SOLVE:%=$(DIROBJ)%)
DEPS		=	$(OAUX:.c=.d)
OBJS		=	$(OAUX:.c=.o)

CFLAGS		=	-Wall -Wextra -Werror

CC			=	/usr/bin/gcc
RM			=	/bin/rm -f

%.o		:		../$(DIRSRC)/%.c
				$(CC) $(CFLAGS) $(INCLUDE) -MMD -o $@ -c $<

%.o		:		../$(DIRFARM)/%.c
				$(CC) $(CFLAGS) $(INCLUDE) -MMD -o $@ -c $<

%.o		:		../$(DIRGRAPH)/%.c
				$(CC) $(CFLAGS) $(INCLUDE) -MMD -o $@ -c $<

%.o		:		../$(DIRSOLVE)/%.c
				$(CC) $(CFLAGS) $(INCLUDE) -MMD -o $@ -c $<

$(NAME)	: 		submake $(OBJS)
				@$(CC) $(INCLUDE) $(CFLAGS) -o $(NAME) $(OBJS) $(INC_LIB)

clean	:
				@$(RM) -rf $(DIROBJ)
				@$(RM) $(DEPS)
				@(cd $(SUB_MAKE) && $(MAKE) clean)

all		:		$(NAME)

fclean	:		clean
				@$(RM) $(NAME)
				@(cd $(SUB_MAKE) && $(MAKE) fclean)

re		:		fclean $(NAME)

submake	:
				@mkdir -p $(DIROBJ) 
				@(cd $(SUB_MAKE) && $(MAKE))

.PHONY	: all clean fclean re
# Emscripten settings
EMSDK_PATH?=$(HOME)/emsdk
EMCC=$(EMSDK_PATH)/upstream/emscripten/emcc
WEB_OUTPUT=./docs/lem-in.js

web: submake_web
@echo "Compiling to WebAssembly..."
@$(EMCC) \
-I $(INC) -I $(INC2) \
-O2 \
-s WASM=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_FUNCTIONS='["_run_lemin", "_init_web", "_malloc", "_free"]' \
-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "stringToUTF8"]' \
-s MODULARIZE=1 \
-s EXPORT_NAME="createLemInModule" \
-s ENVIRONMENT=web \
-o $(WEB_OUTPUT) \
$(DIRSRC)lem_in.c \
$(DIRFARM)*.c \
$(DIRGRAPH)*.c \
$(DIRSOLVE)*.c \
$(SUB_MAKE)libft.a \
-D__EMSCRIPTEN__
@echo "WebAssembly build complete: $(WEB_OUTPUT)"

submake_web:
@mkdir -p $(DIROBJ)
@(cd $(SUB_MAKE) && $(MAKE))
# Add this to the end of your main Makefile

.PHONY: web

# Emscripten settings
EMSDK_PATH?=$(HOME)/emsdk
EMCC=$(EMSDK_PATH)/upstream/emscripten/emcc
WEB_OUTPUT=./docs/lem-in.js

web: submake
@echo "🔨 Compiling Lem-in to WebAssembly..."
@$(EMCC) \
-I $(INC) -I $(INC2) \
-O2 \
-s WASM=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_FUNCTIONS='["_run_lemin", "_init_web"]' \
-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "stringToUTF8"]' \
-s MODULARIZE=1 \
-s EXPORT_NAME="createLemInModule" \
-s ENVIRONMENT=web \
-o $(WEB_OUTPUT) \
$(DIRSRC)lem_in.c \
$(wildcard $(DIRFARM)*.c) \
$(wildcard $(DIRGRAPH)*.c) \
$(wildcard $(DIRSOLVE)*.c) \
$(SUB_MAKE)libft.a \
-D__EMSCRIPTEN__
@echo "✅ WebAssembly build complete!"
@ls -lh $(WEB_OUTPUT)* 2>/dev/null || echo "Files generated"
