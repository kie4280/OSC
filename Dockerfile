FROM fedora:latest

# Useful basic tools for an interactive Fedora container.
RUN dnf -y update \
    && dnf -y install bash coreutils findutils procps-ng \
    && dnf clean all \
    && rm -rf /var/cache/dnf

ENV HOME=/root
WORKDIR /root

# The host directory is supplied at runtime by create-container.sh.
VOLUME ["/root"]

CMD ["/bin/bash"]
