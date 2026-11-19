FROM gcc:latest

COPY worker.c /worker.c

RUN gcc /worker.c -o worker

CMD ["/worker"]
