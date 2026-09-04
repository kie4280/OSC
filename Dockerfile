FROM fedora:latest

# Useful basic tools for an interactive Fedora container.
RUN dnf -y update \
    && dnf -y install \
    	bash coreutils findutils procps-ng git make fish \
	iputils gcc-aarch64-linux-gnu neovim \
    && dnf clean all \
    && rm -rf /var/cache/dnf

RUN dnf -y upgrade

ENV HOME=/root
WORKDIR /root

# The host directory is supplied at runtime by create-container.sh.
VOLUME ["/root"]

CMD ["/usr/bin/fish"]
