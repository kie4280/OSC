FROM fedora:latest

ARG UID=1000
ARG GID=1000
ARG USERNAME=dev

ENV DEBIAN_FRONTEND=noninteractive
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ENV ZEPHYR_SDK_INSTALL_DIR=/work/tools/zephyr-sdk
ENV STM32_CUBE_PROGRAMMER_DIR=/work/tools/STM32CubeProgrammer

# Setup a new user inside the container
RUN set -eux; \
  existing_user="$(getent passwd "${UID}" | cut -d: -f1 || true)"; \
  named_user="$(getent passwd "${USERNAME}" | cut -d: -f1 || true)"; \
  if [ -n "${existing_user}" ]; then \
  if [ -n "${named_user}" ] && [ "${named_user}" != "${existing_user}" ]; then \
  echo "user '${USERNAME}' already exists with a different UID" >&2; \
  exit 1; \
  fi; \
  if [ "${existing_user}" != "${USERNAME}" ]; then \
  usermod --login "${USERNAME}" "${existing_user}"; \
  fi; \
  if ! getent group "${GID}" >/dev/null; then \
  groupadd --gid "${GID}" "${USERNAME}"; \
  fi; \
  usermod --gid "${GID}" \
  --home "/home/${USERNAME}" \
  --move-home \
  --shell /usr/bin/fish \
  "${USERNAME}"; \
  else \
  if [ -n "${named_user}" ]; then \
  echo "user '${USERNAME}' already exists with a different UID" >&2; \
  exit 1; \
  fi; \
  if ! getent group "${GID}" >/dev/null; then \
  groupadd --gid "${GID}" "${USERNAME}"; \
  fi; \
  useradd --uid "${UID}" --gid "${GID}" -G wheel \
  --create-home --shell /usr/bin/fish "${USERNAME}"; \
  fi

# force users to change password
RUN echo "${USERNAME}:TempPassword123" | sudo chpasswd \
  && sudo passwd -e "${USERNAME}"

# Useful basic tools for an interactive Fedora container.
RUN dnf -y update \
    && dnf -y install \
    	neovim bash coreutils findutils cracklib-dicts procps-ng git fish iputils \
	make gcc-aarch64-linux-gnu gdb \
    && dnf clean all \
    && rm -rf /var/cache/dnf

RUN dnf -y upgrade

ENV PATH="/work/.venv/bin:/work/tools:/work/tools/STM32CubeProgrammer/bin:${PATH}"
ENV HOME="/home/${USERNAME}"
USER ${USERNAME}
WORKDIR /work

CMD ["/usr/bin/fish"]
