FROM ubuntu:latest

LABEL com.wildboarsoftware.app="quipu"
LABEL com.wildboarsoftware.major_version="8"
LABEL com.wildboarsoftware.minor_version="1"

# From: https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#run
# "Always combine RUN apt-get update with apt-get install in the same RUN statement."
RUN apt-get update && apt-get install -y build-essential git bison flex
ADD . /isode
WORKDIR /isode
ADD config/linux.h h/config.h
ADD config/linux.make config/CONFIG.make
ADD config/*.local support/

# It seems that the Makefile is not very intelligent: you still have to make
# everything so you have all the headers. But we can trim down the build by at
# least installing only quipu.
RUN ./make everything && ./make inst-everything

# Arbitrarily initialize the config and database to _something_ that will boot up.
RUN mkdir -p /usr/local/etc/isode/quipu-db/ && \
    cp -r /isode/others/quipu/quipu-db/organisation/* /usr/local/etc/isode/quipu-db/ && \
    sed -i 's/mydsaname\s*.*/mydsaname\tc=GB@cn=toucan/' /usr/local/etc/isode/quiputailor

RUN rm -rf /isode
WORKDIR /usr/local/etc/isode/

# You HAVE to use a local path to quiputailor. I don't know why.
CMD [ "/usr/local/sbin/ros.quipu", "-t", "./quiputailor" ]
