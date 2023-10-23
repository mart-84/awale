GCC=gcc
CFLAGS=-Wall -Werror -c
LDFLAGS=-Wall -Werror
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

BINDIR=bin
OBJDIR=obj
DEPDIR=dep

CLIENT=$(BINDIR)/client
SERVER=$(BINDIR)/server
TARGET=$(CLIENT) $(SERVER)

SRCS=$(wildcard */*.c)
OBJS=$(SRCS:%.c=$(OBJDIR)/%.o)

all: $(TARGET)
	@echo Build complete

$(CLIENT): $(OBJDIR)/client/client.o
$(SERVER): $(OBJDIR)/server/server.o $(OBJDIR)/server/listeclient.o

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
	gnome-terminal -- ${SERVER} && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 valery && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 georges && gnome-terminal --tab -- ${CLIENT} 127.0.0.1 jacques


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(DEPDIR)
