FROM    intendo/centos7_gcc5_3_boost1_60

################################################################################
# Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
################################################################################

# ##########################################################
#   Get prerequisites
# ##########################################################
RUN     yum -y install rpm-build

# ##########################################################
#   SETUP Build Environment
# ##########################################################
RUN     mkdir -p /opt/flower/bin /opt/flower/lib
ENV     PATH=/opt/flower/src:/usr/local/gcc/bin:/usr/local/bin:$PATH
ENV     LD_LIBRARY_PATH=/usr/local/boost/lib:/usr/local/gcc/lib64:/usr/local/gcc/lib:/usr/local/lib:/usr/lib:$LD_LIBRARY_PATH
ENV     CC=/usr/local/gcc/bin/gcc
ENV     CXX=/usr/local/gcc/bin/g++


# ##########################################################
#   Build FLOWER
# ##########################################################

WORKDIR /opt/flower
COPY    AUTHORS                                  \
        COPYING                                  \
        ChangeLog                                \
        INSTALL                                  \
        NEWS                                     \
        README                                   \
        VERSION                                  \
        configure.ac                             \
        Makefile.am                              \
        rebuild_from_scratch.sh                  \
        /opt/flower/
COPY    build-aux/ build-aux/
COPY    conf/      conf/
COPY    data/      data/
COPY    include/   include/
COPY    m4/        m4/
COPY    support/   support/
COPY    src/       src/
RUN     ./rebuild_from_scratch.sh
RUN     /opt/flower/src/flower/api/test/flower.test

RUN     ip a

#RUN mkdir

# build FLOWER gui
#WORKDIR /opt/flower
#RUN wget -q https://nodejs.org/dist/v6.9.2/node-v6.9.2-linux-x64.tar.xz
#WORKDIR /usr/local
#RUN tar --strip-components 1 -xf /opt/flower/node-v6.9.2-linux-x64.tar.xz
#COPY node/ /opt/flower/node
#WORKDIR /opt/flower/node/flower-gui
#RUN npm install
#
#CMD /opt/flower/src/flower -h
#EXPOSE 3000
#CMD npm start
