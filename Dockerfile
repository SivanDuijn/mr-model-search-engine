FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
COPY . /app
COPY /frontend/model-viewer/public/PSBDatabase /app/frontend/model-viewer/public/PSBDatabase

RUN apt-get update && apt-get install -y pip python3 python3.8-venv git clang make cmake nano

# RUN git config -f .gitmodules submodule.eigen.branch 3147391d946bb4b6c68edd901f2add6ac1f31f8c
# WORKDIR /app
# RUN git submodule update --init
# WORKDIR /app/backend/processor/submodules/pmp-library
# RUN git clone https://gitlab.com/libeigen/eigen.git/ 
# WORKDIR /app/backend/processor/submodules/pmp-library/eigen
# RUN git checkout 3147391d946bb4b6c68edd901f2add6ac1f31f8c
# WORKDIR /app/backend/processor/submodules/pmp-library
# RUN git submodule update --init

# WORKDIR /app/backend/processor
# RUN cmake -S . -B out && cmake --build out

WORKDIR /app/backend/server
RUN python3 -m venv venv
RUN . venv/bin/activate
RUN pip install flask flask_cors

CMD flask --app server run --host=0.0.0.0