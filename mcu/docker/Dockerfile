FROM ubuntu:latest

RUN apt update -y; apt upgrade -y; apt install -y \
    git                 \
    wget                \
    unzip               \
    sudo                \
    libncurses-dev      \
    flex                \
    bison               \
    gperf               \
    python              \
    python-pip          \
    python-setuptools   \
    python-serial       \
    python-click        \
    python-cryptography \
    python-future       \
    python-pyparsing    \
    python-pyelftools   \
    cmake               \
    ninja-build         \
    ccache              \
    gawk                \
    gperf               \ 
    grep                \ 
    gettext             \
    python              \
    python-dev          \
    automake            \
    bison               \
    flex                \
    texinfo             \
    help2man            \
    libtool             \
    libtool-bin         \
    make

RUN useradd -G sudo -ms /bin/bash dev
USER dev
WORKDIR /home/dev

RUN mkdir -p ~/source/espressif && cd ~/source/espressif                    && \
    git clone https://github.com/espressif/crosstool-NG.git                 && \
    cd crosstool-NG && git checkout tags/esp32-2019r1                       && \
    ./bootstrap && ./configure --enable-local && make -j                    && \
    ./ct-ng xtensa-esp32-elf                                                && \
    ./ct-ng build                                                           && \
    chmod -R u+w builds/xtensa-esp32-elf

RUN cd ~/source/espressif                                                   && \
    git clone --recursive https://github.com/espressif/esp-idf.git          && \
    pip install --user -r esp-idf/requirements.txt

RUN echo "PATH=/home/dev/source/espressif/crosstool-NG/builds/xtensa-esp32-elf/bin:$PATH" | tee -a ~/.bashrc && \
    echo "export IDF_PATH=/home/dev/source/espressif/esp-idf" | tee -a ~/.bashrc
