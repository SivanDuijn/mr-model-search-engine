FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
COPY . /app
COPY /frontend/model-viewer/public/PSBDatabase /app/frontend/model-viewer/public/PSBDatabase

RUN git submodule update --init --recursive

RUN apt-get update && apt-get install -y pip python3 python3.8-venv git clang make cmake nano

WORKDIR /app/backend/processor
RUN cmake -S . -B out && cmake --build out

WORKDIR /app/backend/server
RUN python3 -m venv venv
RUN . venv/bin/activate
RUN pip install flask flask_cors

CMD flask --app server run --host=0.0.0.0