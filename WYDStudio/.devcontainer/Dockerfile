FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-ubuntu-22.04

# Evitar interação durante a instalação
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    clang \
    clang-format \
    clang-tidy \
    gdb \
    pkg-config \
    python3 \
    python3-pip \
    wget \
    curl \
    unzip \
    tar \
    mysql-server \
    mysql-client \
    libmysqlclient-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Instalar Qt6
RUN wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run \
    && chmod +x qt-unified-linux-x64-online.run \
    && ./qt-unified-linux-x64-online.run --accept-licenses --accept-obligations --no-force-installations \
        --root /opt/qt \
        --email your.email@example.com \
        --password your_password \
        --confirm-command install \
        qt.qt6.650.gcc_64 \
        qt.qt6.650.qtcharts \
    && rm qt-unified-linux-x64-online.run

# Configurar variáveis de ambiente do Qt
ENV Qt6_DIR=/opt/qt/6.5.0/gcc_64
ENV PATH=$Qt6_DIR/bin:$PATH
ENV LD_LIBRARY_PATH=$Qt6_DIR/lib:$LD_LIBRARY_PATH

# Instalar vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh \
    && ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg

# Instalar dependências via vcpkg
RUN vcpkg install \
    nlohmann-json:x64-linux \
    spdlog:x64-linux \
    fmt:x64-linux \
    openssl:x64-linux

# Configurar MySQL
RUN service mysql start \
    && mysql -e "CREATE DATABASE wydbr2;" \
    && mysql -e "CREATE USER 'wydbr2_user'@'localhost' IDENTIFIED BY 'Miz@170395';" \
    && mysql -e "GRANT ALL PRIVILEGES ON wydbr2.* TO 'wydbr2_user'@'localhost';" \
    && mysql -e "FLUSH PRIVILEGES;"

# Limpar cache
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Configurar usuário não-root
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Mudar para o usuário não-root
USER $USERNAME

# Configurar git
RUN git config --global user.email "your.email@example.com" \
    && git config --global user.name "Your Name"

# Configurar diretório de trabalho
WORKDIR /workspace

# Manter o container rodando
CMD ["sleep", "infinity"]
