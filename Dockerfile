FROM ubuntu:latest

RUN apt-get update && apt-get install -y build-essential git bison flex

ADD . /isode-8.0
RUN cd /isode-8.0 && \
    cp config/linux.h h/config.h && \
    cp config/linux.make config/CONFIG.make && \
    cp config/*.local support/ && \
    ./make everything && \
    ./make inst-everything

RUN mkdir -p /usr/local/etc/isode/quipu-db/ && \
    cp -r /isode-8.0/others/quipu/quipu-db/organisation/* /usr/local/etc/isode/quipu-db/ && \
    sed -i 's/mydsaname\s*.*/mydsaname\tc=GB@cn=toucan/' /usr/local/etc/isode/quiputailor
