FROM fedora:44

ARG UID=1000
ARG GID=1000
ARG USERNAME=dev

ENV DEBIAN_FRONTEND=noninteractive

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

# Allow the container user to run sudo without a password.
RUN printf '%s ALL=(ALL) NOPASSWD: ALL\n' "${USERNAME}" > "/etc/sudoers.d/${USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${USERNAME}" \
    && visudo -cf "/etc/sudoers.d/${USERNAME}"

# Force users to change password.
RUN echo "${USERNAME}:TempPassword123" | sudo chpasswd \
  && sudo passwd -e "${USERNAME}"

# Useful basic tools for an interactive Fedora container.
RUN dnf -y update \
    && dnf -y install \
      sudo neovim bash coreutils findutils cracklib-dicts procps-ng git fish iputils \
      make gcc-aarch64-linux-gnu gdb cmake git-lfs sysroot-aarch64-fc44-glibc \
    && dnf clean all \
    && rm -rf /var/cache/dnf

RUN dnf -y upgrade

ENV PATH="/work/.venv/bin:/work/tools:${PATH}"
ENV HOME="/home/${USERNAME}"
USER ${USERNAME}
WORKDIR /work

CMD ["/usr/bin/fish"]
