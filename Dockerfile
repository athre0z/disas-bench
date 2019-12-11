FROM ubuntu:18.04
WORKDIR /root

RUN apt-get update
RUN apt-get install python3 python3-pip python3-venv make gcc-8 g++-8 clang-8 cmake -y

COPY requirements.txt .
RUN pip3 install -r requirements.txt

COPY libs ./libs
COPY input ./input
COPY bench ./bench
COPY make-all.sh .
COPY clean-all.sh .
COPY bench.py .

