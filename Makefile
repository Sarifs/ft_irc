# ============================
#   Makefile – Projet IRC
# ============================

# Nom de l'exécutable
NAME        = irc_server

# Dossiers
SRCDIR      = src
INCDIR      = include
OBJDIR      = obj

# Compilateur & flags
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -I$(INCDIR) -MMD -MP
LDFLAGS     += -no-pie   # ✅ Ajout ici

# Sources / Objets / Dépendances (auto)
SRCS        = $(wildcard $(SRCDIR)/*.cpp)
OBJS        = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS        = $(OBJS:.o=.d)

# Couleurs (optionnel)
GREEN       = \033[1;32m
RED         = \033[1;31m
YELLOW      = \033[1;33m
RESET       = \033[0m

# ============================
#   Règles
# ============================

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(YELLOW)🔧 Édition des liens...$(RESET)"
	@$(CXX) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✅ Build OK : $(NAME)$(RESET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "$(GREEN)✔ $<$(RESET)"

clean:
	@echo "$(RED)🧹 Suppression des objets...$(RESET)"
	@rm -f $(OBJS) $(DEPS)

fclean: clean
	@echo "$(RED)🗑 Suppression de l'exécutable...$(RESET)"
	@rm -f $(NAME)

re: fclean all

run: $(NAME)
	@./$(NAME)

-include $(DEPS)

.PHONY: all clean fclean re run
