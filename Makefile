#####################################
## PROJECT      : C GAME - PUNTO   ##
## DATE         : 2023             ##
## ENVIRONEMENT : Unix             ##
## DEPENDENCIES : OCML OCL CSFML   ##
## AUTHOR       : ORNSOLOT         ##
#####################################

##################
## Project Tree ##
##################

## Binary file(s)
BDR	= .
BXT	= .exe
BIN	= $(shell basename $(shell pwd))

## Environement file(s)
EDR	= ./env
LOG = $(EDR)/$(BIN).log
ENV	= $(EDR)/$(BIN).toml

## Source file(s)
SDR	= ./src
SXT	= .c
SRC = $(shell find $(SDR) -name '*$(SXT)' ! -path '*/lib/*')

ODR	= ./obj
OXT	= .o
OBJ = $(subst $(SXT),$(OXT), $(subst $(SDR),$(ODR),$(SRC)))

## Dependencies file(s)
IDR = -I $(SDR)/inc -I $(LDR)/ocml/src/inc -I $(LDR)/ocl/src/inc
LDR	= $(SDR)/lib
ELB = $(shell find $(LDR) -mindepth 1 -maxdepth 1 -not -empty -type d -printf '%f\n' | sort -k 2)
CLN = $(addprefix clean_, $(ELB))
PRG = $(addprefix purge_, $(ELB))
LXT	= .a
LIB	= $(addprefix -l, $(ELB) csfml-graphics csfml-window csfml-audio csfml-system)

#################
## Compilation ##
#################
CC		= gcc
CCFLAGS	= -g3 -Wall -Wextra $(IDR)

LK		= gcc
LDFLAGS	= -L $(LDR)

#######################
## MAKEFILE VARIABLE ##
#######################
MAKEFLAGS	+= --no-print-directory

####################
## Makefile Rules ##
####################
.PHONY: re all env clean purge $(BIN)

re: purge all

all: $(BIN)

clean: $(CLN)
	@rm -Rf $(ODR)

purge: $(PRG) clean
	@rm -f $(BDR)/$(BIN)$(BXT)
	

$(BIN): $(ELB) $(OBJ)
	@$(LK) $(CCFLAGS) $(LDFLAGS) -o $(BDR)/$@$(BXT) $(OBJ) $(LIB) 
	@echo " └─ [✓] $@$(BXT)"

$(ELB):
	@make -C $(LDR)/$@ lib$@

$(CLN):
	@make -C $(LDR)/$(subst clean_,,$@) clean

$(PRG):
	@make -C $(LDR)/$(subst purge_,,$@) purge

$(ODR)/%.o: $(SDR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CCFLAGS) -o $@ -c $<
	@echo " ├─ [✓] $<"

#$(foreach src, $(ELB), make -C "$(LDR)/${src} clean") 