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

## NETWORK
CARD	= $(shell ip route | grep -m1 default | cut -d ' ' -f5)
LAN		= 127.0.1.1

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
IDR = -I $(SDR)/inc -I $(LDR)/ocml/src/inc -I $(LDR)/ocl/src/inc -I/usr/include/libbson-1.0 -I/usr/include/libmongoc-1.0
LDR	= $(SDR)/lib
ELB = $(shell find $(LDR) -mindepth 1 -maxdepth 1 -not -empty -type d -printf '%f\n' | sort -k 2)
CLN = $(addprefix clean_, $(ELB))
PRG = $(addprefix purge_, $(ELB))
LXT	= .a
LIB	= $(addprefix -l, $(ELB) csfml-graphics csfml-window csfml-audio csfml-system mysqlclient sqlite3 mongoc-1.0 bson-1.0 neo4j-client) 

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
.PHONY: re db all env clean purge $(BIN)

re: clean all

db: clean
	@echo "version: '3.8'\n" > docker-compose.yml
	
	@echo "services:" >> docker-compose.yml
	
	@echo "  mongodb:" >> docker-compose.yml
	@echo "    container_name: 'mongo'" >> docker-compose.yml
	@echo "    build:" >> docker-compose.yml
	@echo "      dockerfile: 'data/db/mongo/Dockerfile'" >> docker-compose.yml
	@echo "    ports:" >> docker-compose.yml
	@echo "      - $(LAN):21000:27017" >> docker-compose.yml
	@echo "    volumes:" >> docker-compose.yml
	@echo "      - mongodata:/data/db\n" >> docker-compose.yml

	@echo "  mysql:" >> docker-compose.yml
	@echo "    container_name: 'mysql'" >> docker-compose.yml
	@echo "    build:" >> docker-compose.yml
	@echo "      dockerfile: 'data/db/mysql/Dockerfile'" >> docker-compose.yml
	@echo "    ports:" >> docker-compose.yml
	@echo "      - $(LAN):21001:33060" >> docker-compose.yml
	@echo "      - $(LAN):21002:3306" >> docker-compose.yml
	@echo "    volumes:" >> docker-compose.yml
	@echo "      - mysqldata:/var/lib/mysql\n" >> docker-compose.yml

	@echo "  neo4j:" >> docker-compose.yml
	@echo "    container_name: 'neo4j'" >> docker-compose.yml
	@echo "    build:" >> docker-compose.yml
	@echo "      dockerfile: 'data/db/neo4j/Dockerfile'" >> docker-compose.yml
	@echo "    ports:" >> docker-compose.yml
	@echo "      - $(LAN):21003:7473" >> docker-compose.yml
	@echo "      - $(LAN):21004:7474" >> docker-compose.yml
	@echo "      - $(LAN):21005:7687" >> docker-compose.yml
	@echo "    environment:" >> docker-compose.yml
	@echo "      - NEO4J_AUTH=neo4j/secret4j" >> docker-compose.yml
	@echo "    volumes:" >> docker-compose.yml
	@echo "      - neo4jdata:/data/\n" >> docker-compose.yml

	@echo "volumes:" >> docker-compose.yml
	@echo "  mongodata:" >> docker-compose.yml
	@echo "  mysqldata:" >> docker-compose.yml
	@echo "  neo4jdata:" >> docker-compose.yml
	
	@-docker compose up -d

all: db $(BIN)

clean: $(CLN)
	@-docker-compose down
	@-docker rm -fv mongo mysql neo4j
	@rm -Rf $(ODR)

purge: $(PRG) clean
	@-docker rmi -f mongo mysql punto-neo4j
	@-docker volume rm -f punto_neo4jdata punto_mongodata punto_mysqldata
	@-docker system prune -af --volumes
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