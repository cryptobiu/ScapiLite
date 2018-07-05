FROM ubuntu:18.04
RUN apt update
RUN apt install -y libgmp-dev
RUN apt install -y liblog4cpp5-dev
RUN apt install -y libevent-2.1-6
# CMD [ "export", "LD_LIBRARY_PATH=\"/apps/HyperMPC\"" ]
ENV LD_LIBRARY_PATH=/apps/hypermpc/
RUN env
WORKDIR /apps/hypermpc
RUN ls -l
ADD apps/ /apps/
RUN ls -l
# RUN cat /proc/cpuinfo
RUN ldd HyperMPC
CMD [ "./inellipseHyperMPC" ]
