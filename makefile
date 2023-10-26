GCC=gcc
CFLAGS=-Wall -Werror -c -g
LDFLAGS=-Wall -Werror
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

BINDIR=bin
OBJDIR=obj
DEPDIR=dep

CLIENT=$(BINDIR)/client
SERVER=$(BINDIR)/server
TARGET=$(CLIENT) $(SERVER)

CLIENT_SRCS=client/client.c
SERVER_SRCS=server/server.c server/listes/listeClient.c server/listes/listeMatch.c server/awale/awale.c server/sauvegarde.c

SRCS=$(CLIENT_SRCS) $(SERVER_SRCS)
OBJS=$(SRCS:%.c=$(OBJDIR)/%.o)

all: $(TARGET)
	@echo Build complete

$(CLIENT): $(CLIENT_SRCS:%.c=$(OBJDIR)/%.o)
$(SERVER): $(SERVER_SRCS:%.c=$(OBJDIR)/%.o)

$(BINDIR)/%:
	@mkdir -p $(@D)
	$(GCC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d $(DEPDIR)
	@mkdir -p $(@D)
	@mkdir -p $(subst $(OBJDIR),$(DEPDIR),$(dir $@))
	$(GCC) $(DEPFLAGS) $(CFLAGS) -o $@ $<

$(DEPDIR):
	@mkdir -p $(DEPDIR)

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

.PHONY: clean start

start:
	gnome-terminal -- ${SERVER} && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 a && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 b && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 c


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(DEPDIR)
	