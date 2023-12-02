# Punto

This is a Punto Game made in C with OCML (based on OCL & CSFML) librairies.

## SETUP

### Clone the Repo

```shell
git clone --recurse-submodules git@github.com:Ornsolot/Punto.git
```

### Install dependencies

Use this commande at the root of the project to install all the libraries and tool necessary to deploy, compile and use the project.

```shell
sudo ./wizard.sh $(whoami)
```

### Complie and deploy the docker's containers

Use this commande at the root of the project to deploy the docker's containers (mysql and mongo) and compile the punto game.

```shell
make all
```

or

```shell
make Punto db
```

### Make sqlite3 databse

To create the sqlite database use this command at the root of the project.

```shell
sqlite3 data/db/punto.db < data/db/sqlite.sql
```

## How to use

### KEYWORDS

```shell
$Database = sqlite, mysql, mongodb
$Player = 2 or 4
$Cycle = 1 to 2,147,483,647 (MAX_32_INT)
```

## Launch the game

Use this command at the root of the project to launch the game.

```shell
./Punto.exe play $Database $Player
./Punto.exe sqlite 2
```

## Print the Score Board

Use this command at the root of the project to print the Score board.

```shell
./Punto.exe score $Database
./Punto.exe score sqlite
```

## Launch the autofill

Use this command at the root of the project to launch the game.

```shell
./Punto.exe auto $Database $Player $Cycle
./Punto.exe auto sqlite 2 2
```

## Launch the database migration

Use this command at the root of the project to launch migration from a database to another.

```shell
./Punto.exe migrate $Database $Database
./Punto.exe migrate sqlite mysql
```
