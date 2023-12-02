FROM mysql
EXPOSE 3306
EXPOSE 33060

ENV MYSQL_DATABASE=Punto
ENV MYSQL_USER=admin
ENV MYSQL_PASSWORD=secret
ENV MYSQL_ROOT_PASSWORD=root
ADD data/db/mysql.sql /docker-entrypoint-initdb.d

